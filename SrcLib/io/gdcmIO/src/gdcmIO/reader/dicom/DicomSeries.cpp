/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/make_shared.hpp>

#include <gdcmImageReader.h>
#include <gdcmScanner.h>

#include <fwTools/dateAndTime.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwData/Image.hpp>

#include "gdcmIO/helper/GdcmHelper.hpp"

#include "gdcmIO/reader/dicom/DicomImage.hpp"
#include "gdcmIO/reader/dicom/DicomSR.hpp"
#include "gdcmIO/reader/dicom/DicomRT.hpp"
#include "gdcmIO/reader/dicom/DicomSeries.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

//------------------------------------------------------------------------------

DicomSeries::DicomSeries()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSeries::~DicomSeries()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSeries::MapSeriesType DicomSeries::sortSeriesFiles(const DicomSeries::VectFilesType& seriesFiles) const
{
    // Sort DICOM files following their SOP Class UIDs
    // key : a SOP Class UID; value : file names
    MapSeriesType mapSeries;

    ::gdcm::Scanner     gScanner;
    const ::gdcm::Tag   SOPClassUID(0x0008,0x0016); // SOP Class UID
    gScanner.AddTag( SOPClassUID );
    if( !gScanner.Scan( seriesFiles ) )
    {
        OSLM_ERROR("Image search failed");
        throw ::gdcmIO::exception::Failed("Image search failed");
    }

    // Get the DICOM file names
    const VectFilesType & dicomFiles = gScanner.GetKeys();

    ::gdcm::Directory::FilenamesType::const_iterator it     = dicomFiles.begin();
    ::gdcm::Directory::FilenamesType::const_iterator itEnd  = dicomFiles.end();
    for(; it != itEnd; ++it)
    {
        const char * filename   = it->c_str();
        const char * value      = gScanner.GetValue(filename, SOPClassUID);
        if (value)
        {
            mapSeries[value].push_back(filename);
        }
        else
        {
            OSLM_ERROR ( "No SOP Class UID found in file : " << filename );
        }
    }
    return mapSeries;
}

//------------------------------------------------------------------------------

void DicomSeries::read() throw(::gdcmIO::exception::Failed)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    SLM_ASSERT("fwMedData::SeriesDB not instanced", seriesDB);

    // files which contain a common series
    const VectFilesType& seriesFiles = this->getFileNames();
    const MapSeriesType& mapSeries = this->sortSeriesFiles(seriesFiles);


    // Get the DICOM image file names
    const VectFilesType& imageFiles = this->getImageFileNames(mapSeries);
    if (!imageFiles.empty())
    {
        // Read the image
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::New();
        ::fwData::Image::sptr image = ::fwData::Image::New();
        imageSeries->setImage(image);

        ::gdcmIO::reader::dicom::DicomImage imageReader;
        imageReader.setObject( image );
        imageReader.setFileNames( imageFiles );

        ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance =
                ::boost::make_shared< ::gdcmIO::container::DicomInstance >();
        imageReader.setDicomInstance( dicomInstance );

        try
        {
            imageReader.read();
        }
        catch (::gdcmIO::exception::Failed & e)
        {
            // NOTE : if there is no image, reading is stopped.
            OSLM_ERROR(e.what());
            throw;
        }

        // Get the SR file names
        const VectFilesType& srFiles = this->getSRFileNames(mapSeries);
        if (!srFiles.empty())
        {
            ::gdcmIO::reader::dicom::DicomSR srReader;
            srReader.setObject( imageSeries->getImage() );
            srReader.setFileNames( srFiles );
            srReader.setDicomInstance( dicomInstance );

            // Read document SR
            try
            {
                srReader.read();
            }
            catch (::gdcmIO::exception::Failed & e)
            {
                //NOTE: no throw for reading error for SR and RT doc
                OSLM_ERROR("SR document reading failed: " << e.what());
            }
        }

        // Read the series
        this->setReader(imageReader.getReader());
        this->readSeries(imageSeries);
        seriesDB->getContainer().push_back(imageSeries);
    }

    // Get the RT file names
    const VectFilesType& rtFiles = this->getRTFileNames(mapSeries);
    if (!rtFiles.empty())
    {
        ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();

        ::gdcmIO::reader::dicom::DicomRT rtReader;
        rtReader.setObject( modelSeries );
        rtReader.setFileNames( rtFiles );

        // Read document RT
        try
        {
            rtReader.read();
        }
        catch (::gdcmIO::exception::Failed & e)
        {
            //NOTE: no throw for reading error for SR and RT doc
            OSLM_ERROR("RT document reading failed: " << e.what());
        }
        // Read the series
        this->setReader(rtReader.getReader());
        this->readSeries(modelSeries);
        seriesDB->getContainer().push_back(modelSeries);
    }
}

//------------------------------------------------------------------------------

void DicomSeries::readSeries(::fwMedData::Series::sptr series)
{
    const ::gdcm::DataSet& ds = this->getDataSet();

    // Series's Date
    series->setDate(helper::GdcmData::getTagValue<0x0008,0x0021>(ds));
    // Series's Time
    series->setTime(helper::GdcmData::getTagValue<0x0008,0x0031>(ds));
    // Series's descritpion
    series->setDescription(helper::GdcmData::getTagValue<0x0008,0x103e>(ds));
    // Series's UID
    series->setInstanceUID(helper::GdcmData::getTagValue<0x0020,0x000e>(ds));
    // Series's Modality
    series->setModality(helper::GdcmData::getTagValue<0x0008,0x0060>(ds));

    // Series's PerformingPhysiciansName
    std::string gdcmPhysicianNames =  helper::GdcmData::getTagValue<0x0008,0x1050>(ds);
    ::fwMedData::DicomValuesType seriesPhysicianNames;
    ::fwMedData::DicomValuesType result;
    ::boost::split( result, gdcmPhysicianNames, ::boost::is_any_of("\\"));
    seriesPhysicianNames.reserve(seriesPhysicianNames.size() + result.size());
    seriesPhysicianNames.insert(seriesPhysicianNames.end(), result.begin(), result.end());
    series->setPerformingPhysiciansName(seriesPhysicianNames);

    this->readStudy(series->getStudy());
    this->readPatient(series->getPatient());
    this->readEquipment(series->getEquipment());
}

//------------------------------------------------------------------------------

void DicomSeries::readPatient(::fwMedData::Patient::sptr patient)
{
    const ::gdcm::DataSet& ds = this->getDataSet();

    // Patient's name
    patient->setName(helper::GdcmData::getTagValue<0x0010,0x0010>(ds));
    // Patient's PatientId
    patient->setPatientId(helper::GdcmData::getTagValue<0x0010,0x0020>(ds));
    // Patient's Birthdate
    patient->setBirthdate(helper::GdcmData::getTagValue<0x0010,0x0030>(ds));
    // Patient's Sex
    patient->setSex(helper::GdcmData::getTagValue<0x0010,0x0040>(ds));
}

//------------------------------------------------------------------------------

void DicomSeries::readEquipment(::fwMedData::Equipment::sptr equipment)
{
    const ::gdcm::DataSet& ds = this->getDataSet();

    // Equipment's InstitutionName
    equipment->setInstitutionName(helper::GdcmData::getTagValue<0x0008,0x0080>(ds));
}
//------------------------------------------------------------------------------

void DicomSeries::readStudy(::fwMedData::Study::sptr study)
{
    const ::gdcm::DataSet& ds = this->getDataSet();

    // Study's InstanceUID
    study->setInstanceUID(helper::GdcmData::getTagValue<0x0020,0x000d>(ds));
    // Study's Date
    study->setDate(helper::GdcmData::getTagValue<0x0008,0x0020>(ds));
    // Study's Time
    study->setTime(helper::GdcmData::getTagValue<0x0008,0x0030>(ds));
    // Study's ReferringPhysicianName
    study->setReferringPhysicianName(helper::GdcmData::getTagValue<0x0008,0x0090>(ds));
    // Study's descritpion
    study->setDescription(helper::GdcmData::getTagValue<0x0008,0x1030>(ds));
    // Study's PatientAge
    study->setPatientAge(helper::GdcmData::getTagValue<0x0010,0x1010>(ds));
}

//------------------------------------------------------------------------------

DicomSeries::VectFilesType DicomSeries::getImageFileNames(const DicomSeries::MapSeriesType& a_map)
{
    VectFilesType imgFiles;
    BOOST_FOREACH(const MapSeriesType::value_type elt, a_map)
    {
        if(::gdcm::MediaStorage::IsImage( ::gdcm::MediaStorage::GetMSType( elt.first.c_str() ) ))
        {
            imgFiles = elt.second;
            break;
        }
    }
    return imgFiles;
}

//------------------------------------------------------------------------------

DicomSeries::VectFilesType DicomSeries::getSRFileNames(const DicomSeries::MapSeriesType& a_map)
{
    VectFilesType srFiles;
    BOOST_FOREACH(const MapSeriesType::value_type elt, a_map)
    {
        if(::gdcm::MediaStorage::GetMSType( elt.first.c_str() ) == ::gdcm::MediaStorage::EnhancedSR ||
           ::gdcm::MediaStorage::GetMSType( elt.first.c_str() ) == ::gdcm::MediaStorage::ComprehensiveSR)
        {
            srFiles = elt.second;
            break;
        }
    }
    return srFiles;
}

//------------------------------------------------------------------------------

DicomSeries::VectFilesType DicomSeries::getRTFileNames(const DicomSeries::MapSeriesType& a_map)
{
    VectFilesType rtFiles;
    BOOST_FOREACH(const MapSeriesType::value_type elt, a_map)
    {
        if(::gdcm::MediaStorage::GetMSType(elt.first.c_str()) == ::gdcm::MediaStorage::SurfaceSegmentationStorage)
        {
            rtFiles = elt.second;
            break;
        }
    }
    return rtFiles;
}

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO


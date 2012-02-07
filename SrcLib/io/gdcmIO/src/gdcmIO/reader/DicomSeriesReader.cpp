/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmImageReader.h>
#include <gdcmScanner.h>

#include <fwTools/dateAndTime.hpp>

#include "gdcmIO/reader/DicomSeriesReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/reader/DicomImageReader.hpp"
#include "gdcmIO/reader/DicomSRReader.hpp"
#include "gdcmIO/reader/DicomRTReader.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomSeriesReader::DicomSeriesReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSeriesReader::~DicomSeriesReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSeriesReader::read() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Acquisition::sptr series = this->getConcreteObject();
    SLM_ASSERT("fwData::Acquisition not instanced", series);

    //*****     Get all file names      *****//
    const std::vector< std::string > & seriesFiles = this->getFileNames(); // files which contain a common series
    OSLM_TRACE("Number of files for a series : " << seriesFiles.size());

    //*****     Divide files into groups      *****//
    ::gdcm::Scanner     gScanner;
    const ::gdcm::Tag   SOPClassUID(0x0008,0x0016); // SOP Class UID
    gScanner.AddTag( SOPClassUID );
    if( !gScanner.Scan( seriesFiles ) )
    {
        OSLM_ERROR("Image search failed");
        throw ::fwTools::Failed("Image search failed");
    }

    // Get the DICOM file names
    const std::vector< std::string > & dicomFiles = gScanner.GetKeys();
    OSLM_TRACE("Number of DICOM files : " << dicomFiles.size());

    // Sort DICOM files following their SOP Class UIDs
    std::map< std::string, std::vector< std::string > > mapSeries;  // key : a SOP Class UID; value : file names

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

    // Get the DICOM image file names
    const std::vector< std::string > imageFiles = this->getImageFileNames(mapSeries);

    if (imageFiles.size() == 0)
    {
        throw ::fwTools::Failed("No DICOM image file found");
    }

    // Read the image
    DicomImageReader imageReader;
    imageReader.setObject( series );
    imageReader.setFileNames( imageFiles );

    ::boost::shared_ptr< DicomInstance > dicomInstance = ::boost::shared_ptr< DicomInstance >( new DicomInstance );
    imageReader.setDicomInstance( dicomInstance );

    try
    {
        imageReader.read();
    }
    catch (::fwTools::Failed & e)
    {// NOTE : if there is no image, reading is stopped.
        OSLM_ERROR(e.what());
        throw;
    }

    // HERE : an image was found and properly read

    // Read the series
    this->setReader( imageReader.getReader() );
    this->readSeries();

    // Get the SR file names
    const std::vector< std::string > SRFiles = this->getSRFileNames(mapSeries);
    OSLM_TRACE("Number of files for SR documents : " << SRFiles.size());
    if (SRFiles.size() > 0)
    {
        DicomSRReader SRReader;
        SRReader.setObject( series->getImage() );
        SRReader.setFileNames( SRFiles );
        SRReader.setDicomInstance( dicomInstance );

        // Read document SR
        try
        {
            SRReader.read();
        }
        catch (::fwTools::Failed & e)
        {    //NOTE: no throw for reading error for SR and RT doc
            OSLM_ERROR("SR document reading failed");
        }
    }

    // Get the RT file names
    const std::vector< std::string > RTFiles = this->getRTFileNames(mapSeries);
    OSLM_TRACE("Number of files for RT documents : " << RTFiles.size());
    if (RTFiles.size() > 0)
    {
        DicomRTReader RTReader;
        RTReader.setObject( series );
        RTReader.setFileNames( RTFiles );

        // Read document RT
        try
        {
            RTReader.read();
        }
        catch (::fwTools::Failed & e)
        {    //NOTE: no throw for reading error for SR and RT doc
            OSLM_ERROR("RT document reading failed");
        }
    }
}

//------------------------------------------------------------------------------

void DicomSeriesReader::readSeries()
{
    SLM_TRACE_FUNC();

    // List of tag use in this method
//    const ::gdcm::Tag sTypTag(0x0008,0x0008); // Image Type
//    const ::gdcm::Tag sAcDTag(0x0008,0x0022); // Acquisition Date
//    const ::gdcm::Tag sAcTTag(0x0008,0x0032); // Acquisition Time
//    const ::gdcm::Tag sUIDTag(0x0020,0x000e); // Series Instance UID
//    const ::gdcm::Tag iPosTag(0x0020,0x0032); // Image Position (Patient)

    ::fwData::Acquisition::sptr series = this->getConcreteObject();

    const ::gdcm::DataSet &     a_ds = this->getDataSet();

    // Serie's image type
    series->setCRefImageType(helper::GdcmData::getTagValue<0x0008,0x0008>(a_ds));
    OSLM_TRACE("Serie's image type : "<<series->getImageType());

    // Serie's creation date
    {
        // Acquisition Date
        std::string acqDate = helper::GdcmData::getTagValue<0x0008,0x0022>(a_ds);

        // Acquisition Time
        std::string acqTime = helper::GdcmData::getTagValue<0x0008,0x0032>(a_ds);

        // Set the creation date
        ::boost::posix_time::ptime acqCreatDate = ::fwTools::strToBoostDateAndTime(acqDate, acqTime);
        series->setCRefCreationDate(acqCreatDate);
        OSLM_TRACE("Serie's creation date : "<<series->getCreationDate());
    }

    // Serie's descritpion
    series->setDescription(helper::GdcmData::getTagValue<0x0008,0x103e>(a_ds));
    // Serie's UID
    series->setCRefUID(helper::GdcmData::getTagValue<0x0020,0x000e>(a_ds));
    OSLM_TRACE("Serie's UID : "<<series->getUID());

    // Serie's associated file
    const std::vector< std::string > &          seriesFiles = this->getFileNames();
    std::vector< std::string >::const_iterator  itVector    = seriesFiles.begin();
    std::vector< std::string >::const_iterator  itVectorEnd = seriesFiles.end();
    for (; itVector != itVectorEnd; itVector++)
    {
        series->addDicomFileUrl( *itVector );
//        OSLM_TRACE("Serie's associated file : "<< *itVector );
    }

    const ::gdcm::Image & a_gImg = ::boost::static_pointer_cast< ::gdcm::ImageReader >( this->getReader() )->GetImage();

    // Image's bytes per pixel
    series->setBitsPerPixel( a_gImg.GetPixelFormat().GetPixelSize() * 8 );
    OSLM_TRACE("Image's bytes per pixel : "<<(int) series->getBitsPerPixel());

    // Image's position (patient)
    series->setPatientPosition( helper::GdcmData::getTagValue<0x0020,0x0032>(a_ds) );
    OSLM_TRACE("Image's position (patient) : "<<series->getPatientPosition());
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomSeriesReader::getImageFileNames(const std::map< std::string, std::vector< std::string > > a_map)
{
    std::map< std::string, std::vector< std::string > >::const_iterator itMap     = a_map.begin();
    std::map< std::string, std::vector< std::string > >::const_iterator itMapEnd  = a_map.end();
    while ( itMap != itMapEnd
         && !::gdcm::MediaStorage::IsImage( ::gdcm::MediaStorage::GetMSType( itMap->first.c_str() ) ) )
    {
        itMap++;
    }

    if (itMap != itMapEnd)
    {
        return itMap->second;
    }

    return std::vector< std::string >();    // Empty vector
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomSeriesReader::getSRFileNames(const std::map< std::string, std::vector< std::string > > a_map)
{
    std::map< std::string, std::vector< std::string > >::const_iterator itMap     = a_map.begin();
    std::map< std::string, std::vector< std::string > >::const_iterator itMapEnd  = a_map.end();
    while ( itMap != itMapEnd
//         && ::gdcm::MediaStorage::GetMSType( itMap->first.c_str() ) != ::gdcm::MediaStorage::BasicTextSR
         && ::gdcm::MediaStorage::GetMSType( itMap->first.c_str() ) != ::gdcm::MediaStorage::EnhancedSR
         && ::gdcm::MediaStorage::GetMSType( itMap->first.c_str() ) != ::gdcm::MediaStorage::ComprehensiveSR
         )
    {
        itMap++;
    }

    if (itMap != itMapEnd)
    {
        return itMap->second;
    }

    return std::vector< std::string >();    // Empty vector
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomSeriesReader::getRTFileNames(const std::map< std::string, std::vector< std::string > > a_map)
{
    std::map< std::string, std::vector< std::string > >::const_iterator itMap     = a_map.begin();
    std::map< std::string, std::vector< std::string > >::const_iterator itMapEnd  = a_map.end();
    while ( itMap != itMapEnd
         && ::gdcm::MediaStorage::GetMSType( itMap->first.c_str() ) != ::gdcm::MediaStorage::SurfaceSegmentationStorage
//         && ::gdcm::MediaStorage::GetMSType( itMap->first.c_str() ) != ::gdcm::MediaStorage::SegmentationStorage
//         && ::gdcm::MediaStorage::GetMSType( itMap->first.c_str() ) != ::gdcm::MediaStorage::RTIonPlanStorage
//         && ...
         )
    {
        itMap++;
    }

    if (itMap != itMapEnd)
    {
        return itMap->second;
    }

    return std::vector< std::string >();    // Empty vector
}

}  // namespace reader

}  // namespace gdcmIO


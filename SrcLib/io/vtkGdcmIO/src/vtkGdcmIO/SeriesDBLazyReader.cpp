/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "vtkGdcmIO/SeriesDBLazyReader.hpp"
#include "vtkGdcmIO/helper/GdcmHelper.hpp"
#include "vtkGdcmIO/helper/ImageDicomStream.hpp"

#include <fwCore/base.hpp>
#if (SPYLOG_LEVEL >= 4 )
#include <fwCore/HiResTimer.hpp>
#endif

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwVtkIO/helper/vtkLambdaCommand.hpp>
#include <fwVtkIO/vtk.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/categories.hpp>

#include <exception>
#include <gdcmImageHelper.h>
#include <gdcmImageReader.h>
#include <gdcmIPPSorter.h>
#include <gdcmIPPSorter.h>
#include <gdcmReader.h>
#include <gdcmRescaler.h>
#include <gdcmScanner.h>
#include <iosfwd>

fwDataIOReaderRegisterMacro( ::vtkGdcmIO::SeriesDBLazyReader );

namespace vtkGdcmIO
{

//------------------------------------------------------------------------------

const ::gdcm::Tag seriesUIDTag(0x0020,0x000e);
const ::gdcm::Tag seriesDateTag(0x0008,0x0021);
const ::gdcm::Tag seriesTimeTag(0x0008,0x0031);
const ::gdcm::Tag seriesTypeTag(0x0008,0x0060);
const ::gdcm::Tag seriesDescriptionTag(0x0008,0x103e);
const ::gdcm::Tag seriesPhysicianNamesTag(0x0008,0x1050);

const ::gdcm::Tag equipmentInstitutionNameTag(0x0008,0x0080);

const ::gdcm::Tag patientNameTag(0x0010,0x0010);
const ::gdcm::Tag patientIDTag(0x0010,0x0020);
const ::gdcm::Tag patientBirthdateTag(0x0010,0x0030);
const ::gdcm::Tag patientSexTag(0x0010,0x0040);

const ::gdcm::Tag studyUIDTag(0x0020,0x000d);
const ::gdcm::Tag studyDateTag(0x0008,0x0020);
const ::gdcm::Tag studyTimeTag(0x0008,0x0030);
const ::gdcm::Tag studyReferingPhysicianNameTag(0x0008,0x0090);
const ::gdcm::Tag studyDescriptionTag(0x0008,0x1030);
const ::gdcm::Tag studyPatientAgeTag(0x0010,0x1010);

const ::gdcm::Tag imageTypeTag(0x0008,0x0008);
const ::gdcm::Tag acquisitionDateTag(0x0008,0x0022);
const ::gdcm::Tag acquisitionTimeTag(0x0008,0x0032);

const ::gdcm::Tag sliceThicknessTag(0x0018,0x0050);
const ::gdcm::Tag windowCenterTag(0x0028,0x1050);
const ::gdcm::Tag windowWidthTag(0x0028,0x1051);

//------------------------------------------------------------------------------

SeriesDBLazyReader::SeriesDBLazyReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this),
    m_job(::fwJobs::Observer::New("SeriesDB reader"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDBLazyReader::~SeriesDBLazyReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SeriesDBLazyReader::createSeriesDB( const ::boost::filesystem::path &dicomDir )
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    std::vector<std::string> filenames;
    ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(dicomDir, filenames);

    this->addSeries( seriesDB, filenames);
    return seriesDB;
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::scanFiles( ::gdcm::Scanner & scanner, const std::vector< std::string > & filenames )
{
    scanner.AddTag( seriesUIDTag );
    scanner.AddTag( seriesDateTag );
    scanner.AddTag( seriesTimeTag );
    scanner.AddTag( seriesTypeTag );
    scanner.AddTag( seriesDescriptionTag );
    scanner.AddTag( seriesPhysicianNamesTag );

    scanner.AddTag( equipmentInstitutionNameTag );

    scanner.AddTag( patientNameTag );
    scanner.AddTag( patientIDTag );
    scanner.AddTag( patientBirthdateTag );
    scanner.AddTag( patientSexTag );

    scanner.AddTag( studyUIDTag );
    scanner.AddTag( studyUIDTag );
    scanner.AddTag( studyDateTag );
    scanner.AddTag( studyTimeTag );
    scanner.AddTag( studyReferingPhysicianNameTag );
    scanner.AddTag( studyDescriptionTag );
    scanner.AddTag( studyPatientAgeTag );

    scanner.AddTag( imageTypeTag );

    scanner.AddTag( acquisitionDateTag );
    scanner.AddTag( acquisitionTimeTag );

    scanner.AddTag( sliceThicknessTag );
    scanner.AddTag( windowCenterTag );
    scanner.AddTag( windowWidthTag );

    bool scanIsOk = scanner.Scan( filenames );
    FW_RAISE_IF( "Dicom scanner failed", !scanIsOk );
}

//------------------------------------------------------------------------------

SeriesDBLazyReader::MapSeriesType buildMapSeriesFromScanner( ::gdcm::Scanner & scanner )
{
    ::gdcm::Directory::FilenamesType keys               = scanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator it = keys.begin();

    SeriesDBLazyReader::MapSeriesType mapSeries;

    for(; it != keys.end(); ++it)
    {
        const char *filename = it->c_str();
        assert( scanner.IsKey( filename ) );

        const char *seriesUID = scanner.GetValue( filename, seriesUIDTag );
        const char *acqDate   = scanner.GetValue( filename, acquisitionDateTag );

        if (seriesUID)
        {
            std::string fileSetId = seriesUID;

            if (acqDate)
            {
                fileSetId += "_";
                fileSetId += acqDate;
            }

            const char *imageTypeStr = scanner.GetValue(filename, imageTypeTag);
            if(imageTypeStr)
            {
                // Treatment of secondary capture dicom file.
                std::string imageType(imageTypeStr);
                SLM_TRACE("Image Type : " + imageType);

                fileSetId += "_";
                fileSetId += imageTypeStr;
            }
            mapSeries[fileSetId].push_back(filename);

        }
        else
        {
            SLM_ERROR ( "Error in vtkGdcmIO : No serie name found in : " + *it );
        }

    }

    return mapSeries;
}

//------------------------------------------------------------------------------


std::string getValue( ::gdcm::Scanner & scanner, const std::string & dcmFile, const ::gdcm::Tag & tag )
{
    const char * tagValue = scanner.GetValue( dcmFile.c_str(), tag );
    if( tagValue )
    {
        return std::string(tagValue);
    }
    else
    {
        return std::string("");
    }
}

//------------------------------------------------------------------------------

template < typename T >
T getNumericValue( ::gdcm::Scanner & scanner, const std::string & dcmFile, const ::gdcm::Tag & tag )
{
    const char * tagValue = scanner.GetValue( dcmFile.c_str(), tag );
    if( tagValue )
    {
        std::string val (tagValue);
        ::boost::algorithm::trim(val);
        return ::boost::lexical_cast<T>( val );
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillSeries( ::gdcm::Scanner & scanner,
                                     const std::string & dcmFile, ::fwMedData::Series::sptr series )
{
    const std::string seriesUID         = getValue( scanner, dcmFile, seriesUIDTag );
    const std::string seriesTime        = getValue( scanner, dcmFile, seriesTimeTag );
    const std::string seriesDate        = getValue( scanner, dcmFile, seriesDateTag );
    const std::string seriesModality    = getValue( scanner, dcmFile, seriesTypeTag );
    const std::string seriesDescription = getValue( scanner, dcmFile, seriesDescriptionTag );

    ::fwMedData::DicomValuesType seriesPhysicianNames;
    ::gdcm::Scanner::ValuesType gdcmPhysicianNames = scanner.GetValues( seriesPhysicianNamesTag );
    for(const std::string &str :  gdcmPhysicianNames)
    {
        ::fwMedData::DicomValuesType result;
        ::boost::split( result, str, ::boost::is_any_of("\\"));
        seriesPhysicianNames.reserve(seriesPhysicianNames.size() + result.size());
        seriesPhysicianNames.insert(seriesPhysicianNames.end(), result.begin(), result.end());
    }

    SLM_ASSERT("No series UID", !seriesUID.empty() );
    series->setInstanceUID( seriesUID );
    series->setModality( seriesModality );
    series->setDescription( seriesDescription );
    series->setDate( seriesDate );
    series->setTime( seriesTime );
    series->setPerformingPhysiciansName( seriesPhysicianNames );
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillPatient( ::gdcm::Scanner & scanner,
                                      const std::string & dcmFile, ::fwMedData::Patient::sptr patient )
{
    const std::string patientName      = getValue( scanner, dcmFile, patientNameTag );
    const std::string patientId        = getValue( scanner, dcmFile, patientIDTag );
    const std::string patientBirthdate = getValue( scanner, dcmFile, patientBirthdateTag );
    const std::string patientSex       = getValue( scanner, dcmFile, patientSexTag );

    patient->setName(patientName);
    patient->setPatientId(patientId);
    patient->setBirthdate(patientBirthdate);
    patient->setSex(patientSex);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillStudy( ::gdcm::Scanner & scanner,
                                    const std::string & dcmFile, ::fwMedData::Study::sptr study )
{
    const std::string studyUID                   = getValue( scanner, dcmFile, studyUIDTag );
    const std::string studyReferingPhysicianName = getValue( scanner, dcmFile, studyReferingPhysicianNameTag );
    const std::string studyDate                  = getValue( scanner, dcmFile, studyDateTag );
    const std::string studyTime                  = getValue( scanner, dcmFile, studyTimeTag );
    const std::string studyDescription           = getValue( scanner, dcmFile, studyDescriptionTag );
    const std::string studyPatientAge            = getValue( scanner, dcmFile, studyPatientAgeTag );

    SLM_ASSERT("No study UID", !studyUID.empty() );
    study->setInstanceUID(studyUID);
    study->setDate(studyDate);
    study->setTime(studyTime);
    study->setDescription(studyDescription);
    study->setPatientAge(studyPatientAge);
    study->setReferringPhysicianName(studyReferingPhysicianName);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillEquipment( ::gdcm::Scanner & scanner,
                                        const std::string & dcmFile, ::fwMedData::Equipment::sptr equipment )
{
    const std::string equipementInstitution = getValue( scanner, dcmFile, equipmentInstitutionNameTag );

    equipment->setInstitutionName(equipementInstitution);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::preprocessImage(
    const ::fwData::Image::sptr & img,
    const SeriesDBLazyReader::SeriesFilesType & files )
{
    ::gdcm::Reader localReader;
    localReader.SetFileName( files[0].c_str() );
    localReader.Read();
    ::gdcm::File & gdcmReaderFile = localReader.GetFile();
    std::vector<double> origin    = ::gdcm::ImageHelper::GetOriginValue( gdcmReaderFile );
    std::vector<unsigned int> dim = ::gdcm::ImageHelper::GetDimensionsValue( gdcmReaderFile );
    std::vector<double> spacing   = ::gdcm::ImageHelper::GetSpacingValue( gdcmReaderFile );
    ::gdcm::PixelFormat pixelFormat = ::gdcm::ImageHelper::GetPixelFormatValue( gdcmReaderFile );
    std::vector<double> interceptSlope = ::gdcm::ImageHelper::GetRescaleInterceptSlopeValue( gdcmReaderFile );

    // Origin
    ::fwData::Image::OriginType imgOrigin;
    imgOrigin.resize(3);
    imgOrigin[0] = origin[0];
    imgOrigin[1] = origin[1];
    imgOrigin[2] = origin[2];
    img->setOrigin( imgOrigin );

    // Spacing
    ::fwData::Image::SpacingType imgSpacing;
    imgSpacing.resize(3);
    imgSpacing[0] = spacing[0];
    imgSpacing[1] = spacing[1];
    imgSpacing[2] = 1.0;
    img->setSpacing( imgSpacing );

    // Size
    ::fwData::Image::SizeType imgSize (3);
    imgSize.resize(3);
    imgSize[0] = dim[0];
    imgSize[1] = dim[1];
    imgSize[2] = files.size();

    // Image type
    ::fwTools::Type imgType;
    ::gdcm::PixelFormat::ScalarType scalarType = pixelFormat.GetScalarType();

    ::gdcm::Rescaler r;
    r.SetIntercept( interceptSlope[0] );
    r.SetSlope( interceptSlope[1] );
    r.SetPixelFormat( scalarType );
    scalarType = r.ComputeInterceptSlopePixelType();
    OSLM_TRACE("Intercept = " << interceptSlope[0] );
    OSLM_TRACE("Slope = " << interceptSlope[1] );

    switch( scalarType )
    {
        case ::gdcm::PixelFormat::UINT8:
            imgType = ::fwTools::Type::s_UINT8;
            break;
        case ::gdcm::PixelFormat::INT8:
            imgType = ::fwTools::Type::s_INT8;
            break;
        case ::gdcm::PixelFormat::UINT16:
            imgType = ::fwTools::Type::s_UINT16;
            break;
        case ::gdcm::PixelFormat::INT16:
            imgType = ::fwTools::Type::s_INT16;
            break;
        case ::gdcm::PixelFormat::UINT32:
            imgType = ::fwTools::Type::s_UINT32;
            break;
        case ::gdcm::PixelFormat::INT32:
            imgType = ::fwTools::Type::s_INT32;
            break;
        case ::gdcm::PixelFormat::FLOAT32:
            imgType = ::fwTools::Type::s_FLOAT;
            break;
        case ::gdcm::PixelFormat::FLOAT64:
            imgType = ::fwTools::Type::s_DOUBLE;
            break;
        default:
            SLM_FATAL("Type not managed");
            break;
    }

    // Number of component
    size_t numberOfComponents = pixelFormat.GetSamplesPerPixel();

    img->setSize( imgSize );
    img->setNumberOfComponents( numberOfComponents );
    img->setType( imgType );
    img->getDataArray()->resize(imgType, imgSize, numberOfComponents, false);
}

//------------------------------------------------------------------------------

SeriesDBLazyReader::SeriesFilesType sortImageSeriesFiles( const SeriesDBLazyReader::SeriesFilesType & seriesFiles )
{
    SeriesDBLazyReader::SeriesFilesType sortedSeriesFiles = seriesFiles;

    ::gdcm::IPPSorter s;
    s.SetComputeZSpacing( true );
    s.SetZSpacingTolerance( 1e-3 );
    bool success = s.Sort( seriesFiles );

    SLM_WARN_IF( "Failed to sort : " + seriesFiles[0], !success );
    if ( success )
    {
        sortedSeriesFiles = s.GetFilenames();
    }

    return sortedSeriesFiles;
}

//------------------------------------------------------------------------------

double SeriesDBLazyReader::computeZSpacing( const SeriesDBLazyReader::SeriesFilesType & seriesFiles )
{
    SLM_ASSERT("This method only work if series files contains at least 2 frames.", seriesFiles.size() > 1);
    double zspacing = 0;

    ::gdcm::Reader r1;
    ::gdcm::Reader r2;

    r1.SetFileName( seriesFiles[0].c_str() );
    r2.SetFileName( seriesFiles[1].c_str() );

    bool canRead = r1.Read() && r2.Read();
    SLM_ASSERT("Reader must be ok", canRead);

    std::vector<double> vOrigin1 = ::gdcm::ImageHelper::GetOriginValue(r1.GetFile());
    std::vector<double> vOrigin2 = ::gdcm::ImageHelper::GetOriginValue(r2.GetFile());
    zspacing = vOrigin2[2] - vOrigin1[2];

    return zspacing;
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillImage(
    ::gdcm::Scanner & scanner,
    const SeriesDBLazyReader::SeriesFilesType & seriesFiles,
    const std::string & dcmFile,
    ::fwData::Image::sptr img )
{

    this->preprocessImage(img,seriesFiles);

    double center = getNumericValue<double>(scanner, dcmFile, windowCenterTag);
    double width  = getNumericValue<double>(scanner, dcmFile, windowWidthTag);

    img->setWindowCenter(center);
    img->setWindowWidth(width);

    ::fwData::Image::SpacingType imgSpacing = img->getSpacing();
    imgSpacing.resize(3);
    double thickness = getNumericValue<double>(scanner, dcmFile, sliceThicknessTag);
    thickness = thickness ? thickness : 1.;
    if ( seriesFiles.size() > 1 )
    {
        double computedZSpacing = computeZSpacing( seriesFiles );
        imgSpacing[2] = computedZSpacing ? computedZSpacing : thickness;
    }
    else
    {
        imgSpacing[2] = thickness;
    }
    img->setSpacing(imgSpacing);


    ::vtkGdcmIO::helper::ImageDicomInfo::sptr dcmInfo = std::make_shared< ::vtkGdcmIO::helper::ImageDicomInfo >();
    dcmInfo->m_buffSizeInBytes                        = img->getSizeInBytes();
    dcmInfo->m_seriesFiles                            = seriesFiles;

    ::fwMemory::BufferObject::sptr buffObj = img->getDataArray()->getBufferObject();
    buffObj->setIStreamFactory(
        std::make_shared< ::vtkGdcmIO::helper::ImageDicomStream >( dcmInfo ),
        img->getSizeInBytes() );
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::addSeries(
    const ::fwMedData::SeriesDB::sptr &seriesDB,
    const std::vector< std::string > & filenames )
{
    //gdcm::Trace::SetDebug( 1 );
    //gdcm::Trace::SetWarning( 1 );
    //gdcm::Trace::SetError( 1 );

    try
    {

#if (SPYLOG_LEVEL >= 4 ) // Log level info
        ::fwCore::HiResTimer timer;
        timer.start();
#endif

        /// Build scanner
        ::gdcm::Scanner scanner;

        /// Scan specific tags in filenames
        this->scanFiles( scanner, filenames );

        /// Build map series
        MapSeriesType mapSeries = buildMapSeriesFromScanner( scanner );

        for( MapSeriesType::value_type mapElem :  mapSeries )
        {
            SeriesFilesType seriesFiles    = sortImageSeriesFiles( mapElem.second );
            const std::string & refDcmFile = seriesFiles[0];

            // ToDo
            // ::gdcm::MediaStorage::IsImage( ::gdcm::MediaStorage::GetMSType( XXX ) ) );
            bool isAnImage = true;
            if ( isAnImage )
            {
                /// Build new data
                ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::New();
                ::fwData::Image::sptr img             = ::fwData::Image::New();

                series->setImage( img );

                // Fill data object
                this->fillSeries( scanner, refDcmFile, series );
                this->fillPatient( scanner, refDcmFile, series->getPatient() );
                this->fillStudy( scanner, refDcmFile, series->getStudy() );
                this->fillEquipment( scanner, refDcmFile, series->getEquipment() );
                this->fillImage( scanner, seriesFiles, refDcmFile, img );

                seriesDB->getContainer().push_back(series);
            }
        }

#if (SPYLOG_LEVEL >= 4 )
        timer.stop();
        OSLM_INFO( "Time in to lazy read data : " << timer.getElapsedTimeInMilliSec() );
#endif

    }
    catch (std::exception& e)
    {
        OSLM_ERROR( "Try with another reader or retry with this reader on a specific subfolder : " << e.what() );
        std::vector< std::string >::const_iterator it = filenames.begin();
        for(; it != filenames.end(); ++it)
        {
            SLM_ERROR("file error : " + *it );
        }
    }
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::read()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    std::vector<std::string> filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(this->getFolder(), filenames);
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        for(::boost::filesystem::path file :  this->getFiles())
        {
            filenames.push_back(file.string());
        }
    }
    this->addSeries( seriesDB, filenames);
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr SeriesDBLazyReader::getJob() const
{
    return m_job;
}

} //namespace vtkGdcmIO



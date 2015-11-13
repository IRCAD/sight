/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <iosfwd>

#include <boost/iostreams/categories.hpp>

#include <exception>
#include <iostream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fwCore/base.hpp>
#if (SPYLOG_LEVEL >= 4 )
#include <fwCore/HiResTimer.hpp>
#endif

#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include <fwData/Array.hpp>

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include "fwDicomIOExt/dcmtk/helper/DicomDir.hpp"

#include <fwComEd/helper/Array.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <gdcmIPPSorter.h>
#include <gdcmScanner.h>
#include <gdcmReader.h>
#include <gdcmIPPSorter.h>

#include <vtkGdcmIO/helper/GdcmHelper.hpp>
#include "fwDicomIOExt/gdcm/DicomSeriesDBReader.hpp"


fwDataIOReaderRegisterMacro( ::fwDicomIOExt::gdcm::DicomSeriesDBReader );

namespace fwDicomIOExt
{
namespace gdcm
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

DicomSeriesDBReader::DicomSeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSeriesDBReader::~DicomSeriesDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::scanFiles( ::gdcm::Scanner & scanner, const std::vector< std::string > & filenames )
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
    FW_RAISE_IF( "Sorry, dicom scanner failed", !scanIsOk );
}

//------------------------------------------------------------------------------

DicomSeriesDBReader::MapSeriesType buildMapDicomSeriesFromScanner( ::gdcm::Scanner & scanner )
{
    ::gdcm::Directory::FilenamesType keys               = scanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator it = keys.begin();

    DicomSeriesDBReader::MapSeriesType mapSeries;

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


std::string getDicomValue( ::gdcm::Scanner & scanner, const std::string & dcmFile, const ::gdcm::Tag & tag )
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
T getDicomNumericValue( ::gdcm::Scanner & scanner, const std::string & dcmFile, const ::gdcm::Tag & tag )
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

void DicomSeriesDBReader::fillSeries( ::gdcm::Scanner & scanner,
                                      const std::string & dcmFile, ::fwMedData::Series::sptr series )
{
    const std::string seriesUID         = getDicomValue( scanner, dcmFile, seriesUIDTag );
    const std::string seriesTime        = getDicomValue( scanner, dcmFile, seriesTimeTag );
    const std::string seriesDate        = getDicomValue( scanner, dcmFile, seriesDateTag );
    const std::string seriesModality    = getDicomValue( scanner, dcmFile, seriesTypeTag );
    const std::string seriesDescription = getDicomValue( scanner, dcmFile, seriesDescriptionTag );

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

void DicomSeriesDBReader::fillPatient( ::gdcm::Scanner & scanner,
                                       const std::string & dcmFile, ::fwMedData::Patient::sptr patient )
{
    const std::string patientName      = getDicomValue( scanner, dcmFile, patientNameTag );
    const std::string patientId        = getDicomValue( scanner, dcmFile, patientIDTag );
    const std::string patientBirthdate = getDicomValue( scanner, dcmFile, patientBirthdateTag );
    const std::string patientSex       = getDicomValue( scanner, dcmFile, patientSexTag );

    patient->setName(patientName);
    patient->setPatientId(patientId);
    patient->setBirthdate(patientBirthdate);
    patient->setSex(patientSex);
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::fillStudy( ::gdcm::Scanner & scanner,
                                     const std::string & dcmFile, ::fwMedData::Study::sptr study )
{
    const std::string studyUID                   = getDicomValue( scanner, dcmFile, studyUIDTag );
    const std::string studyReferingPhysicianName = getDicomValue( scanner, dcmFile, studyReferingPhysicianNameTag );
    const std::string studyDate                  = getDicomValue( scanner, dcmFile, studyDateTag );
    const std::string studyTime                  = getDicomValue( scanner, dcmFile, studyTimeTag );
    const std::string studyDescription           = getDicomValue( scanner, dcmFile, studyDescriptionTag );
    const std::string studyPatientAge            = getDicomValue( scanner, dcmFile, studyPatientAgeTag );

    SLM_ASSERT("No study UID", !studyUID.empty() );
    study->setInstanceUID(studyUID);
    study->setDate(studyDate);
    study->setTime(studyTime);
    study->setDescription(studyDescription);
    study->setPatientAge(studyPatientAge);
    study->setReferringPhysicianName(studyReferingPhysicianName);
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::fillEquipment( ::gdcm::Scanner & scanner,
                                         const std::string & dcmFile, ::fwMedData::Equipment::sptr equipment )
{
    const std::string equipementInstitution = getDicomValue( scanner, dcmFile, equipmentInstitutionNameTag );

    equipment->setInstitutionName(equipementInstitution);
}

//------------------------------------------------------------------------------

DicomSeriesDBReader::SeriesFilesType
sortDicomSeriesFiles( const DicomSeriesDBReader::SeriesFilesType & seriesFiles )
{
    DicomSeriesDBReader::SeriesFilesType sortedSeriesFiles = seriesFiles;

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

void DicomSeriesDBReader::fillDicomSeries(
    const SeriesFilesType & seriesFiles,
    SPTR( ::fwDicomData::DicomSeries )series )
{
    series->setNumberOfInstances(seriesFiles.size());
    unsigned int count = 0;
    // Fill series
    for(const std::string &str :  seriesFiles)
    {
        ::boost::filesystem::path path(str);
        series->addDicomPath(count,path.string());
        this->notifyProgress(float(++count)/seriesFiles.size(), "Reading series...");
    }
    series->setDicomAvailability(::fwDicomData::DicomSeries::PATHS);
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::addSeries(
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
        MapSeriesType mapSeries = buildMapDicomSeriesFromScanner( scanner );

        for( MapSeriesType::value_type mapElem :  mapSeries )
        {
            SeriesFilesType seriesFiles    = sortDicomSeriesFiles( mapElem.second );
            const std::string & refDcmFile = seriesFiles[0];

            // ToDo
            // ::gdcm::MediaStorage::IsImage( ::gdcm::MediaStorage::GetMSType( XXX ) ) );
            bool isAnImage = true;
            if ( isAnImage )
            {
                /// Build new data
                ::fwDicomData::DicomSeries::sptr series = ::fwDicomData::DicomSeries::New();

                // Fill data object
                this->fillSeries( scanner, refDcmFile, series );
                this->fillPatient( scanner, refDcmFile, series->getPatient() );
                this->fillStudy( scanner, refDcmFile, series->getStudy() );
                this->fillEquipment( scanner, refDcmFile, series->getEquipment() );
                this->fillDicomSeries(seriesFiles, series);

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

void DicomSeriesDBReader::read()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr patientDB = this->getConcreteObject();
    std::vector<std::string> filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        // Try to read dicomdir file
        if(!m_isDicomdirActivated || (m_isDicomdirActivated &&
                                      !::fwDicomIOExt::dcmtk::helper::DicomDir::readDicomDir(this->getFolder(),
                                                                                             filenames)))
        {
            // Recursively search for dicom files
            ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(this->getFolder(), filenames);
        }

    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        for(::boost::filesystem::path file :  this->getFiles())
        {
            filenames.push_back(file.string());
        }
    }
    this->addSeries( patientDB, filenames);
}

//------------------------------------------------------------------------------

bool DicomSeriesDBReader::isDicomDirAvailable()
{
    std::vector<std::string> filenames;
    const bool result = ::fwDicomIOExt::dcmtk::helper::DicomDir::readDicomDir(this->getFolder(), filenames);
    return result && !filenames.empty();
}

//------------------------------------------------------------------------------

SPTR(::fwData::Array) DicomSeriesDBReader::convertToBinary(const std::string& filename)
{
    ::fwData::Array::sptr array = ::fwData::Array::New();
    ::fwComEd::helper::Array arrayHelper(array);

    FILE* file = 0;
    file = fopen(filename.c_str(), "rb");
    FW_RAISE_IF("Unable to read file.", !file);

    // obtain file size:
    fseek (file, 0, SEEK_END);
    size_t length = ftell (file);
    rewind (file);

    // Create buffer
    char *binary = new char[length];

    //Read
    size_t result = fread (binary,1,length,file);
    FW_RAISE_IF("Unable to read file.", result != length);

    //Close
    fclose(file);

    // Fill Array
    ::fwData::Array::SizeType size;
    size.push_back(length);
    arrayHelper.setBuffer(binary, true, ::fwTools::Type::create("uint8"), size, 1);

    return array;
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::setDicomdirActivated(bool value)
{
    m_isDicomdirActivated = value;
}

} // namespace gdcm
} // namespace fwDicomIOExt



/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomSeries.hpp"

#include "fwGdcmIO/helper/DicomDir.hpp"

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/spyLog.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.h>

#include <gdcmMediaStorage.h>
#include <gdcmReader.h>

#include <algorithm>

namespace fwGdcmIO
{
namespace helper
{

//Series
static const ::gdcm::Tag s_MediaStorageSOPClassUID(0x0002, 0x0002);
static const ::gdcm::Tag s_SpecificCharacterSetTag(0x0008, 0x0005);
static const ::gdcm::Tag s_SeriesInstanceUIDTag(0x0020, 0x000e);
static const ::gdcm::Tag s_SeriesDateTag(0x0008, 0x0021);
static const ::gdcm::Tag s_SeriesTimeTag(0x0008, 0x0031);
static const ::gdcm::Tag s_ModalityTag(0x0008, 0x0060);
static const ::gdcm::Tag s_SeriesDescriptionTag(0x0008, 0x103e);
static const ::gdcm::Tag s_PerformingPhysicianNameTag(0x0008, 0x1050);
static const ::gdcm::Tag s_SOPClassUIDTag(0x0008, 0x0016);
//Equipment
static const ::gdcm::Tag s_InstitutionNameTag(0x0008, 0x0080);
//Patient
static const ::gdcm::Tag s_PatientNameTag(0x0010, 0x0010);
static const ::gdcm::Tag s_PatientIDTag(0x0010, 0x0020);
static const ::gdcm::Tag s_PatientBirthDateTag(0x0010, 0x0030);
static const ::gdcm::Tag s_PatientSexTag(0x0010, 0x0040);
//Study
static const ::gdcm::Tag s_StudyInstanceUIDTag(0x0020, 0x000d);
static const ::gdcm::Tag s_StudyDateTag(0x0008, 0x0020);
static const ::gdcm::Tag s_StudyTimeTag(0x0008, 0x0030);
static const ::gdcm::Tag s_ReferringPhysicianNameTag(0x0008, 0x0090);
static const ::gdcm::Tag s_StudyDescriptionTag(0x0008, 0x1030);
static const ::gdcm::Tag s_PatientAgeTag(0x0010, 0x1010);

//------------------------------------------------------------------------------

std::string getStringValue(const ::gdcm::Scanner& scanner,
                           const std::string& filename,
                           const gdcm::Tag& tag)
{
    std::string result = "";

    const char* value = scanner.GetValue( filename.c_str(), tag );

    if(value)
    {
        // Trim buffer
        const std::string trimmedBuffer = ::gdcm::LOComp::Trim(value);
        result = trimmedBuffer;
    }

    return result;
}

// ----------------------------------------------------------------------------

DicomSeries::DicomSeries()
{
}

// ----------------------------------------------------------------------------

DicomSeries::~DicomSeries()
{
}

// ----------------------------------------------------------------------------

DicomSeries::DicomSeriesContainerType DicomSeries::read(FilenameContainerType& filenames,
                                                        const SPTR(::fwJobs::Observer)& readerObserver,
                                                        const SPTR(::fwJobs::Observer)& completeSeriesObserver)
{
    DicomSeriesContainerType seriesDB = DicomSeries::splitFiles(filenames, readerObserver);
    DicomSeries::fillSeries(seriesDB, completeSeriesObserver);
    return seriesDB;
}

//------------------------------------------------------------------------------

void DicomSeries::complete(DicomSeriesContainerType& seriesDB, const SPTR(::fwJobs::Observer)& completeSeriesObserver)
{
    ::gdcm::Scanner seriesScanner;
    seriesScanner.AddTag(s_SpecificCharacterSetTag);
    seriesScanner.AddTag(s_SeriesInstanceUIDTag);
    seriesScanner.AddTag(s_ModalityTag);
    seriesScanner.AddTag(s_SeriesDateTag);
    seriesScanner.AddTag(s_SeriesTimeTag);
    seriesScanner.AddTag(s_SeriesDescriptionTag);
    seriesScanner.AddTag(s_PerformingPhysicianNameTag);
    seriesScanner.AddTag(s_SOPClassUIDTag);

    for(auto series : seriesDB)
    {
        if(series->getLocalDicomPaths().empty())
        {
            SLM_ERROR("DicomSeries doesn't not contain any instance.");
            break;
        }

        std::vector< std::string > filenames;
        auto seriesPath            = series->getLocalDicomPaths().begin()->second;
        const std::string filename = seriesPath.string();
        filenames.push_back(filename);

        bool status = seriesScanner.Scan( filenames );
        FW_RAISE_IF("Unable to read the files.", !status);

        //Modality
        std::string modality = getStringValue( seriesScanner, filename, s_ModalityTag );
        series->setModality(modality);

        //Date
        std::string seriesDate = getStringValue( seriesScanner, filename, s_SeriesDateTag );
        series->setDate(seriesDate);

        //Time
        std::string seriesTime = getStringValue( seriesScanner, filename, s_SeriesTimeTag );
        series->setTime(seriesTime);

        //Description
        std::string seriesDescription = getStringValue( seriesScanner, filename, s_SeriesDescriptionTag );
        series->setDescription(seriesDescription);

        //Performing Physicians Name
        std::string performingPhysicianNamesStr = getStringValue( seriesScanner, filename,
                                                                  s_PerformingPhysicianNameTag );

        if(!performingPhysicianNamesStr.empty())
        {
            ::fwMedData::DicomValuesType performingPhysicianNames;
            ::boost::split( performingPhysicianNames, performingPhysicianNamesStr, ::boost::is_any_of("\\"));
            series->setPerformingPhysiciansName(performingPhysicianNames);
        }

        // Add the SOPClassUID to the series
        std::string sopClassUID = getStringValue( seriesScanner, filename, s_SOPClassUIDTag );
        ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
        sopClassUIDContainer.insert(sopClassUID);
        series->setSOPClassUIDs(sopClassUIDContainer);
    }

    this->fillSeries(seriesDB, completeSeriesObserver);

}

//------------------------------------------------------------------------------

::fwMedData::DicomSeries::sptr DicomSeries::createBlob(FilenameContainerType& filenames)
{
    ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::New();
    series->setDescription("Processing data...");
    series->setDicomAvailability(::fwMedData::DicomSeries::BLOB);
    series->setNumberOfInstances(filenames.size());

    ::fwMedData::Patient::sptr patient = ::fwMedData::Patient::New();
    patient->setName("Loading...");
    series->setPatient(patient);

    auto instanceNumber = 0;
    for(auto file : filenames)
    {
        series->addDicomPath(++instanceNumber, file);
    }

    return series;
}

//------------------------------------------------------------------------------

DicomSeries::DicomSeriesContainerType DicomSeries::splitFiles(FilenameContainerType& filenames,
                                                              const ::fwJobs::Observer::sptr& readerObserver)
{
    ::gdcm::Scanner seriesScanner;
    seriesScanner.AddTag(s_SpecificCharacterSetTag);
    seriesScanner.AddTag(s_SeriesInstanceUIDTag);
    seriesScanner.AddTag(s_ModalityTag);
    seriesScanner.AddTag(s_SeriesDateTag);
    seriesScanner.AddTag(s_SeriesTimeTag);
    seriesScanner.AddTag(s_SeriesDescriptionTag);
    seriesScanner.AddTag(s_PerformingPhysicianNameTag);
    seriesScanner.AddTag(s_SOPClassUIDTag);
    seriesScanner.AddTag(s_MediaStorageSOPClassUID);

    //readerObserver->setTotalWorkUnits(filenames.size());
    readerObserver->doneWork(0);

    std::vector< std::string > fileVec;
    for(auto file : filenames)
    {
        fileVec.push_back(file.string());
    }

    bool status = seriesScanner.Scan( fileVec );
    FW_RAISE_IF("Unable to read the files.", !status);

    ::gdcm::Directory::FilenamesType keys = seriesScanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator it;

    unsigned int progress = 0;

    DicomSeriesContainerType seriesDB;

    //Loop through every files available in the scanner
    for(const ::boost::filesystem::path& dicomFile : filenames)
    {
        auto filename = dicomFile.string();

        OSLM_ASSERT("The file \"" << dicomFile << "\" is not a key of the gdcm scanner",
                    seriesScanner.IsKey(filename.c_str()));

        const std::string sopClassUID             = getStringValue(seriesScanner, filename, s_SOPClassUIDTag);
        const std::string mediaStorageSopClassUID = getStringValue(seriesScanner, filename, s_MediaStorageSOPClassUID);

        if(sopClassUID != ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MediaStorageDirectoryStorage)
           && mediaStorageSopClassUID
           != ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MediaStorageDirectoryStorage))
        {

            this->createSeries(seriesDB, seriesScanner, dicomFile);
        }

        if (!readerObserver || readerObserver->cancelRequested())
        {
            break;
        }

        readerObserver->doneWork(static_cast<unsigned int>(++progress * 100 / keys.size()));
    }

    return seriesDB;
}

//------------------------------------------------------------------------------

void DicomSeries::fillSeries(DicomSeriesContainerType& seriesDB,
                             const ::fwJobs::Observer::sptr& completeSeriesObserver)
{
    m_patientMap.clear();
    m_studyMap.clear();
    m_equipmentMap.clear();

    ::gdcm::Scanner attributeScanner;
    attributeScanner.AddTag(s_SpecificCharacterSetTag);
    attributeScanner.AddTag(s_PatientIDTag);
    attributeScanner.AddTag(s_PatientNameTag);
    attributeScanner.AddTag(s_PatientBirthDateTag);
    attributeScanner.AddTag(s_PatientSexTag);
    attributeScanner.AddTag(s_StudyInstanceUIDTag);
    attributeScanner.AddTag(s_StudyDateTag);
    attributeScanner.AddTag(s_StudyTimeTag);
    attributeScanner.AddTag(s_ReferringPhysicianNameTag);
    attributeScanner.AddTag(s_StudyDescriptionTag);
    attributeScanner.AddTag(s_PatientAgeTag);
    attributeScanner.AddTag(s_InstitutionNameTag);
    attributeScanner.AddTag(s_SeriesInstanceUIDTag);

    unsigned int progress = 0;

    // Fill series
    for(::fwMedData::DicomSeries::sptr series : seriesDB)
    {
        // Compute number of instances
        auto size = series->getLocalDicomPaths().size();
        series->setNumberOfInstances(static_cast< unsigned int >(size));

        if(!size)
        {
            SLM_ERROR("The DicomSeries doesn't contain any instance.");
            break;
        }

        // Get first instance filename
        const std::string filename =
            series->getLocalDicomPaths().begin()->second.string();

        // Load first instance
        std::vector< std::string > firstInstanceContainer;
        firstInstanceContainer.push_back(filename);
        bool status = attributeScanner.Scan( firstInstanceContainer );
        FW_RAISE_IF("Unable to read the files.", !status);
        OSLM_ASSERT("The file \"" << filename << "\" is not a key of the gdcm scanner",
                    attributeScanner.IsKey(filename.c_str()));

        // Create data objects from first instance
        ::fwMedData::Patient::sptr patient     = this->createPatient(attributeScanner, filename);
        ::fwMedData::Study::sptr study         = this->createStudy(attributeScanner, filename);
        ::fwMedData::Equipment::sptr equipment = this->createEquipment(attributeScanner, filename);

        // Fill series
        series->setPatient(patient);
        series->setStudy(study);
        series->setEquipment(equipment);

        if(completeSeriesObserver)
        {
            completeSeriesObserver->doneWork(static_cast<unsigned int>(++progress*100/seriesDB.size() ));

            if(completeSeriesObserver->cancelRequested())
            {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void DicomSeries::createSeries(DicomSeriesContainerType& seriesDB,
                               const ::gdcm::Scanner& scanner,
                               const ::boost::filesystem::path& filename)
{
    ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::sptr();

    auto stringFilename = filename.string();

    // Get Series Instance UID
    std::string seriesInstanceUID = getStringValue( scanner, stringFilename, s_SeriesInstanceUIDTag );

    // Check if the series already exists
    for(::fwMedData::DicomSeries::sptr dicomSeries : seriesDB)
    {
        if(dicomSeries->getInstanceUID() == seriesInstanceUID)
        {
            series = dicomSeries;
            break;
        }
    }

    // If the series doesn't exist we create it
    if(!series)
    {
        series = ::fwMedData::DicomSeries::New();
        series->setDicomAvailability(::fwMedData::DicomSeries::PATHS);

        seriesDB.push_back(series);

        //Instance UID
        series->setInstanceUID(seriesInstanceUID);

        //Modality
        std::string modality = getStringValue( scanner, stringFilename, s_ModalityTag );
        series->setModality(modality);

        //Date
        std::string seriesDate = getStringValue( scanner, stringFilename, s_SeriesDateTag );
        series->setDate(seriesDate);

        //Time
        std::string seriesTime = getStringValue( scanner, stringFilename, s_SeriesTimeTag );
        series->setTime(seriesTime);

        //Description
        std::string seriesDescription = getStringValue( scanner, stringFilename, s_SeriesDescriptionTag );
        series->setDescription(seriesDescription);

        //Performing Physicians Name
        std::string performingPhysicianNamesStr = getStringValue( scanner, stringFilename,
                                                                  s_PerformingPhysicianNameTag );

        if(!performingPhysicianNamesStr.empty())
        {
            ::fwMedData::DicomValuesType performingPhysicianNames;
            ::boost::split( performingPhysicianNames, performingPhysicianNamesStr, ::boost::is_any_of("\\"));
            series->setPerformingPhysiciansName(performingPhysicianNames);
        }
    }

    // Add the SOPClassUID to the series
    std::string sopClassUID = getStringValue( scanner, stringFilename, s_SOPClassUIDTag );
    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
    sopClassUIDContainer.insert(sopClassUID);
    series->setSOPClassUIDs(sopClassUIDContainer);

    // Add the instance to the series
    auto instanceNumber = static_cast< unsigned int >(series->getLocalDicomPaths().size());
    series->addDicomPath(instanceNumber, filename);
}

//------------------------------------------------------------------------------

::fwMedData::Patient::sptr DicomSeries::createPatient(const ::gdcm::Scanner& scanner,
                                                      const std::string& filename)
{
    ::fwMedData::Patient::sptr result;

    // Get Patient ID
    std::string patientID = getStringValue( scanner, filename, s_PatientIDTag );

    // Check if the patient already exists
    if(m_patientMap.find(patientID) == m_patientMap.end())
    {
        result                  = ::fwMedData::Patient::New();
        m_patientMap[patientID] = result;

        //Patient ID
        result->setPatientId(patientID);

        //Patient Name
        std::string patientName = getStringValue( scanner, filename, s_PatientNameTag );
        result->setName(patientName);

        //Patient Birthdate
        std::string patientBirthDate = getStringValue( scanner, filename, s_PatientBirthDateTag );
        result->setBirthdate(patientBirthDate);

        //Patient Sex
        std::string patientSex = getStringValue( scanner, filename, s_PatientSexTag );
        result->setSex(patientSex);

    }
    else
    {
        result = ::fwMedData::Patient::New();
        result->deepCopy(m_patientMap[patientID]);
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Study::sptr DicomSeries::createStudy(const ::gdcm::Scanner& scanner,
                                                  const std::string& filename)
{
    ::fwMedData::Study::sptr result;

    // Get Study ID
    std::string studyInstanceUID = getStringValue( scanner, filename, s_StudyInstanceUIDTag );

    // Check if the study already exists
    if(m_studyMap.find(studyInstanceUID) == m_studyMap.end())
    {
        result                       = ::fwMedData::Study::New();
        m_studyMap[studyInstanceUID] = result;

        //Study ID
        result->setInstanceUID(studyInstanceUID);

        //Study Date
        std::string studyDate = getStringValue( scanner, filename, s_StudyDateTag );
        result->setDate(studyDate);

        //Study Time
        std::string studyTime = getStringValue( scanner, filename, s_StudyTimeTag );
        result->setTime(studyTime);

        //Referring Physician Name
        std::string referringPhysicianName = getStringValue( scanner, filename, s_ReferringPhysicianNameTag );
        result->setReferringPhysicianName(referringPhysicianName);

        //Study Description
        std::string studyDescription = getStringValue( scanner, filename, s_StudyDescriptionTag );
        result->setDescription(studyDescription);

        //Study Patient Age
        std::string patientAge = getStringValue( scanner, filename, s_PatientAgeTag );
        result->setPatientAge(patientAge);

    }
    else
    {
        result = ::fwMedData::Study::New();
        result->deepCopy(m_studyMap[studyInstanceUID]);
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Equipment::sptr DicomSeries::createEquipment(const ::gdcm::Scanner& scanner,
                                                          const std::string& filename)
{
    ::fwMedData::Equipment::sptr result;

    // Get Institution Name
    std::string institutionName = getStringValue( scanner, filename, s_InstitutionNameTag );

    // Check if the equipment already exists
    if(m_equipmentMap.find(institutionName) == m_equipmentMap.end())
    {
        result                          = ::fwMedData::Equipment::New();
        m_equipmentMap[institutionName] = result;

        //Institution Name
        result->setInstitutionName(institutionName);

    }
    else
    {
        result = ::fwMedData::Equipment::New();
        result->deepCopy(m_equipmentMap[institutionName]);
    }

    return result;
}

//------------------------------------------------------------------------------

} //helper
} //fwGdcmIO

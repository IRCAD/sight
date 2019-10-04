/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
#include <boost/regex.h>

#include <filesystem>
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
    const char* value  = scanner.GetValue( filename.c_str(), tag );
    if(value)
    {
        // Trim buffer
        result = ::gdcm::LOComp::Trim(value);
    }
    return result;
}

//------------------------------------------------------------------------------

std::string getStringValue(const ::gdcm::DataSet& dataset,
                           const gdcm::Tag& tag)
{
    std::string result = "";
    if (dataset.FindDataElement(tag))
    {
        const ::gdcm::DataElement& dataElement = dataset.GetDataElement(tag);

        if (!dataElement.IsEmpty())
        {
            // Retrieve buffer
            const ::gdcm::ByteValue* bv = dataElement.GetByteValue();
            if(bv)
            {
                std::string buffer(bv->GetPointer(), bv->GetLength());
                // Trim buffer
                result = ::gdcm::LOComp::Trim(buffer.c_str());
            }
        }
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
    std::set< ::gdcm::Tag > selectedtags;
    selectedtags.insert( s_SpecificCharacterSetTag);
    selectedtags.insert( s_SeriesInstanceUIDTag);
    selectedtags.insert( s_ModalityTag);
    selectedtags.insert( s_SeriesDateTag);
    selectedtags.insert( s_SeriesTimeTag);
    selectedtags.insert( s_SeriesDescriptionTag);
    selectedtags.insert( s_PerformingPhysicianNameTag);
    selectedtags.insert( s_SOPClassUIDTag);

    for(const auto& series : seriesDB)
    {
        if(series->getDicomContainer().empty())
        {
            SLM_ERROR("DicomSeries doesn't not contain any instance.");
            break;
        }
        const auto& firstItem                                  = series->getDicomContainer().begin();
        const ::fwMemory::BufferObject::sptr bufferObj         = firstItem->second;
        const ::fwMemory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
        SPTR(std::istream) is = streamInfo.stream;

        ::gdcm::Reader reader;
        reader.SetStream(*is);

        if(!reader.ReadSelectedTags(selectedtags))
        {
            FW_RAISE("Unable to read Dicom file '"<< bufferObj->getStreamInfo().fsFile.string() <<"' "<<
                     "(slice: '" << firstItem->first << "')");
        }
        const ::gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

        //Modality
        std::string modality = getStringValue( dataset, s_ModalityTag );
        series->setModality(modality);

        //Date
        std::string seriesDate = getStringValue( dataset, s_SeriesDateTag );
        series->setDate(seriesDate);

        //Time
        std::string seriesTime = getStringValue( dataset, s_SeriesTimeTag );
        series->setTime(seriesTime);

        //Description
        std::string seriesDescription = getStringValue( dataset, s_SeriesDescriptionTag );
        series->setDescription(seriesDescription);

        //Performing Physicians Name
        std::string performingPhysicianNamesStr = getStringValue( dataset, s_PerformingPhysicianNameTag );

        if(!performingPhysicianNamesStr.empty())
        {
            ::fwMedData::DicomValuesType performingPhysicianNames;
            ::boost::split( performingPhysicianNames, performingPhysicianNamesStr, ::boost::is_any_of("\\"));
            series->setPerformingPhysiciansName(performingPhysicianNames);
        }

        // Add the SOPClassUID to the series
        std::string sopClassUID = getStringValue( dataset, s_SOPClassUIDTag );
        ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
        sopClassUIDContainer.insert(sopClassUID);
        series->setSOPClassUIDs(sopClassUIDContainer);
    }

    this->fillSeries(seriesDB, completeSeriesObserver);
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

    readerObserver->setTotalWorkUnits(filenames.size());
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
    for(const std::filesystem::path& dicomFile : filenames)
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

        readerObserver->doneWork(static_cast< std::uint64_t >(++progress * 100 / keys.size()));
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

    std::set< ::gdcm::Tag > selectedtags;
    selectedtags.insert(s_SpecificCharacterSetTag);
    selectedtags.insert(s_PatientIDTag);
    selectedtags.insert(s_PatientNameTag);
    selectedtags.insert(s_PatientBirthDateTag);
    selectedtags.insert(s_PatientSexTag);
    selectedtags.insert(s_StudyInstanceUIDTag);
    selectedtags.insert(s_StudyDateTag);
    selectedtags.insert(s_StudyTimeTag);
    selectedtags.insert(s_ReferringPhysicianNameTag);
    selectedtags.insert(s_StudyDescriptionTag);
    selectedtags.insert(s_PatientAgeTag);
    selectedtags.insert(s_InstitutionNameTag);
    selectedtags.insert(s_SeriesInstanceUIDTag);

    std::uint64_t progress = 0;

    // Fill series
    for(const ::fwMedData::DicomSeries::sptr& series : seriesDB)
    {
        // Compute number of instances
        const size_t size = series->getDicomContainer().size();
        series->setNumberOfInstances(size);

        if(!size)
        {
            SLM_ERROR("The DicomSeries doesn't contain any instance.");
            break;
        }

        // Load first instance
        const auto& firstItem                                  = series->getDicomContainer().begin();
        const ::fwMemory::BufferObject::sptr bufferObj         = firstItem->second;
        const ::fwMemory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
        SPTR(std::istream) is = streamInfo.stream;

        ::gdcm::Reader reader;
        reader.SetStream(*is);

        if(!reader.ReadSelectedTags(selectedtags))
        {
            FW_RAISE("Unable to read Dicom file '"<< bufferObj->getStreamInfo().fsFile.string() <<"' "<<
                     "(slice: '" << firstItem->first << "')");
        }
        const ::gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

        // Create data objects from first instance
        ::fwMedData::Patient::sptr patient     = this->createPatient(dataset);
        ::fwMedData::Study::sptr study         = this->createStudy(dataset);
        ::fwMedData::Equipment::sptr equipment = this->createEquipment(dataset);

        // Fill series
        series->setPatient(patient);
        series->setStudy(study);
        series->setEquipment(equipment);

        if(completeSeriesObserver)
        {
            completeSeriesObserver->doneWork(static_cast<std::uint64_t>(++progress * 100 / seriesDB.size() ));

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
                               const std::filesystem::path& filename)
{
    ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::sptr();

    const std::string stringFilename = filename.string();

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
    const size_t instanceNumber = series->getDicomContainer().size();
    series->addDicomPath(instanceNumber, filename);
}

//------------------------------------------------------------------------------

::fwMedData::Patient::sptr DicomSeries::createPatient(const ::gdcm::DataSet& dataset)
{
    ::fwMedData::Patient::sptr result;

    // Get Patient ID
    std::string patientID = getStringValue( dataset, s_PatientIDTag );

    // Check if the patient already exists
    if(m_patientMap.find(patientID) == m_patientMap.end())
    {
        result                  = ::fwMedData::Patient::New();
        m_patientMap[patientID] = result;

        //Patient ID
        result->setPatientId(patientID);

        //Patient Name
        std::string patientName = getStringValue( dataset, s_PatientNameTag );
        result->setName(patientName);

        //Patient Birthdate
        std::string patientBirthDate = getStringValue( dataset, s_PatientBirthDateTag );
        result->setBirthdate(patientBirthDate);

        //Patient Sex
        std::string patientSex = getStringValue( dataset, s_PatientSexTag );
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

::fwMedData::Study::sptr DicomSeries::createStudy(const ::gdcm::DataSet& dataset)
{
    ::fwMedData::Study::sptr result;

    // Get Study ID
    std::string studyInstanceUID = getStringValue( dataset, s_StudyInstanceUIDTag );

    // Check if the study already exists
    if(m_studyMap.find(studyInstanceUID) == m_studyMap.end())
    {
        result                       = ::fwMedData::Study::New();
        m_studyMap[studyInstanceUID] = result;

        //Study ID
        result->setInstanceUID(studyInstanceUID);

        //Study Date
        std::string studyDate = getStringValue( dataset, s_StudyDateTag );
        result->setDate(studyDate);

        //Study Time
        std::string studyTime = getStringValue( dataset, s_StudyTimeTag );
        result->setTime(studyTime);

        //Referring Physician Name
        std::string referringPhysicianName = getStringValue( dataset, s_ReferringPhysicianNameTag );
        result->setReferringPhysicianName(referringPhysicianName);

        //Study Description
        std::string studyDescription = getStringValue( dataset, s_StudyDescriptionTag );
        result->setDescription(studyDescription);

        //Study Patient Age
        std::string patientAge = getStringValue( dataset, s_PatientAgeTag );
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

::fwMedData::Equipment::sptr DicomSeries::createEquipment(const ::gdcm::DataSet& dataset)
{
    ::fwMedData::Equipment::sptr result;

    // Get Institution Name
    std::string institutionName = getStringValue( dataset, s_InstitutionNameTag );

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

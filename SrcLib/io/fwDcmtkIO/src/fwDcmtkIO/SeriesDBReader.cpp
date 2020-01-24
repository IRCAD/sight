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

#include "fwDcmtkIO/SeriesDBReader.hpp"

#include "fwDcmtkIO/helper/Codec.hpp"
#include "fwDcmtkIO/helper/DicomDir.hpp"
#include "fwDcmtkIO/helper/DicomSearch.hpp"
#include "fwDcmtkIO/reader/ImageStorageReader.hpp"

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwDicomIOFilter/composite/CTImageStorageDefaultComposite.hpp>
#include <fwDicomIOFilter/exceptions/FilterFailure.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/splitter/SOPClassUIDSplitter.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmnet/diutil.h>

fwDataIOReaderRegisterMacro( ::fwDcmtkIO::SeriesDBReader );

namespace fwDcmtkIO
{

//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this),
    m_isDicomdirActivated(false)
{
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
}

//------------------------------------------------------------------------------

SeriesDBReader::FilenameContainerType SeriesDBReader::getFilenames()
{
    FilenameContainerType filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        // Try to read dicomdir file
        if(!m_isDicomdirActivated || (m_isDicomdirActivated &&
                                      !::fwDcmtkIO::helper::DicomDir::readDicomDir(this->getFolder(), filenames)))
        {
            // Recursively search for dicom files
            ::fwDcmtkIO::helper::DicomSearch::searchRecursively(this->getFolder(), filenames);
        }
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        for(std::filesystem::path file: this->getFiles())
        {
            filenames.push_back(file.string());
        }
    }

    return filenames;
}

//------------------------------------------------------------------------------

void SeriesDBReader::read()
{
    SLM_TRACE_FUNC();

    // Get filenames
    FilenameContainerType filenames = this->getFilenames();

    // Read Dicom Series
    this->addSeries(filenames);

    // Apply Default filters
    if(!m_dicomFilterType.empty())
    {
        ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(m_dicomFilterType);
        SLM_ASSERT("Failed to instantiate filter of type '" + m_dicomFilterType + "'.", filter);
        ::fwDicomIOFilter::helper::Filter::applyFilter(m_dicomSeriesContainer, filter, true);
    }

    // Read series
    for(::fwMedData::DicomSeries::csptr series: m_dicomSeriesContainer)
    {
        this->convertDicomSeries(series);
    }
}

//------------------------------------------------------------------------------

void SeriesDBReader::readFromDicomSeriesDB(::fwMedData::SeriesDB::csptr dicomSeriesDB,
                                           ::fwServices::IService::sptr notifier)
{
    // Read series
    for(const ::fwMedData::Series::csptr& series : dicomSeriesDB->getContainer())
    {
        ::fwMedData::DicomSeries::csptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(series);
        OSLM_ASSERT("Trying to read a series which is not a DicomSeries.", dicomSeries);
        this->convertDicomSeries(dicomSeries, notifier);
    }
}

//------------------------------------------------------------------------------

void SeriesDBReader::readDicomSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);

    // Get filenames
    FilenameContainerType filenames = this->getFilenames();

    // Read Dicom Series
    this->addSeries(filenames);

    // Push Dicom Series
    for(::fwMedData::DicomSeries::sptr series: m_dicomSeriesContainer)
    {
        seriesDBHelper.add(series);
    }
}

//------------------------------------------------------------------------------

bool SeriesDBReader::isDicomDirAvailable()
{
    std::vector<std::string> filenames;
    bool result = ::fwDcmtkIO::helper::DicomDir::readDicomDir(this->getFolder(), filenames);
    return result && !filenames.empty();
}

//------------------------------------------------------------------------------

void SeriesDBReader::addSeries(const std::vector< std::string >& filenames)
{
    for(const std::string& filename : filenames)
    {
        DcmFileFormat fileFormat;
        OFCondition status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());

        DcmDataset* dataset = fileFormat.getDataset();

        // Create Series
        this->createSeries(dataset, filename);
    }

    // Fill series
    for(const ::fwMedData::DicomSeries::sptr& series : m_dicomSeriesContainer)
    {
        // Compute number of instances
        series->setNumberOfInstances(series->getDicomContainer().size());

        // Get first instance
        const auto& bufferObj = series->getDicomContainer().begin()->second;
        const size_t buffSize = bufferObj->getSize();
        ::fwMemory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast< char* >( lock.getBuffer() );

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        // Load first instance
        DcmFileFormat fileFormat;
        fileFormat.transferInit();
        if (!fileFormat.read(is).good())
        {
            FW_RAISE("Unable to read Dicom file '"<< bufferObj->getStreamInfo().fsFile.string() <<"'");
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        DcmDataset* dataset = fileFormat.getDataset();

        // Create data objects from first instance
        ::fwMedData::Patient::sptr patient     = this->createPatient(dataset);
        ::fwMedData::Study::sptr study         = this->createStudy(dataset);
        ::fwMedData::Equipment::sptr equipment = this->createEquipment(dataset);

        // Fill series
        series->setPatient(patient);
        series->setStudy(study);
        series->setEquipment(equipment);
    }
}

//------------------------------------------------------------------------------

::fwMedData::Patient::sptr SeriesDBReader::createPatient(DcmDataset* dataset)
{
    ::fwMedData::Patient::sptr result;
    OFString data;

    // Get Patient ID
    dataset->findAndGetOFStringArray(DCM_PatientID, data);
    ::std::string patientID = data.c_str();

    // Check if the patient already exists
    if(m_patientMap.find(patientID) == m_patientMap.end())
    {
        result                  = ::fwMedData::Patient::New();
        m_patientMap[patientID] = result;

        //Patient ID
        result->setPatientId(patientID);

        //Patient Name
        dataset->findAndGetOFStringArray(DCM_PatientName, data);
        result->setName(data.c_str());

        //Patient Birthday
        dataset->findAndGetOFStringArray(DCM_PatientBirthDate, data);
        result->setBirthdate(data.c_str());

        //Patient Sex
        dataset->findAndGetOFStringArray(DCM_PatientSex, data);
        result->setSex(data.c_str());

    }
    else
    {
        result = m_patientMap[patientID];
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Study::sptr SeriesDBReader::createStudy(DcmDataset* dataset)
{
    ::fwMedData::Study::sptr result;
    OFString data;

    // Get Study ID
    dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, data);
    ::std::string studyID = data.c_str();

    // Check if the study already exists
    if(m_studyMap.find(studyID) == m_studyMap.end())
    {
        result              = ::fwMedData::Study::New();
        m_studyMap[studyID] = result;

        //Study ID
        result->setInstanceUID(studyID);

        //Study Date
        dataset->findAndGetOFStringArray(DCM_StudyDate, data);
        result->setDate(data.c_str());

        //Study Time
        dataset->findAndGetOFStringArray(DCM_StudyTime, data);
        result->setTime(data.c_str());

        //Referring Physician Name
        dataset->findAndGetOFStringArray(DCM_ReferringPhysicianName, data);
        result->setReferringPhysicianName(data.c_str());

        //Study Description
        dataset->findAndGetOFStringArray(DCM_StudyDescription, data);
        result->setDescription(data.c_str());

        //Study Patient Age
        dataset->findAndGetOFStringArray(DCM_PatientAge, data);
        result->setPatientAge(data.c_str());

    }
    else
    {
        result = m_studyMap[studyID];
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Equipment::sptr SeriesDBReader::createEquipment(DcmDataset* dataset)
{
    ::fwMedData::Equipment::sptr result;
    OFString data;

    // Get Institution Name
    dataset->findAndGetOFStringArray(DCM_InstitutionName, data);
    ::std::string institutionName = data.c_str();

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
        result = m_equipmentMap[institutionName];
    }

    return result;
}

//------------------------------------------------------------------------------

void SeriesDBReader::createSeries(DcmDataset* dataset, const std::string& filename)
{
    ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::sptr();
    OFString data;

    // Get Series Instance UID
    dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, data);
    std::string seriesInstanceUID = data.c_str();

    // Check if the series already exists
    for(const ::fwMedData::DicomSeries::sptr& dicomSeries : m_dicomSeriesContainer)
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
        m_dicomSeriesContainer.push_back(series);

        //Instance UID
        series->setInstanceUID(seriesInstanceUID);

        //Modality
        dataset->findAndGetOFStringArray(DCM_Modality, data);
        series->setModality(data.c_str());

        //Date
        dataset->findAndGetOFStringArray(DCM_SeriesDate, data);
        series->setDate(data.c_str());

        //Time
        dataset->findAndGetOFStringArray(DCM_SeriesTime, data);
        series->setTime(data.c_str());

        //Description
        dataset->findAndGetOFStringArray(DCM_SeriesDescription, data);
        series->setDescription(data.c_str());

        //Performing Physicians Name
        std::vector<std::string> performingPhysiciansName;
        for(int i = 0; dataset->findAndGetOFString(DCM_PerformingPhysicianName, data, i).good(); ++i)
        {
            performingPhysiciansName.push_back(data.c_str());
        }
        series->setPerformingPhysiciansName(performingPhysiciansName);
    }

    // Add the SOPClassUID to the series
    dataset->findAndGetOFStringArray(DCM_SOPClassUID, data);
    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
    sopClassUIDContainer.insert(data.c_str());
    series->setSOPClassUIDs(sopClassUIDContainer);

    // Add the instance to the series
    const std::size_t instanceNumber = series->getDicomContainer().size();
    series->addDicomPath(instanceNumber, filename);
}

//------------------------------------------------------------------------------

void SeriesDBReader::convertDicomSeries(::fwMedData::DicomSeries::csptr dicomSeries,
                                        ::fwServices::IService::sptr notifier)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);
    ::fwMedData::Series::sptr result = ::fwMedData::Series::sptr();

    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
    FW_RAISE_IF("The series contains several SOPClassUIDs. Try to apply a filter in order to split the series.",
                sopClassUIDContainer.size() != 1);
    std::string sopClassUID = sopClassUIDContainer.begin()->c_str();

    const SupportedSOPClassContainerType::iterator bIt = m_supportedSOPClassContainer.begin();
    const SupportedSOPClassContainerType::iterator eIt = m_supportedSOPClassContainer.end();

    if(m_supportedSOPClassContainer.empty() || std::find(bIt, eIt, sopClassUID) != eIt)
    {
        ::fwDcmtkIO::reader::ImageStorageReader reader;
        result = reader.read(dicomSeries);
    }

    if(result)
    {
        // Add the series to the DB
        seriesDBHelper.add(result);
    }
    else
    {
        OSLM_WARN("\""+sopClassUID+"\" SOPClassUID is not supported.");
    }

    if(notifier)
    {
        seriesDBHelper.notify();
    }

}

//------------------------------------------------------------------------------

SeriesDBReader::DicomSeriesContainerType& SeriesDBReader::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

} //namespace fwDcmtkIO

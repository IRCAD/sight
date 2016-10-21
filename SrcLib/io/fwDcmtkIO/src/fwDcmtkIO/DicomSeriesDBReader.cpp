/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDcmtkIO/DicomSeriesDBReader.hpp"

#include "fwDcmtkIO//helper/Codec.hpp"
#include "fwDcmtkIO/helper/DicomDir.hpp"
#include "fwDcmtkIO/helper/DicomSearch.hpp"
#include "fwDcmtkIO/reader/ImageTagReader.hpp"
#include "fwDcmtkIO/reader/MPPSReader.hpp"

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwDcmtkTools/Dictionary.hpp>

#include <fwDicomIOFilter/composite/CTImageStorageDefaultComposite.hpp>
#include <fwDicomIOFilter/exceptions/FilterFailure.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/splitter/SOPClassUIDSplitter.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmnet/diutil.h>


fwDataIOReaderRegisterMacro( ::fwDcmtkIO::DicomSeriesDBReader );

namespace fwDcmtkIO
{

//------------------------------------------------------------------------------

DicomSeriesDBReader::DicomSeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this),
    m_isDicomdirActivated(false)
{
    SLM_TRACE_FUNC();

    // Load dictionary
    ::fwDcmtkTools::Dictionary::loadDictionary();

    // Register codecs
    ::fwDcmtkIO::helper::Codec::registerCodecs();

    // Added to avoid parsing error while reading radioscopic/fluoroscopic dicoms.
    dcmIgnoreParsingErrors.set(OFTrue);
}

//------------------------------------------------------------------------------

DicomSeriesDBReader::~DicomSeriesDBReader()
{
    // Clean up codecs
    ::fwDcmtkIO::helper::Codec::cleanup();
}

//------------------------------------------------------------------------------

DicomSeriesDBReader::FilenameContainerType DicomSeriesDBReader::getFilenames()
{
    FilenameContainerType filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        // Try to read dicomdir file
        if(!m_isDicomdirActivated || (m_isDicomdirActivated &&
                                      !::fwDcmtkIO::helper::DicomDir::readDicomDir(this->getFolder(),
                                                                                   filenames)))
        {
            // Recursively search for dicom files
            try
            {
                ::fwDcmtkIO::helper::DicomSearch::searchRecursively(this->getFolder(), filenames);
            }
            catch(const std::runtime_error& e)
            {
                throw e;
            }
        }
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        for(::boost::filesystem::path file :  this->getFiles())
        {
            filenames.push_back(file.string());
        }
    }

    return filenames;
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::read()
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
        ::fwDicomIOFilter::helper::Filter::applyFilter(m_dicomSeriesContainer,filter, true);
    }

    // Read series
    for(::fwMedData::DicomSeries::sptr series :  m_dicomSeriesContainer)
    {
        this->convertDicomSeries(series);
    }
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::readFromDicomSeriesDB(::fwMedData::SeriesDB::sptr dicomSeriesDB)
{
    // Read series
    for(::fwMedData::Series::sptr series :  dicomSeriesDB->getContainer())
    {
        ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(series);
        OSLM_ASSERT("Trying to read a series which is not a DicomSeries.", dicomSeries);
        this->convertDicomSeries(dicomSeries, true);
    }
}

//------------------------------------------------------------------------------

bool DicomSeriesDBReader::isDicomDirAvailable()
{
    std::vector<std::string> filenames;
    bool result = ::fwDcmtkIO::helper::DicomDir::readDicomDir(this->getFolder(), filenames);
    return result && !filenames.empty();
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::addSeries(const std::vector< std::string >& filenames)
{
    DcmFileFormat fileFormat;
    for(std::string filename :  filenames)
    {
        OFCondition status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());

        DcmDataset* dataset = fileFormat.getDataset();

        // Create Series
        this->createSeries(dataset, filename);

    }

    // Fill series
    for(::fwMedData::DicomSeries::sptr series :  m_dicomSeriesContainer)
    {
        // Compute number of instances
        series->setNumberOfInstances(series->getLocalDicomPaths().size());

        // Get first instance filename
        const std::string filename = series->getLocalDicomPaths().begin()->second.string();

        // Load first instance
        DcmFileFormat fileFormat;
        OFCondition status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());
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

::fwMedData::Patient::sptr DicomSeriesDBReader::createPatient(DcmDataset* dataset)
{
    ::fwMedData::Patient::sptr result;
    OFString data;

    // Get Patient ID
    dataset->findAndGetOFString(DCM_PatientID,data);
    ::std::string patientID = data.c_str();

    // Check if the patient already exists
    if(m_patientMap.find(patientID) == m_patientMap.end())
    {
        result                  = ::fwMedData::Patient::New();
        m_patientMap[patientID] = result;

        //Patient ID
        result->setPatientId(patientID);

        //Patient Name
        dataset->findAndGetOFString(DCM_PatientName,data);
        result->setName(data.c_str());

        //Patient Birthday
        dataset->findAndGetOFString(DCM_PatientBirthDate,data);
        result->setBirthdate(data.c_str());

        //Patient Sex
        dataset->findAndGetOFString(DCM_PatientSex,data);
        result->setSex(data.c_str());

    }
    else
    {
        result = m_patientMap[patientID];
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Study::sptr DicomSeriesDBReader::createStudy(DcmDataset* dataset)
{
    ::fwMedData::Study::sptr result;
    OFString data;

    // Get Study ID
    dataset->findAndGetOFString(DCM_StudyInstanceUID,data);
    ::std::string studyID = data.c_str();

    // Check if the study already exists
    if(m_studyMap.find(studyID) == m_studyMap.end())
    {
        result              = ::fwMedData::Study::New();
        m_studyMap[studyID] = result;

        //Study ID
        result->setInstanceUID(studyID);

        //Study Date
        dataset->findAndGetOFString(DCM_StudyDate,data);
        result->setDate(data.c_str());

        //Study Time
        dataset->findAndGetOFString(DCM_StudyTime,data);
        result->setTime(data.c_str());

        //Referring Physician Name
        dataset->findAndGetOFString(DCM_ReferringPhysicianName,data);
        result->setReferringPhysicianName(data.c_str());

        //Study Description
        dataset->findAndGetOFString(DCM_StudyDescription,data);
        result->setDescription(data.c_str());

        //Study Patient Age
        dataset->findAndGetOFString(DCM_PatientAge,data);
        result->setPatientAge(data.c_str());

    }
    else
    {
        result = m_studyMap[studyID];
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Equipment::sptr DicomSeriesDBReader::createEquipment(DcmDataset* dataset)
{
    ::fwMedData::Equipment::sptr result;
    OFString data;

    // Get Institution Name
    dataset->findAndGetOFString(DCM_InstitutionName,data);
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

void DicomSeriesDBReader::createSeries(DcmDataset* dataset, const std::string& filename)
{
    ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::sptr();
    OFString data;

    // Get Series Instance UID
    dataset->findAndGetOFString(DCM_SeriesInstanceUID,data);
    std::string seriesInstanceUID = data.c_str();

    // Check if the series already exists
    for(::fwMedData::DicomSeries::sptr dicomSeries :  m_dicomSeriesContainer)
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

        m_dicomSeriesContainer.push_back(series);

        //Instance UID
        series->setInstanceUID(seriesInstanceUID);

        //Modality
        dataset->findAndGetOFString(DCM_Modality,data);
        series->setModality(data.c_str());

        //Date
        dataset->findAndGetOFString(DCM_SeriesDate,data);
        series->setDate(data.c_str());

        //Time
        dataset->findAndGetOFString(DCM_SeriesTime,data);
        series->setTime(data.c_str());

        //Description
        dataset->findAndGetOFString(DCM_SeriesDescription,data);
        series->setDescription(data.c_str());

        //Performing Physicians Name
        std::vector<std::string> performingPhysiciansName;
        for(unsigned long i = 0; dataset->findAndGetOFString(DCM_PerformingPhysicianName,data, i).good(); ++i)
        {
            performingPhysiciansName.push_back(data.c_str());
        }
        series->setPerformingPhysiciansName(performingPhysiciansName);

    }

    // Add the SOPClassUID to the series
    dataset->findAndGetOFString(DCM_SOPClassUID,data);
    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
    sopClassUIDContainer.insert(data.c_str());
    series->setSOPClassUIDs(sopClassUIDContainer);

    // Add the instance to the series
    unsigned int instanceNumber = series->getLocalDicomPaths().size();
    series->addDicomPath(instanceNumber, filename);
}

//------------------------------------------------------------------------------

void DicomSeriesDBReader::convertDicomSeries(SPTR(::fwMedData::DicomSeries)dicomSeries,
                                             bool notify)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);
    ::fwMedData::Series::sptr result = ::fwMedData::Series::sptr();

    ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
    FW_RAISE_IF("The series contains several SOPClassUIDs. Try to apply a filter in order to split the series.",
                sopClassUIDContainer.size() != 1);

    std::string sopClassUID = *sopClassUIDContainer.begin();
    if (!sopClassUID.empty())
    {
        sopClassUID = dcmFindNameOfUID(sopClassUID.c_str());
    }

    // CT Image Storage
    if(sopClassUID == "CTImageStorage" || sopClassUID == "MRImageStorage" ||
       sopClassUID == "SecondaryCaptureImageStorage" || sopClassUID == "XRayAngiographicImageStorage")
    {
        ::fwDcmtkIO::reader::ImageTagReader reader;
        result = reader.read(dicomSeries);
    }
    else if(sopClassUID == "ModalityPerformedProcedureStepSOPClass" )
    {
        ::fwDcmtkIO::reader::MPPSReader reader;
        result = reader.read(dicomSeries);
    }
    else
    {
        // SOP Class uid is not defined, check if it is a MPPS
        ::fwMedData::DicomSeries::DicomPathContainerType instances = dicomSeries->getLocalDicomPaths();
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;

        //Get informations from the first instance
        std::string firstInstance = instances.begin()->second.string();
        status = fileFormat.loadFile(firstInstance.c_str());

        SLM_ASSERT("Cannot load dicom files", status.good());
        if (status.good())
        {
            dataset = fileFormat.getDataset();

            // Decompress data set if compressed
            dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

            // A MPPS dicom must have a 'DCM_PerformedProcedureStepStatus' tag
            OFString mmpsStatus;
            dataset->findAndGetOFString(DCM_PerformedProcedureStepStatus, mmpsStatus);
            if (!mmpsStatus.empty())
            {
                //delete mmpsStatus;
                ::fwDcmtkIO::reader::MPPSReader reader;
                result = reader.read(dicomSeries);
            }
        }
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

    if(notify)
    {
        seriesDBHelper.notify();
    }

}

//------------------------------------------------------------------------------

DicomSeriesDBReader::DicomSeriesContainerType& DicomSeriesDBReader::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

} //namespace fwDcmtkIO



/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include <fwComEd/helper/SeriesDB.hpp>
#include <fwDataIO/reader/registry/macros.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>


#include "fwDicomIOExt/dcmtk/helper/Codec.hpp"
#include "fwDicomIOExt/dcmtk/helper/DicomDir.hpp"
#include "fwDicomIOExt/dcmtk/helper/DicomSearch.hpp"
#include "fwDicomIOExt/dcmtk/helper/Dictionary.hpp"
#include "fwDicomIOExt/dcmtk/reader/ImageStorageReader.hpp"

#include "fwDicomIOExt/dcmtk/SeriesDBReader.hpp"

fwDataIOReaderRegisterMacro( ::fwDicomIOExt::dcmtk::SeriesDBReader );

namespace fwDicomIOExt
{
namespace dcmtk
{

//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this),
     m_isDicomdirActivated(false)
{
    SLM_TRACE_FUNC();

    // Load dictionary
    ::fwDicomIOExt::dcmtk::helper::Dictionary::loadDictionary();

    // Register codecs
    ::fwDicomIOExt::dcmtk::helper::Codec::registerCodecs();
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
    // Clean up codecs
    ::fwDicomIOExt::dcmtk::helper::Codec::cleanup();
}

//------------------------------------------------------------------------------

SeriesDBReader::FilenameContainerType SeriesDBReader::getFilenames()
{
    FilenameContainerType filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        // Try to read dicomdir file
        if(!m_isDicomdirActivated || (m_isDicomdirActivated &&
                !::fwDicomIOExt::dcmtk::helper::DicomDir::readDicomDir(this->getFolder(), filenames)))
        {
            // Recursively search for dicom files
            ::fwDicomIOExt::dcmtk::helper::DicomSearch::searchRecursively(this->getFolder(), filenames);
        }
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        BOOST_FOREACH(::boost::filesystem::path file, this->getFiles())
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

    // Read series
    BOOST_FOREACH(::fwDicomData::DicomSeries::sptr series, m_dicomSeriesContainer)
    {
        this->convertDicomSeries(series);
    }
}

//------------------------------------------------------------------------------

void SeriesDBReader::readFromDicomSeriesDB(::fwMedData::SeriesDB::sptr dicomSeriesDB,
        ::fwServices::IService::sptr notifier)
{
    // Read series
    BOOST_FOREACH(::fwMedData::Series::sptr series, dicomSeriesDB->getContainer())
    {
        ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast(series);
        OSLM_ASSERT("Trying to read a series which is not a DicomSeries.", dicomSeries);
        this->convertDicomSeries(dicomSeries, notifier);
    }
}

//------------------------------------------------------------------------------

void SeriesDBReader::readDicomSeries()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);

    // Get filenames
    FilenameContainerType filenames = this->getFilenames();

    // Read Dicom Series
    this->addSeries(filenames);

    // Push Dicom Series
    BOOST_FOREACH(::fwDicomData::DicomSeries::sptr series, m_dicomSeriesContainer)
    {
        seriesDBHelper.add(series);
    }
}

//------------------------------------------------------------------------------

bool SeriesDBReader::isDicomDirAvailable()
{
    std::vector<std::string> filenames;
    bool result = ::fwDicomIOExt::dcmtk::helper::DicomDir::readDicomDir(this->getFolder(), filenames);
    return result && !filenames.empty();
}

//------------------------------------------------------------------------------

void SeriesDBReader::addSeries(const std::vector< std::string > &filenames)
{
    DcmFileFormat fileFormat;
    BOOST_FOREACH(std::string filename, filenames)
    {
        OFCondition status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());

        DcmDataset* dataset = fileFormat.getDataset();

        // Create Series
        this->createSeries(dataset, filename);

    }

    // Fill series
    BOOST_FOREACH(::fwDicomData::DicomSeries::sptr series, m_dicomSeriesContainer)
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
        ::fwMedData::Patient::sptr patient = this->createPatient(dataset);
        ::fwMedData::Study::sptr study = this->createStudy(dataset);
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
    dataset->findAndGetOFString(DCM_PatientID,data);
    ::std::string patientID = data.c_str();

    // Check if the patient already exists
    if(m_patientMap.find(patientID) == m_patientMap.end())
    {
        result = ::fwMedData::Patient::New();
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

::fwMedData::Study::sptr SeriesDBReader::createStudy(DcmDataset* dataset)
{
    ::fwMedData::Study::sptr result;
    OFString data;

    // Get Study ID
    dataset->findAndGetOFString(DCM_StudyInstanceUID,data);
    ::std::string studyID = data.c_str();

    // Check if the study already exists
    if(m_studyMap.find(studyID) == m_studyMap.end())
    {
        result = ::fwMedData::Study::New();
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

::fwMedData::Equipment::sptr SeriesDBReader::createEquipment(DcmDataset* dataset)
{
    ::fwMedData::Equipment::sptr result;
    OFString data;

    // Get Institution Name
    dataset->findAndGetOFString(DCM_InstitutionName,data);
    ::std::string institutionName = data.c_str();

    // Check if the equipment already exists
    if(m_equipmentMap.find(institutionName) == m_equipmentMap.end())
    {
        result = ::fwMedData::Equipment::New();
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
    ::fwDicomData::DicomSeries::sptr series = ::fwDicomData::DicomSeries::sptr();
    OFString data;

    // Get Series Instance UID
    dataset->findAndGetOFString(DCM_SeriesInstanceUID,data);
    std::string seriesInstanceUID = data.c_str();

    // Check if the series already exists
    BOOST_FOREACH(::fwDicomData::DicomSeries::sptr dicomSeries, m_dicomSeriesContainer)
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
        series = ::fwDicomData::DicomSeries::New();
        series->setDicomAvailability(::fwDicomData::DicomSeries::PATHS);

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
        for(int i=0; dataset->findAndGetOFString(DCM_PerformingPhysicianName,data, i).good(); ++i)
        {
            performingPhysiciansName.push_back(data.c_str());
        }
        series->setPerformingPhysiciansName(performingPhysiciansName);

    }

    // Add the SOPClassUID to the series
    dataset->findAndGetOFString(DCM_SOPClassUID,data);
    ::fwDicomData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
    sopClassUIDContainer.insert(data.c_str());
    series->setSOPClassUIDs(sopClassUIDContainer);

    // Add the instance to the series
    int instanceNumber = series->getLocalDicomPaths().size();
    series->addDicomPath(instanceNumber, filename);
}

//------------------------------------------------------------------------------

void SeriesDBReader::convertDicomSeries(SPTR(::fwDicomData::DicomSeries) dicomSeries,
        ::fwServices::IService::sptr notifier)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);
    ::fwMedData::Series::sptr result = ::fwMedData::Series::sptr();

    ::fwDicomData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
    FW_RAISE_IF("The series contains several SOPClassUIDs. Try to apply a filter in order to split the series.",
            sopClassUIDContainer.size() != 1);
    std::string sopClassUID = dcmFindNameOfUID(sopClassUIDContainer.begin()->c_str());

    // CT Image Storage
    if(sopClassUID == "CTImageStorage" || sopClassUID == "MRImageStorage" ||
            sopClassUID == "SecondaryCaptureImageStorage")
    {
        ::fwDicomIOExt::dcmtk::reader::ImageStorageReader reader;
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
        seriesDBHelper.notify(notifier);
    }

}

//------------------------------------------------------------------------------

SeriesDBReader::DicomSeriesContainerType &SeriesDBReader::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

} //dcmtk
} //namespace fwDicomIOExt



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

#include "fwDicomIOExt/dcmtk/helper/DicomDir.hpp"
#include "fwDicomIOExt/dcmtk/helper/DicomSearch.hpp"
#include "fwDicomIOExt/dcmtk/reader/CTImageStorageReader.hpp"
#include "fwDicomIOExt/dcmtk/SeriesDBReader.hpp"

fwDataIOReaderRegisterMacro( ::fwDicomIOExt::dcmtk::SeriesDBReader );

namespace fwDicomIOExt
{
namespace dcmtk
{

//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SeriesDBReader::read()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    std::vector<std::string> filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        // Try to read dicomdir file
        if(!::fwDicomIOExt::dcmtk::helper::DicomDir::readDicomDir(this->getFolder(), filenames))
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
    this->addSeries( seriesDB , filenames);
}

//------------------------------------------------------------------------------

void SeriesDBReader::addSeries( const ::fwMedData::SeriesDB::sptr &seriesDB, const std::vector< std::string > &filenames)
{
    ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);

    DcmFileFormat fileFormat;
    for(std::vector< std::string >::const_iterator it = filenames.begin(); it != filenames.end(); ++it)
    {
        OFCondition status = fileFormat.loadFile(it->c_str());
        FW_RAISE_IF("Unable to read the file: \""+*it+"\"", status.bad());

        DcmDataset* dataset = fileFormat.getDataset();

        // Create data objects
        ::fwMedData::Patient::sptr patient = this->createPatient(dataset);
        ::fwMedData::Study::sptr study = this->createStudy(dataset);
        ::fwMedData::Equipment::sptr equipment = this->createEquipment(dataset);
        ::fwMedData::Series::sptr series = this->createSeries(dataset, *it);

        // Fill series
        series->setPatient(patient);
        series->setStudy(study);
        series->setEquipment(equipment);




    }

    // Create every series and push them in the database.
    for(ObjectReaderMapType::iterator it = m_objectReaderMap.begin(); it != m_objectReaderMap.end(); ++it)
    {
        ::fwMedData::Series::sptr s = it->second->create();
        seriesDBHelper.add(s);
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

    // Get Patient ID
    dataset->findAndGetOFString(DCM_StudyID,data);
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

::fwMedData::Series::sptr SeriesDBReader::createSeries(DcmDataset* dataset, const std::string& filename)
{
    ::fwMedData::Series::sptr result;
    OFString data;

    // Get Series Instance UID
    dataset->findAndGetOFString(DCM_SeriesInstanceUID,data);
    ::std::string seriesInstanceUID = data.c_str();

    // Check if the series already exists
    if(m_seriesMap.find(seriesInstanceUID) == m_seriesMap.end())
    {
        //Get SOP Class UID
        dataset->findAndGetOFString(DCM_SOPClassUID,data);
        ::std::string sopClassUID = data.c_str();
        if(sopClassUID != "SurfaceSegmentationStorage")
        {
            result = ::fwMedData::ImageSeries::New();
        }
        else
        {
            result = ::fwMedData::ModelSeries::New();
        }

        m_seriesMap[seriesInstanceUID] = result;
        this->createObjectReader(sopClassUID,result);

        //Instance UID
        result->setInstanceUID(seriesInstanceUID);

        //Modality
        dataset->findAndGetOFString(DCM_Modality,data);
        result->setModality(data.c_str());

        //Date
        dataset->findAndGetOFString(DCM_SeriesDate,data);
        result->setDate(data.c_str());

        //Time
        dataset->findAndGetOFString(DCM_SeriesTime,data);
        result->setTime(data.c_str());

        //Description
        dataset->findAndGetOFString(DCM_SeriesDescription,data);
        result->setDescription(data.c_str());

        //Performing Physicians Name
        std::vector<std::string> performingPhysiciansName;
        for(int i=0; dataset->findAndGetOFString(DCM_PerformingPhysicianName,data, i).good(); ++i)
        {
            performingPhysiciansName.push_back(data.c_str());
        }
        result->setPerformingPhysiciansName(performingPhysiciansName);


    }
    else
    {
        result = m_seriesMap[seriesInstanceUID];
    }

    //Add the dataset to the reader
    if(m_objectReaderMap.find(result) != m_objectReaderMap.end())
    {
        m_objectReaderMap[result]->addInstance(dataset, filename);
    }


    return result;
}

//------------------------------------------------------------------------------

void SeriesDBReader::createObjectReader(const std::string& readerType, ::fwMedData::Series::sptr series)
{
    // CT Image Storage
    if(readerType == "1.2.840.10008.5.1.4.1.1.2")
    {
        m_objectReaderMap[series] = ::fwDicomIOExt::dcmtk::reader::CTImageStorageReader::New(series);
    }
    else
    {
        SLM_WARN("There is no reader for the SOPClassUID \""+readerType+"\". Unable to read the instance.");
    }
}

} //dcmtk
} //namespace fwDicomIOExt



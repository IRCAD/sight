/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <gdcmAttribute.h>
#include <gdcmDirectory.h>
#include <gdcmMediaStorage.h>
#include <gdcmUIDs.h>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwComEd/helper/SeriesDB.hpp>

#include <fwDataIO/reader/registry/macros.hpp>
#include <fwDicomIOExt/dcmtk/helper/DicomDir.hpp>
#include <fwDicomIOExt/dcmtk/helper/DicomSearch.hpp>
#include <fwDicomIOExt/dcmtk/helper/Dictionary.hpp>
#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/IFilter.hpp>

#include "gdcmIO/reader/SeriesDB.hpp"
#include "gdcmIO/reader/Series.hpp"
#include "gdcmIO/helper/DicomData.hpp"

fwDataIOReaderRegisterMacro( ::gdcmIO::reader::SeriesDB );

namespace gdcmIO
{

namespace reader
{
//Series
static const gdcm::Tag s_SeriesInstanceUIDTag(0x0020, 0x000e);
static const gdcm::Tag s_SeriesDateTag(0x0008, 0x0021);
static const gdcm::Tag s_SeriesTimeTag(0x0008, 0x0031);
static const gdcm::Tag s_ModalityTag(0x0008, 0x0060);
static const gdcm::Tag s_SeriesDescriptionTag(0x0008, 0x103e);
static const gdcm::Tag s_PerformingPhysicianNameTag(0x0008, 0x1050);
static const gdcm::Tag s_SOPClassUIDTag(0x0008, 0x0016);
//Equipment
static const gdcm::Tag s_InstitutionNameTag(0x0008, 0x0080);
//Patient
static const gdcm::Tag s_PatientNameTag(0x0010, 0x0010);
static const gdcm::Tag s_PatientIDTag(0x0010, 0x0020);
static const gdcm::Tag s_PatientBirthDateTag(0x0010, 0x0030);
static const gdcm::Tag s_PatientSexTag(0x0010, 0x0040);
//Study
static const gdcm::Tag s_StudyInstanceUIDTag(0x0020, 0x000d);
static const gdcm::Tag s_StudyDateTag(0x0008, 0x0020);
static const gdcm::Tag s_StudyTimeTag(0x0008, 0x0030);
static const gdcm::Tag s_ReferringPhysicianNameTag(0x0008, 0x0090);
static const gdcm::Tag s_StudyDescriptionTag(0x0008, 0x1030);
static const gdcm::Tag s_PatientAgeTag(0x0010, 0x1010);

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this),
    m_isDicomdirActivated(false),
    m_dicomFilterType("")
{
    SLM_TRACE_FUNC();

    // Load DCMTK dictionary for filters
    ::fwDicomIOExt::dcmtk::helper::Dictionary::loadDictionary();
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDB::FilenameContainerType SeriesDB::getFilenames()
{
    FilenameContainerType filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        // Try to read dicomdir file
        if(!m_isDicomdirActivated || (m_isDicomdirActivated &&
                                      !::fwDicomIOExt::dcmtk::helper::DicomDir::readDicomDir(this->getFolder(),
                                                                                             filenames)))
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

void SeriesDB::read()
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
    this->convertDicomSeries();
}

//------------------------------------------------------------------------------

void SeriesDB::readFromDicomSeriesDB(::fwMedData::SeriesDB::sptr dicomSeriesDB, ::fwServices::IService::sptr notifier)
{
    // Clear DicomSeries container
    m_dicomSeriesContainer.clear();

    // Read series
    BOOST_FOREACH(::fwMedData::Series::sptr series, dicomSeriesDB->getContainer())
    {
        ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast(series);
        SLM_ASSERT("Trying to read a series which is not a DicomSeries.", dicomSeries);
        m_dicomSeriesContainer.push_back(dicomSeries);
    }

    this->convertDicomSeries(notifier);
}

//------------------------------------------------------------------------------

void SeriesDB::readDicomSeries()
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

bool SeriesDB::isDicomDirAvailable()
{
    std::vector<std::string> filenames;
    bool result = ::fwDicomIOExt::dcmtk::helper::DicomDir::readDicomDir(this->getFolder(), filenames);
    return result && !filenames.empty();
}

//------------------------------------------------------------------------------

void SeriesDB::addSeries(const std::vector< std::string > &filenames)
{
    ::gdcm::Scanner seriesScanner;
    seriesScanner.AddTag(s_SeriesInstanceUIDTag);
    seriesScanner.AddTag(s_ModalityTag);
    seriesScanner.AddTag(s_SeriesDateTag);
    seriesScanner.AddTag(s_SeriesTimeTag);
    seriesScanner.AddTag(s_SeriesDescriptionTag);
    seriesScanner.AddTag(s_PerformingPhysicianNameTag);
    seriesScanner.AddTag(s_SOPClassUIDTag);

    bool status = seriesScanner.Scan( filenames );
    FW_RAISE_IF("Unable to read the files.", !status);

    ::gdcm::Directory::FilenamesType keys = seriesScanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator it;

    //Loop through every files available in the scanner
    for(it = keys.begin(); it != keys.end(); ++it)
    {
        const std::string filename = it->c_str();
        OSLM_ASSERT("The file \"" << filename << "\" is not a key of the gdcm scanner",
                    seriesScanner.IsKey(filename.c_str()));

        // Create Series
        this->createSeries(seriesScanner, filename);
    }

    ::gdcm::Scanner attributeScanner;
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

    // Fill series
    BOOST_FOREACH(::fwDicomData::DicomSeries::sptr series, m_dicomSeriesContainer)
    {
        // Compute number of instances
        series->setNumberOfInstances(series->getLocalDicomPaths().size());

        // Get first instance filename
        const std::string filename = series->getLocalDicomPaths().begin()->second.string();

        // Load first instance
        std::vector< std::string > firstInstanceContainer;
        firstInstanceContainer.push_back(filename);
        status = attributeScanner.Scan( firstInstanceContainer );
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
    }

}

//------------------------------------------------------------------------------

::fwMedData::Patient::sptr SeriesDB::createPatient(const ::gdcm::Scanner& scanner, const std::string& filename)
{
    ::fwMedData::Patient::sptr result;

    // Get Patient ID
    std::string patientID = SeriesDB::getStringValue( scanner, filename, s_PatientIDTag );

    // Check if the patient already exists
    if(m_patientMap.find(patientID) == m_patientMap.end())
    {
        result                  = ::fwMedData::Patient::New();
        m_patientMap[patientID] = result;

        //Patient ID
        result->setPatientId(patientID);

        //Patient Name
        std::string patientName = SeriesDB::getStringValue( scanner, filename, s_PatientNameTag );
        result->setName(patientName);

        //Patient Birthdate
        std::string patientBirthDate = SeriesDB::getStringValue( scanner, filename, s_PatientBirthDateTag );
        result->setBirthdate(patientBirthDate);

        //Patient Sex
        std::string patientSex = SeriesDB::getStringValue( scanner, filename, s_PatientSexTag );
        result->setSex(patientSex);

    }
    else
    {
        result = m_patientMap[patientID];
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Study::sptr SeriesDB::createStudy(const ::gdcm::Scanner& scanner, const std::string& filename)
{
    ::fwMedData::Study::sptr result;

    // Get Study ID
    std::string studyInstanceUID = SeriesDB::getStringValue( scanner, filename, s_StudyInstanceUIDTag );

    // Check if the study already exists
    if(m_studyMap.find(studyInstanceUID) == m_studyMap.end())
    {
        result                       = ::fwMedData::Study::New();
        m_studyMap[studyInstanceUID] = result;

        //Study ID
        result->setInstanceUID(studyInstanceUID);

        //Study Date
        std::string studyDate = SeriesDB::getStringValue( scanner, filename, s_StudyDateTag );
        result->setDate(studyDate);

        //Study Time
        std::string studyTime = SeriesDB::getStringValue( scanner, filename, s_StudyTimeTag );
        result->setTime(studyTime);

        //Referring Physician Name
        std::string referringPhysicianName = SeriesDB::getStringValue( scanner, filename, s_ReferringPhysicianNameTag );
        result->setReferringPhysicianName(referringPhysicianName);

        //Study Description
        std::string studyDescription = SeriesDB::getStringValue( scanner, filename, s_StudyDescriptionTag );
        result->setDescription(studyDescription);

        //Study Patient Age
        std::string patientAge = SeriesDB::getStringValue( scanner, filename, s_PatientAgeTag );
        result->setPatientAge(patientAge);

    }
    else
    {
        result = m_studyMap[studyInstanceUID];
    }

    return result;
}

//------------------------------------------------------------------------------

::fwMedData::Equipment::sptr SeriesDB::createEquipment(const ::gdcm::Scanner& scanner, const std::string& filename)
{
    ::fwMedData::Equipment::sptr result;

    // Get Institution Name
    std::string institutionName = SeriesDB::getStringValue( scanner, filename, s_InstitutionNameTag );

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

void SeriesDB::createSeries(const ::gdcm::Scanner& scanner, const std::string& filename)
{
    ::fwDicomData::DicomSeries::sptr series = ::fwDicomData::DicomSeries::sptr();

    // Get Series Instance UID
    std::string seriesInstanceUID = SeriesDB::getStringValue( scanner, filename, s_SeriesInstanceUIDTag );

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
        std::string modality = SeriesDB::getStringValue( scanner, filename, s_ModalityTag );
        series->setModality(modality);

        //Date
        std::string seriesDate = SeriesDB::getStringValue( scanner, filename, s_SeriesDateTag );
        series->setDate(seriesDate);

        //Time
        std::string seriesTime = SeriesDB::getStringValue( scanner, filename, s_SeriesTimeTag );
        series->setTime(seriesTime);

        //Description
        std::string seriesDescription = SeriesDB::getStringValue( scanner, filename, s_SeriesDescriptionTag );
        series->setDescription(seriesDescription);

        //Performing Physicians Name
        std::string performingPhysicianNamesStr = SeriesDB::getStringValue( scanner, filename,
                                                                            s_PerformingPhysicianNameTag );
        ::fwMedData::DicomValuesType performingPhysicianNames;
        ::boost::split( performingPhysicianNames, performingPhysicianNamesStr, ::boost::is_any_of("\\"));
        series->setPerformingPhysiciansName(performingPhysicianNames);

    }

    // Add the SOPClassUID to the series
    std::string sopClassUID = SeriesDB::getStringValue( scanner, filename, s_SOPClassUIDTag );
    ::fwDicomData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = series->getSOPClassUIDs();
    sopClassUIDContainer.insert(sopClassUID);
    series->setSOPClassUIDs(sopClassUIDContainer);

    // Add the instance to the series
    int instanceNumber = series->getLocalDicomPaths().size();
    series->addDicomPath(instanceNumber, filename);
}

//------------------------------------------------------------------------------

void SeriesDB::convertDicomSeries(::fwServices::IService::sptr notifier)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    // Sort DicomSeries
    std::sort(m_dicomSeriesContainer.begin(), m_dicomSeriesContainer.end(), SeriesDB::dicomSeriesComparator);

    // Create reader
    ::gdcmIO::reader::Series seriesReader;

    // Read series
    BOOST_FOREACH(::fwDicomData::DicomSeries::sptr dicomSeries, m_dicomSeriesContainer)
    {
        try
        {
            // Read series
            ::fwMedData::Series::sptr series = seriesReader.read(dicomSeries);

            if(series)
            {
                // Add the series to the DB
                ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);
                seriesDBHelper.add(series);

                if(notifier)
                {
                    seriesDBHelper.notify(notifier);
                }
            }
        }
        catch (::gdcmIO::exception::Failed & e)
        {
            OSLM_ERROR("Unable to read series : " << dicomSeries->getInstanceUID());
        }

    }

}

//------------------------------------------------------------------------------

bool SeriesDB::dicomSeriesComparator(SPTR(::fwDicomData::DicomSeries)a, SPTR(::fwDicomData::DicomSeries)b)
{
    ::fwDicomData::DicomSeries::SOPClassUIDContainerType aSOPClassUIDContainer = a->getSOPClassUIDs();
    std::string aSOPClassUID = *(aSOPClassUIDContainer.begin());
    ::fwDicomData::DicomSeries::SOPClassUIDContainerType bSOPClassUIDContainer = b->getSOPClassUIDs();
    std::string bSOPClassUID = *(bSOPClassUIDContainer.begin());
    // a > b if a contains a SR and not b
    return !((::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR ||
              ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR ||
              aSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" ||       // FIXME Replace hard coded string by "::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::Comprehensive3DSR"
              ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage ||
              ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SurfaceSegmentationStorage)
             && !(::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR ||
                  ::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR ||
                  bSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" ||       // FIXME Replace hard coded string by "::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::Comprehensive3DSR"
                  ::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) ==
                  ::gdcm::MediaStorage::SpacialFiducialsStorage ||
                  ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) ==
                  ::gdcm::MediaStorage::SurfaceSegmentationStorage));
}

//------------------------------------------------------------------------------

SeriesDB::DicomSeriesContainerType &SeriesDB::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

//------------------------------------------------------------------------------

std::string SeriesDB::getStringValue(const ::gdcm::Scanner& scanner, const std::string& filename, const gdcm::Tag& tag)
{
    const char* value  = scanner.GetValue( filename.c_str(), tag );
    std::string result = (value) ? value : "";
    ::boost::algorithm::trim(result);
    return result;
}

}  // namespace reader

}  // namespace gdcmIO


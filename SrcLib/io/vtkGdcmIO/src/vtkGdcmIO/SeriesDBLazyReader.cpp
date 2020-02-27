/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#if (SPYLOG_LEVEL >= 4)
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
#include <filesystem>
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

fwDataIOReaderRegisterMacro(::vtkGdcmIO::SeriesDBLazyReader);

namespace vtkGdcmIO
{

//------------------------------------------------------------------------------

static const ::gdcm::Tag s_SERIES_MODALITY_TAG(0x0008, 0x0060);
static const ::gdcm::Tag s_SERIES_UID_TAG(0x0020, 0x000e);
static const ::gdcm::Tag s_SERIES_NUMBER_TAG(0x0020, 0x0011);
static const ::gdcm::Tag s_SERIES_LATERALITY_TAG(0x0020, 0x0060);
static const ::gdcm::Tag s_SERIES_DATE_TAG(0x0008, 0x0021);
static const ::gdcm::Tag s_SERIES_TIME_TAG(0x0008, 0x0031);
static const ::gdcm::Tag s_SERIES_PHYSICIAN_NAMES_TAG(0x0008, 0x1050);
static const ::gdcm::Tag s_SERIES_PROTOCOL_NAME_TAG(0x0018, 0x1030);
static const ::gdcm::Tag s_SERIES_DESCRIPTION_TAG(0x0008, 0x103e);
static const ::gdcm::Tag s_SERIES_BODY_PART_EXAMINED_TAG(0x0018, 0x0015);
static const ::gdcm::Tag s_SERIES_PATIENT_POSITION_TAG(0x0018, 0x5100);
static const ::gdcm::Tag s_SERIES_ANATOMICAL_ORIENTATION_TYPE_TAG(0x0010, 0x2210);
static const ::gdcm::Tag s_SERIES_PERFORMDED_PROCEDURE_STEP_ID_TAG(0x0040, 0x0253);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_START_DATE_TAG(0x0040, 0x0244);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_START_TIME_TAG(0x0040, 0x0245);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_END_DATE_TAG(0x0040, 0x0250);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_END_TIME_TAG(0x0040, 0x0251);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_DESCRIPTION_TAG(0x0040, 0x0254);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_COMMENTS_TAG(0x0040, 0x0280);

static const ::gdcm::Tag s_PATIENT_NAME_TAG(0x0010, 0x0010);
static const ::gdcm::Tag s_PATIENT_ID_TAG(0x0010, 0x0020);
static const ::gdcm::Tag s_PATIENT_BIRTHDATE_TAG(0x0010, 0x0030);
static const ::gdcm::Tag s_PATIENT_SEX_TAG(0x0010, 0x0040);

static const ::gdcm::Tag s_STUDY_UID_TAG(0x0020, 0x000d);
static const ::gdcm::Tag s_STUDY_ID_TAG(0x0020, 0x0010);
static const ::gdcm::Tag s_STUDY_DATE_TAG(0x0008, 0x0020);
static const ::gdcm::Tag s_STUDY_TIME_TAG(0x0008, 0x0030);
static const ::gdcm::Tag s_STUDY_REFERRING_PHYSICIAN_NAME_TAG(0x0008, 0x0090);
static const ::gdcm::Tag s_STUDY_CONSULTING_PHYSICIAN_NAME_TAG(0x0008, 0x009C);
static const ::gdcm::Tag s_STUDY_DESCRIPTION_TAG(0x0008, 0x1030);
static const ::gdcm::Tag s_STUDY_PATIENT_AGE_TAG(0x0010, 0x1010);
static const ::gdcm::Tag s_STUDY_PATIENT_SIZE_TAG(0x0010, 0x1020);
static const ::gdcm::Tag s_STUDY_PATIENT_WEIGHT_TAG(0x0010, 0x1030);
static const ::gdcm::Tag s_STUDY_PATIENT_BODY_MASS_INDEX_TAG(0x0010, 0x1022);

static const ::gdcm::Tag s_EQUIPMENT_INSTITUTION_NAME_TAG(0x0008, 0x0080);

static const ::gdcm::Tag s_IMAGE_CONTRAST_AGENT_TAG(0x0018, 0x0010);
static const ::gdcm::Tag s_IMAGE_CONTRAST_ROUTE_TAG(0x0018, 0x1040);
static const ::gdcm::Tag s_IMAGE_CONTRAST_VOLUME_TAG(0x0018, 0x1041);
static const ::gdcm::Tag s_IMAGE_CONTRAST_START_TIME_TAG(0x0018, 0x1042);
static const ::gdcm::Tag s_IMAGE_CONTRAST_STOP_TIME_TAG(0x0018, 0x1043);
static const ::gdcm::Tag s_IMAGE_CONTRAST_TOTAL_DOSE_TAG(0x0018, 0x1044);
static const ::gdcm::Tag s_IMAGE_CONTRAST_FLOW_RATE_TAG(0x0018, 0x1046);
static const ::gdcm::Tag s_IMAGE_CONTRAST_FLOW_DURATION_TAG(0x0018, 0x1047);
static const ::gdcm::Tag s_IMAGE_CONTRAST_INGREDIENT_TAG(0x0018, 0x1048);
static const ::gdcm::Tag s_IMAGE_CONTRAST_INGREDIENT_CONCENTRATION_TAG(0x0018, 0x1049);
static const ::gdcm::Tag s_IMAGE_TYPE_TAG(0x0008, 0x0008);
static const ::gdcm::Tag s_IMAGE_ACQUISITION_DATE_TAG(0x0008, 0x0022);
static const ::gdcm::Tag s_IMAGE_ACQUISITION_TIME_TAG(0x0008, 0x0032);

static const ::gdcm::Tag s_SLICE_THICKNESS_TAG(0x0018, 0x0050);
static const ::gdcm::Tag s_WINDOW_CENTER_TAG(0x0028, 0x1050);
static const ::gdcm::Tag s_WINDOW_WIDTH_TAG(0x0028, 0x1051);

//------------------------------------------------------------------------------

template < typename T >
T getNumericValue(::gdcm::Scanner& _scanner, const std::string& _dcmFile, const ::gdcm::Tag& _tag)
{
    const char* tagValue = _scanner.GetValue(_dcmFile.c_str(), _tag);
    if(tagValue)
    {
        std::string val(tagValue);
        ::boost::algorithm::trim(val);
        return ::boost::lexical_cast<T>(val);
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------

SeriesDBLazyReader::SeriesFilesType sortImageSeriesFiles(const SeriesDBLazyReader::SeriesFilesType& _seriesFiles)
{
    SeriesDBLazyReader::SeriesFilesType sortedSeriesFiles = _seriesFiles;

    ::gdcm::IPPSorter s;
    s.SetComputeZSpacing(true);
    s.SetZSpacingTolerance(1e-3);
    bool success = s.Sort(_seriesFiles);

    SLM_WARN_IF("Failed to sort : " + _seriesFiles[0], !success);
    if(success)
    {
        sortedSeriesFiles = s.GetFilenames();
    }

    return sortedSeriesFiles;
}

//------------------------------------------------------------------------------

SeriesDBLazyReader::MapSeriesType buildMapSeriesFromScanner(::gdcm::Scanner& _scanner)
{
    SeriesDBLazyReader::MapSeriesType mapSeries;

    ::gdcm::Directory::FilenamesType keys = _scanner.GetKeys();
    for(const std::string& filename : keys)
    {
        SLM_ASSERT("'"+filename+"' is not a key in the mapping table", _scanner.IsKey(filename.c_str()));

        const std::string seriesUID = _scanner.GetValue(filename.c_str(), s_SERIES_UID_TAG);
        if(!seriesUID.empty())
        {
            std::string fileSetId = seriesUID;

            const std::string imageAcquisitionDate = _scanner.GetValue(filename.c_str(), s_IMAGE_ACQUISITION_DATE_TAG);
            if(!imageAcquisitionDate.empty())
            {
                fileSetId += "_";
                fileSetId += imageAcquisitionDate;
            }

            const std::string imageType = _scanner.GetValue(filename.c_str(), s_IMAGE_TYPE_TAG);
            if(!imageType.empty())
            {
                // Treatment of secondary capture dicom file.
                SLM_TRACE("Image Type : " + std::string(imageType));
                fileSetId += "_";
                fileSetId += imageType;
            }
            mapSeries[fileSetId].push_back(filename);
        }
        else
        {
            SLM_ERROR("Unique identifier is require for all series and was not found in the file: " + filename);
        }

    }

    return mapSeries;
}

//------------------------------------------------------------------------------

SeriesDBLazyReader::SeriesDBLazyReader(::fwDataIO::reader::IObjectReader::Key) :
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

void SeriesDBLazyReader::read()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    std::vector<std::string> filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        helper::GdcmHelper::searchRecursivelyFiles(this->getFolder(), filenames);
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        for(std::filesystem::path file :  this->getFiles())
        {
            filenames.push_back(file.string());
        }
    }
    this->addSeries(seriesDB, filenames);
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr SeriesDBLazyReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SeriesDBLazyReader::createSeriesDB(const std::filesystem::path& _dicomDir)
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    std::vector<std::string> filenames;
    helper::GdcmHelper::searchRecursivelyFiles(_dicomDir, filenames);

    this->addSeries(seriesDB, filenames);
    return seriesDB;
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::addSeries(const ::fwMedData::SeriesDB::sptr& _seriesDB,
                                   const std::vector< std::string >& _filenames)
{
    //gdcm::Trace::SetDebug(1);
    //gdcm::Trace::SetWarning(1);
    //gdcm::Trace::SetError(1);

    try
    {

#if (SPYLOG_LEVEL >= 4) // Log level info
        ::fwCore::HiResTimer timer;
        timer.start();
#endif

        /// Build scanner
        ::gdcm::Scanner scanner;

        /// Scan specific tags in filenames
        this->scanFiles(scanner, _filenames);

        /// Build map series
        MapSeriesType mapSeries = buildMapSeriesFromScanner(scanner);
        for(MapSeriesType::value_type mapElem :  mapSeries)
        {
            SeriesFilesType seriesFiles   = sortImageSeriesFiles(mapElem.second);
            const std::string& refDcmFile = seriesFiles[0];

            // ToDo
            // ::gdcm::MediaStorage::IsImage(::gdcm::MediaStorage::GetMSType(XXX)));
            bool isAnImage = true;
            if (isAnImage)
            {
                /// Build new data
                ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::New();
                ::fwData::Image::sptr img             = ::fwData::Image::New();

                series->setImage(img);

                // Fill data object
                this->fillSeries(scanner, refDcmFile, series);
                this->fillPatient(scanner, refDcmFile, series->getPatient());
                this->fillStudy(scanner, refDcmFile, series->getStudy());
                this->fillEquipment(scanner, refDcmFile, series->getEquipment());
                this->fillImage(scanner, seriesFiles, refDcmFile, img);

                _seriesDB->getContainer().push_back(series);
            }
        }

#if (SPYLOG_LEVEL >= 4)
        timer.stop();
        OSLM_INFO("Time in to lazy read data : " << timer.getElapsedTimeInMilliSec());
#endif

    }
    catch (std::exception& e)
    {
        OSLM_ERROR("Try with another reader or retry with this reader on a specific subfolder : " << e.what());
        std::vector< std::string >::const_iterator it = _filenames.begin();
        for(; it != _filenames.end(); ++it)
        {
            SLM_ERROR("file error : " + *it);
        }
    }
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::scanFiles(::gdcm::Scanner& _scanner, const std::vector< std::string >& _filenames)
{
    _scanner.AddTag(s_SERIES_MODALITY_TAG);
    _scanner.AddTag(s_SERIES_UID_TAG);
    _scanner.AddTag(s_SERIES_NUMBER_TAG);
    _scanner.AddTag(s_SERIES_LATERALITY_TAG);
    _scanner.AddTag(s_SERIES_DATE_TAG);
    _scanner.AddTag(s_SERIES_TIME_TAG);
    _scanner.AddTag(s_SERIES_PHYSICIAN_NAMES_TAG);
    _scanner.AddTag(s_SERIES_PROTOCOL_NAME_TAG);
    _scanner.AddTag(s_SERIES_DESCRIPTION_TAG);
    _scanner.AddTag(s_SERIES_BODY_PART_EXAMINED_TAG);
    _scanner.AddTag(s_SERIES_PATIENT_POSITION_TAG);
    _scanner.AddTag(s_SERIES_ANATOMICAL_ORIENTATION_TYPE_TAG);
    _scanner.AddTag(s_SERIES_PERFORMDED_PROCEDURE_STEP_ID_TAG);
    _scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_START_DATE_TAG);
    _scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_START_TIME_TAG);
    _scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_END_DATE_TAG);
    _scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_END_TIME_TAG);
    _scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_DESCRIPTION_TAG);
    _scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_COMMENTS_TAG);

    _scanner.AddTag(s_PATIENT_NAME_TAG);
    _scanner.AddTag(s_PATIENT_ID_TAG);
    _scanner.AddTag(s_PATIENT_BIRTHDATE_TAG);
    _scanner.AddTag(s_PATIENT_SEX_TAG);

    _scanner.AddTag(s_STUDY_UID_TAG);
    _scanner.AddTag(s_STUDY_ID_TAG);
    _scanner.AddTag(s_STUDY_DATE_TAG);
    _scanner.AddTag(s_STUDY_TIME_TAG);
    _scanner.AddTag(s_STUDY_REFERRING_PHYSICIAN_NAME_TAG);
    _scanner.AddTag(s_STUDY_CONSULTING_PHYSICIAN_NAME_TAG);
    _scanner.AddTag(s_STUDY_DESCRIPTION_TAG);
    _scanner.AddTag(s_STUDY_PATIENT_AGE_TAG);
    _scanner.AddTag(s_STUDY_PATIENT_SIZE_TAG);
    _scanner.AddTag(s_STUDY_PATIENT_WEIGHT_TAG);
    _scanner.AddTag(s_STUDY_PATIENT_BODY_MASS_INDEX_TAG);

    _scanner.AddTag(s_EQUIPMENT_INSTITUTION_NAME_TAG);

    _scanner.AddTag(s_IMAGE_CONTRAST_AGENT_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_ROUTE_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_VOLUME_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_START_TIME_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_STOP_TIME_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_TOTAL_DOSE_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_FLOW_RATE_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_FLOW_DURATION_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_INGREDIENT_TAG);
    _scanner.AddTag(s_IMAGE_CONTRAST_INGREDIENT_CONCENTRATION_TAG);
    _scanner.AddTag(s_IMAGE_TYPE_TAG);
    _scanner.AddTag(s_IMAGE_ACQUISITION_DATE_TAG);
    _scanner.AddTag(s_IMAGE_ACQUISITION_TIME_TAG);

    bool scanIsOk = _scanner.Scan(_filenames);
    FW_RAISE_IF("Dicom scanner failed", !scanIsOk);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillSeries(::gdcm::Scanner& _scanner, const std::string& _dcmFile,
                                    ::fwMedData::ImageSeries::sptr _imgSeries)
{
    const std::string seriesModality = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                    s_SERIES_MODALITY_TAG);
    const std::string seriesUID =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_UID_TAG);
    const std::string seriesNumber
        = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_NUMBER_TAG);
    const std::string seriesLaterality
        = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_LATERALITY_TAG);
    const std::string seriesDate
        = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_DATE_TAG);
    const std::string seriesTime
        = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_TIME_TAG);
    const ::gdcm::Scanner::ValuesType physicianNames = _scanner.GetValues(s_SERIES_PHYSICIAN_NAMES_TAG);
    ::fwMedData::DicomValuesType seriesPhysicianNames;
    for(const std::string& name :  physicianNames)
    {
        ::fwMedData::DicomValuesType result;
        ::boost::split(result, name, ::boost::is_any_of("\\"));
        seriesPhysicianNames.reserve(seriesPhysicianNames.size() + result.size());
        seriesPhysicianNames.insert(seriesPhysicianNames.end(), result.begin(), result.end());
    }
    const std::string seriesProtocolName =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PROTOCOL_NAME_TAG);
    const std::string seriesDescription =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_DESCRIPTION_TAG);
    const std::string seriesBodypartExamined =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PROTOCOL_NAME_TAG);
    const std::string seriesPatientPosition =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PATIENT_POSITION_TAG);
    const std::string seriesAnatomicalOrientationType =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_ANATOMICAL_ORIENTATION_TYPE_TAG);
    const std::string seriesPerformedProcedureStepID =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PERFORMDED_PROCEDURE_STEP_ID_TAG);
    const std::string seriesPerformedProcedureStepStartDate =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PERFORMED_PROCEDURE_STEP_START_DATE_TAG);
    const std::string seriesPerformedProcedureStepStartTime =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PERFORMED_PROCEDURE_STEP_START_TIME_TAG);
    const std::string seriesPerformedProcedureStepEndDate =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PERFORMED_PROCEDURE_STEP_END_DATE_TAG);
    const std::string seriesPerformedProcedureStepEndTime =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PERFORMED_PROCEDURE_STEP_END_TIME_TAG);
    const std::string seriesPerformedProcedureStepDescription =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PERFORMED_PROCEDURE_STEP_DESCRIPTION_TAG);
    const std::string seriesPerformedProcedureComments =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_SERIES_PERFORMED_PROCEDURE_COMMENTS_TAG);

    const std::string imageContrastAgent =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_AGENT_TAG);
    const std::string imageContrastRoute =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_ROUTE_TAG);
    const std::string imageContrastVolume =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_VOLUME_TAG);
    const std::string imageContrastStartTime =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_START_TIME_TAG);
    const std::string imageContrastStopTime =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_STOP_TIME_TAG);
    const std::string imageContrastTotalDose =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_TOTAL_DOSE_TAG);
    const std::string imageContrastFlowRate =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_FLOW_RATE_TAG);
    const std::string imageContrastFlowDuration =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_FLOW_DURATION_TAG);
    const std::string imageContrastIngredient =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_INGREDIENT_TAG);
    const std::string imageContrastIngredientConcentration =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_CONTRAST_INGREDIENT_CONCENTRATION_TAG);
    const std::string imageAcquisitionDate =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_ACQUISITION_DATE_TAG);
    const std::string imageAcquisitionTime =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_IMAGE_ACQUISITION_TIME_TAG);

    SLM_ASSERT("No series modality", !seriesModality.empty());
    _imgSeries->setModality(seriesModality);
    SLM_ASSERT("No series UID", !seriesUID.empty());
    _imgSeries->setInstanceUID(seriesUID);
    _imgSeries->setNumber(seriesNumber);
    _imgSeries->setLaterality(seriesLaterality);
    _imgSeries->setDate(seriesDate);
    _imgSeries->setTime(seriesTime);
    _imgSeries->setPerformingPhysiciansName(seriesPhysicianNames);
    _imgSeries->setProtocolName(seriesProtocolName);
    _imgSeries->setDescription(seriesDescription);
    _imgSeries->setBodyPartExamined(seriesBodypartExamined);
    _imgSeries->setPatientPosition(seriesPatientPosition);
    _imgSeries->setAnatomicalOrientationType(seriesAnatomicalOrientationType);
    _imgSeries->setPerformedProcedureStepID(seriesPerformedProcedureStepID);
    _imgSeries->setPerformedProcedureStepStartDate(seriesPerformedProcedureStepStartDate);
    _imgSeries->setPerformedProcedureStepStartTime(seriesPerformedProcedureStepStartTime);
    _imgSeries->setPerformedProcedureStepEndDate(seriesPerformedProcedureStepEndDate);
    _imgSeries->setPerformedProcedureStepEndTime(seriesPerformedProcedureStepEndTime);
    _imgSeries->setPerformedProcedureStepDescription(seriesPerformedProcedureStepDescription);
    _imgSeries->setPerformedProcedureComments(seriesPerformedProcedureComments);

    _imgSeries->setContrastAgent(imageContrastAgent);
    _imgSeries->setContrastRoute(imageContrastRoute);
    _imgSeries->setContrastVolume(imageContrastVolume);
    _imgSeries->setContrastStartTime(imageContrastStartTime);
    _imgSeries->setContrastStopTime(imageContrastStopTime);
    _imgSeries->setContrastTotalDose(imageContrastTotalDose);
    _imgSeries->setContrastFlowRate(imageContrastFlowRate);
    _imgSeries->setContrastFlowDuration(imageContrastFlowDuration);
    _imgSeries->setContrastIngredient(imageContrastIngredient);
    _imgSeries->setContrastIngredientConcentration(imageContrastIngredientConcentration);
    _imgSeries->setAcquisitionDate(imageAcquisitionDate);
    _imgSeries->setAcquisitionTime(imageAcquisitionTime);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillPatient(::gdcm::Scanner& _scanner, const std::string& _dcmFile,
                                     ::fwMedData::Patient::sptr _patient)
{
    const std::string patientName      = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_PATIENT_NAME_TAG);
    const std::string patientId        = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_PATIENT_ID_TAG);
    const std::string patientBirthdate = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_PATIENT_BIRTHDATE_TAG);
    const std::string patientSex       = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_PATIENT_SEX_TAG);

    _patient->setName(patientName);
    _patient->setPatientId(patientId);
    _patient->setBirthdate(patientBirthdate);
    _patient->setSex(patientSex);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillStudy(::gdcm::Scanner& _scanner, const std::string& _dcmFile,
                                   ::fwMedData::Study::sptr _study)
{
    const std::string studyUID                    = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_STUDY_UID_TAG);
    const std::string studyID                     = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_STUDY_ID_TAG);
    const std::string studyDate                   = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_STUDY_DATE_TAG);
    const std::string studyTime                   = helper::GdcmHelper::getValue(_scanner, _dcmFile, s_STUDY_TIME_TAG);
    const std::string studyReferringPhysicianName = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                                 s_STUDY_REFERRING_PHYSICIAN_NAME_TAG);
    const std::string studyConsultingPhysicianName =
        helper::GdcmHelper::getValue(_scanner, _dcmFile, s_STUDY_CONSULTING_PHYSICIAN_NAME_TAG);
    const std::string studyDescription = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                      s_STUDY_DESCRIPTION_TAG);
    const std::string studyPatientAge = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                     s_STUDY_PATIENT_AGE_TAG);
    const std::string studyPatientSize = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                      s_STUDY_PATIENT_SIZE_TAG);
    const std::string studyPatientWeight = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                        s_STUDY_PATIENT_WEIGHT_TAG);
    const std::string studyPatientBodyMassIndex = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                               s_STUDY_PATIENT_BODY_MASS_INDEX_TAG);

    SLM_ASSERT("No study UID", !studyUID.empty());
    _study->setInstanceUID(studyUID);
    _study->setStudyID(studyID);
    _study->setDate(studyDate);
    _study->setTime(studyTime);
    _study->setReferringPhysicianName(studyReferringPhysicianName);
    _study->setConsultingPhysicianName(studyConsultingPhysicianName);
    _study->setDescription(studyDescription);
    _study->setPatientAge(studyPatientAge);
    _study->setPatientSize(studyPatientSize);
    _study->setPatientWeight(studyPatientWeight);
    _study->setPatientBodyMassIndex(studyPatientBodyMassIndex);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillEquipment(::gdcm::Scanner& _scanner, const std::string& _dcmFile,
                                       ::fwMedData::Equipment::sptr _equipment)
{
    const std::string equipmentInstitutionName = helper::GdcmHelper::getValue(_scanner, _dcmFile,
                                                                              s_EQUIPMENT_INSTITUTION_NAME_TAG);

    _equipment->setInstitutionName(equipmentInstitutionName);
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::preprocessImage(const ::fwData::Image::sptr& _img,
                                         const SeriesDBLazyReader::SeriesFilesType& _files)
{
    ::gdcm::Reader localReader;
    localReader.SetFileName(_files[0].c_str());
    localReader.Read();
    ::gdcm::File& gdcmReaderFile = localReader.GetFile();
    std::vector<double> origin    = ::gdcm::ImageHelper::GetOriginValue(gdcmReaderFile);
    std::vector<unsigned int> dim = ::gdcm::ImageHelper::GetDimensionsValue(gdcmReaderFile);
    std::vector<double> spacing   = ::gdcm::ImageHelper::GetSpacingValue(gdcmReaderFile);
    ::gdcm::PixelFormat pixelFormat = ::gdcm::ImageHelper::GetPixelFormatValue(gdcmReaderFile);
    std::vector<double> interceptSlope = ::gdcm::ImageHelper::GetRescaleInterceptSlopeValue(gdcmReaderFile);

    // Origin
    ::fwData::Image::Origin imgOrigin;
    imgOrigin[0] = origin[0];
    imgOrigin[1] = origin[1];
    imgOrigin[2] = origin[2];
    _img->setOrigin2(imgOrigin);

    // Spacing
    ::fwData::Image::Spacing imgSpacing;
    imgSpacing[0] = spacing[0];
    imgSpacing[1] = spacing[1];
    imgSpacing[2] = 1.0;
    _img->setSpacing2(imgSpacing);

    // Size
    ::fwData::Image::SizeType imgSize(3);
    imgSize.resize(3);
    imgSize[0] = dim[0];
    imgSize[1] = dim[1];
    imgSize[2] = _files.size();

    // Image type
    ::fwTools::Type imgType;
    ::gdcm::PixelFormat::ScalarType scalarType = pixelFormat.GetScalarType();

    ::gdcm::Rescaler r;
    r.SetIntercept(interceptSlope[0]);
    r.SetSlope(interceptSlope[1]);
    r.SetPixelFormat(scalarType);
    scalarType = r.ComputeInterceptSlopePixelType();
    OSLM_TRACE("Intercept = " << interceptSlope[0]);
    OSLM_TRACE("Slope = " << interceptSlope[1]);

    switch(scalarType)
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

    _img->setSize(imgSize);
    _img->setNumberOfComponents(numberOfComponents);
    _img->setType(imgType);
    _img->getDataArray()->resize(imgType, imgSize, numberOfComponents, false);
}

//------------------------------------------------------------------------------

double SeriesDBLazyReader::computeZSpacing(const SeriesDBLazyReader::SeriesFilesType& _seriesFiles)
{
    SLM_ASSERT("This method only work if series files contains at least 2 frames.", _seriesFiles.size() > 1);
    double zspacing = 0;

    ::gdcm::Reader r1;
    ::gdcm::Reader r2;

    r1.SetFileName(_seriesFiles[0].c_str());
    r2.SetFileName(_seriesFiles[1].c_str());

    bool canRead = r1.Read() && r2.Read();
    SLM_ASSERT("Reader must be ok", canRead);

    std::vector<double> vOrigin1 = ::gdcm::ImageHelper::GetOriginValue(r1.GetFile());
    std::vector<double> vOrigin2 = ::gdcm::ImageHelper::GetOriginValue(r2.GetFile());
    zspacing = vOrigin2[2] - vOrigin1[2];

    return zspacing;
}

//------------------------------------------------------------------------------

void SeriesDBLazyReader::fillImage(::gdcm::Scanner& _scanner, const SeriesDBLazyReader::SeriesFilesType& _seriesFiles,
                                   const std::string& _dcmFile, ::fwData::Image::sptr _img)
{

    this->preprocessImage(_img, _seriesFiles);

    double center = getNumericValue<double>(_scanner, _dcmFile, s_WINDOW_CENTER_TAG);
    double width  = getNumericValue<double>(_scanner, _dcmFile, s_WINDOW_WIDTH_TAG);

    _img->setWindowCenter(center);
    _img->setWindowWidth(width);

    ::fwData::Image::SpacingType imgSpacing = _img->getSpacing();
    imgSpacing.resize(3);
    double thickness = getNumericValue<double>(_scanner, _dcmFile, s_SLICE_THICKNESS_TAG);
    thickness = thickness ? thickness : 1.;
    if(_seriesFiles.size() > 1)
    {
        double computedZSpacing = computeZSpacing(_seriesFiles);
        imgSpacing[2] = computedZSpacing ? computedZSpacing : thickness;
    }
    else
    {
        imgSpacing[2] = thickness;
    }
    _img->setSpacing(imgSpacing);

    ::vtkGdcmIO::helper::ImageDicomInfo::sptr dcmInfo = std::make_shared< ::vtkGdcmIO::helper::ImageDicomInfo >();
    dcmInfo->m_buffSizeInBytes                        = _img->getSizeInBytes();
    dcmInfo->m_seriesFiles                            = _seriesFiles;

    ::fwMemory::BufferObject::sptr buffObj = _img->getDataArray()->getBufferObject();
    buffObj->setIStreamFactory(
        std::make_shared< ::vtkGdcmIO::helper::ImageDicomStream >(dcmInfo),
        _img->getSizeInBytes());
}

} //namespace vtkGdcmIO

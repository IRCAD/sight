/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/reader/ie/series.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <data/dicom_series.hpp>
#include <data/image_series.hpp>
#include <data/series.hpp>
#include <data/types.hpp>

#include <sstream>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

series::series(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::DicomInstance::sptr& _instance,
    const data::series::sptr& _series,
    const core::log::logger::sptr& _logger,
    ProgressCallback _progress,
    CancelRequestedCallback _cancel
) :
    io::dicom::reader::ie::InformationEntity<data::series>(_dicom_series, _reader, _instance, _series,
                                                           _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

series::~series()
= default;

//------------------------------------------------------------------------------

void series::readGeneralSeriesModule()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& modality = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0060>(dataset);
    m_object->setModality(modality);

    const std::string& uid = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000e>(dataset);
    m_object->setSeriesInstanceUID(uid);

    const std::string& number = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0011>(dataset);
    if(!number.empty())
    {
        m_object->setSeriesNumber(std::stoi(number));
    }

    const std::string& laterality = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0060>(dataset);
    m_object->setLaterality(laterality);

    const std::string& date = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0021>(dataset);
    m_object->setSeriesDate(date);

    const std::string& time = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0031>(dataset);
    m_object->setSeriesTime(time);

    const std::string& performing_physician_name =
        io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1050>(dataset);
    m_object->setPerformingPhysicianName(performing_physician_name);

    const std::string& protocol_name = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1030>(dataset);
    m_object->setProtocolName(protocol_name);

    const std::string& description = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x103e>(dataset);
    m_object->setSeriesDescription(description);

    const std::string& body_part_examined = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x0015>(dataset);
    m_object->setBodyPartExamined(body_part_examined);

    const std::string& patient_position = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x5100>(dataset);
    m_object->setPatientPosition(patient_position);

    const std::string& anatomical_orientation_type =
        io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x2210>(dataset);
    m_object->set_anatomical_orientation_type(anatomical_orientation_type);

    const std::string& performed_procedure_step_id =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0253>(dataset);
    m_object->setPerformedProcedureStepID(performed_procedure_step_id);

    const std::string& performed_procedure_step_start_date =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0244>(dataset);
    m_object->setPerformedProcedureStepStartDate(performed_procedure_step_start_date);

    const std::string& performed_procedure_step_start_time =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0245>(dataset);
    m_object->setPerformedProcedureStepStartTime(performed_procedure_step_start_time);

    const std::string& performed_procedure_step_end_date =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0250>(dataset);
    m_object->setPerformedProcedureStepEndDate(performed_procedure_step_end_date);

    const std::string& performed_procedure_step_end_time =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0251>(dataset);
    m_object->setPerformedProcedureStepEndTime(performed_procedure_step_end_time);

    const std::string& performed_procedure_step_description =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0254>(dataset);
    m_object->setPerformedProcedureStepDescription(performed_procedure_step_description);

    const std::string& performed_procedure_comments =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0280>(dataset);
    m_object->setCommentsOnThePerformedProcedureStep(performed_procedure_comments);

    const data::image_series::sptr image_series = std::dynamic_pointer_cast<data::image_series>(m_object);
    if(image_series)
    {
        const std::string& image_contrast_bolus_agent =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x0010>(dataset);
        image_series->setContrastBolusAgent(image_contrast_bolus_agent);

        const std::string& image_contrast_bolus_route =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1040>(dataset);
        image_series->setContrastBolusRoute(image_contrast_bolus_route);

        const std::string& image_contrast_bolus_volume =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1041>(
                dataset
            );

        if(!image_contrast_bolus_volume.empty())
        {
            image_series->setContrastBolusVolume(std::stod(image_contrast_bolus_volume));
        }

        const std::string& image_contrast_bolus_start_time =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1042>(dataset);
        image_series->setContrastBolusStartTime(image_contrast_bolus_start_time);

        const std::string& image_contrast_bolus_stop_time =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1043>(dataset);
        image_series->setContrastBolusStopTime(image_contrast_bolus_stop_time);

        const std::string& image_contrast_bolus_total_dose =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1044>(dataset);

        if(!image_contrast_bolus_total_dose.empty())
        {
            image_series->setContrastBolusTotalDose(std::stod(image_contrast_bolus_total_dose));
        }

        const std::string& image_contrast_flow_rate =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1046>(dataset);
        image_series->setContrastFlowRate(image_contrast_flow_rate);

        const std::string& image_contrast_flow_duration =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1047>(dataset);
        image_series->setContrastFlowDuration(image_contrast_flow_duration);

        const std::string& image_contrast_bolus_ingredient =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1048>(dataset);
        image_series->setContrastBolusIngredient(image_contrast_bolus_ingredient);

        const std::string& image_contrast_bolus_ingredient_concentration =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1049>(dataset);

        if(!image_contrast_bolus_ingredient_concentration.empty())
        {
            image_series->setContrastBolusIngredientConcentration(
                std::stod(
                    image_contrast_bolus_ingredient_concentration
                )
            );
        }

        const std::string& image_acquisition_date =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0022>(dataset);
        image_series->setAcquisitionDate(image_acquisition_date);

        const std::string& image_acquisition_time =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0032>(dataset);
        image_series->setAcquisitionTime(image_acquisition_time);
    }
}

} // namespace sight::io::dicom::reader::ie

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
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::series::sptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::ie::information_entity<data::series>(_dicom_series, _reader, _instance, _series,
                                                            _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

series::~series()
= default;

//------------------------------------------------------------------------------

void series::read_general_series_module()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& modality = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0060>(dataset);
    m_object->set_modality(modality);

    const std::string& uid = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x000e>(dataset);
    m_object->set_series_instance_uid(uid);

    const std::string& number = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x0011>(dataset);
    if(!number.empty())
    {
        m_object->set_series_number(std::stoi(number));
    }

    const std::string& laterality = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x0060>(dataset);
    m_object->set_laterality(laterality);

    const std::string& date = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0021>(dataset);
    m_object->set_series_date(date);

    const std::string& time = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0031>(dataset);
    m_object->set_series_time(time);

    const std::string& performing_physician_name =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x1050>(dataset);
    m_object->set_performing_physician_name(performing_physician_name);

    const std::string& protocol_name = io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1030>(dataset);
    m_object->set_protocol_name(protocol_name);

    const std::string& description = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x103e>(dataset);
    m_object->set_series_description(description);

    const std::string& body_part_examined =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x0015>(dataset);
    m_object->set_body_part_examined(body_part_examined);

    const std::string& patient_position = io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x5100>(dataset);
    m_object->set_patient_position(patient_position);

    const std::string& anatomical_orientation_type =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0010, 0x2210>(dataset);
    m_object->set_anatomical_orientation_type(anatomical_orientation_type);

    const std::string& performed_procedure_step_id =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x0253>(dataset);
    m_object->set_performed_procedure_step_id(performed_procedure_step_id);

    const std::string& performed_procedure_step_start_date =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x0244>(dataset);
    m_object->set_performed_procedure_step_start_date(performed_procedure_step_start_date);

    const std::string& performed_procedure_step_start_time =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x0245>(dataset);
    m_object->set_performed_procedure_step_start_time(performed_procedure_step_start_time);

    const std::string& performed_procedure_step_end_date =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x0250>(dataset);
    m_object->set_performed_procedure_step_end_date(performed_procedure_step_end_date);

    const std::string& performed_procedure_step_end_time =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x0251>(dataset);
    m_object->set_performed_procedure_step_end_time(performed_procedure_step_end_time);

    const std::string& performed_procedure_step_description =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x0254>(dataset);
    m_object->set_performed_procedure_step_description(performed_procedure_step_description);

    const std::string& performed_procedure_comments =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x0280>(dataset);
    m_object->set_comments_on_the_performed_procedure_step(performed_procedure_comments);

    const data::image_series::sptr image_series = std::dynamic_pointer_cast<data::image_series>(m_object);
    if(image_series)
    {
        const std::string& image_contrast_bolus_agent =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x0010>(dataset);
        image_series->set_contrast_bolus_agent(image_contrast_bolus_agent);

        const std::string& image_contrast_bolus_route =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1040>(dataset);
        image_series->set_contrast_bolus_route(image_contrast_bolus_route);

        const std::string& image_contrast_bolus_volume =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1041>(
                dataset
            );

        if(!image_contrast_bolus_volume.empty())
        {
            image_series->set_contrast_bolus_volume(std::stod(image_contrast_bolus_volume));
        }

        const std::string& image_contrast_bolus_start_time =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1042>(dataset);
        image_series->set_contrast_bolus_start_time(image_contrast_bolus_start_time);

        const std::string& image_contrast_bolus_stop_time =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1043>(dataset);
        image_series->set_contrast_bolus_stop_time(image_contrast_bolus_stop_time);

        const std::string& image_contrast_bolus_total_dose =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1044>(dataset);

        if(!image_contrast_bolus_total_dose.empty())
        {
            image_series->set_contrast_bolus_total_dose(std::stod(image_contrast_bolus_total_dose));
        }

        const std::string& image_contrast_flow_rate =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1046>(dataset);
        image_series->set_contrast_flow_rate(image_contrast_flow_rate);

        const std::string& image_contrast_flow_duration =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1047>(dataset);
        image_series->set_contrast_flow_duration(image_contrast_flow_duration);

        const std::string& image_contrast_bolus_ingredient =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1048>(dataset);
        image_series->set_contrast_bolus_ingredient(image_contrast_bolus_ingredient);

        const std::string& image_contrast_bolus_ingredient_concentration =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1049>(dataset);

        if(!image_contrast_bolus_ingredient_concentration.empty())
        {
            image_series->set_contrast_bolus_ingredient_concentration(
                std::stod(
                    image_contrast_bolus_ingredient_concentration
                )
            );
        }

        const std::string& image_acquisition_date =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0022>(dataset);
        image_series->set_acquisition_date(image_acquisition_date);

        const std::string& image_acquisition_time =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0032>(dataset);
        image_series->set_acquisition_time(image_acquisition_time);
    }
}

} // namespace sight::io::dicom::reader::ie

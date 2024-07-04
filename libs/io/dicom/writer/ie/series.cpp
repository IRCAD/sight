/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/writer/ie/series.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

#include <data/image_series.hpp>
#include <data/series.hpp>
#include <data/types.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

series::series(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::series::csptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::ie::information_entity<data::series>(_writer, _instance, _series,
                                                            _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

series::~series()
= default;

//------------------------------------------------------------------------------

void series::write_general_series_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0060>(m_object->get_modality_string(), dataset);

    // As the data may have been updated between two export, we regenerate an UID
    gdcm::UIDGenerator uid_generator;
    const std::string instance_uid = uid_generator.Generate();
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x000e>(instance_uid, dataset);

    if(const auto& series_number = m_object->get_series_number(); series_number)
    {
        io::dicom::helper::dicom_data_writer::set_tag_value<int, 0x0020, 0x0011>(*series_number, dataset);
    }

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x0060>(m_object->get_laterality(), dataset);

    boost::posix_time::ptime ptime = boost::posix_time::second_clock::local_time();
    const std::string fulldate     = boost::posix_time::to_iso_string(ptime);
    // Split iso time in YYYYMMDDTHHMMSS
    boost::char_separator<char> sep("T");
    boost::tokenizer<boost::char_separator<char> > tokens(fulldate, sep);
    boost::tokenizer<boost::char_separator<char> >::iterator tok_iter = tokens.begin();
    const std::string date                                            = *tok_iter;
    ++tok_iter;
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0021>(date, dataset);

    const std::string time = *tok_iter;
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0031>(time, dataset);

    // Performing physicians name - Type 3
    std::vector<std::string> performing_physician_names;
    boost::split(performing_physician_names, m_object->get_performing_physician_name(), boost::is_any_of("\\"));

    if(!performing_physician_names.empty())
    {
        auto* physicians   = new gdcm::String<>[performing_physician_names.size()];
        unsigned int count = 0;
        for(const auto& physician : performing_physician_names)
        {
            physicians[count++] = gdcm::String<>(physician);
        }

        io::dicom::helper::dicom_data_writer::set_tag_values<gdcm::String<>, 0x0008, 0x1050>(
            physicians,
            count,
            dataset
        );

        delete[] physicians;
    }

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x1030>(m_object->get_protocol_name(), dataset);

    // series' description
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x103e>(m_object->get_series_description(), dataset);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x0015>(m_object->get_body_part_examined(), dataset);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x5100>(m_object->get_patient_position(), dataset);
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0010, 0x2210>(
        m_object->get_anatomical_orientation_type(),
        dataset
    );

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0x0253>(
        m_object->get_performed_procedure_step_id(),
        dataset
    );

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0x0244>(
        m_object->get_performed_procedure_step_start_date(),
        dataset
    );

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0x0245>(
        m_object->get_performed_procedure_step_start_time(),
        dataset
    );

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0x0250>(
        m_object->get_performed_procedure_step_end_date(),
        dataset
    );

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0x0251>(
        m_object->get_performed_procedure_step_end_time(),
        dataset
    );

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0x0254>(
        m_object->get_performed_procedure_step_description(),
        dataset
    );

    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0x0280>(
        m_object->get_comments_on_the_performed_procedure_step(),
        dataset
    );

    const data::image_series::csptr image_series = std::dynamic_pointer_cast<const data::image_series>(m_object);
    if(image_series)
    {
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x0010>(
            image_series->get_contrast_bolus_agent(),
            dataset
        );
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x1040>(
            image_series->get_contrast_bolus_route(),
            dataset
        );

        if(const auto& contrast_bolus_volume = image_series->get_contrast_bolus_volume(); contrast_bolus_volume)
        {
            io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0018, 0x1041>(
                *contrast_bolus_volume,
                dataset
            );
        }

        io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x1042>(
            image_series->get_contrast_bolus_start_time(),
            dataset
        );

        io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x1043>(
            image_series->get_contrast_bolus_stop_time(),
            dataset
        );

        if(const auto& contrast_bolus_total_dose = image_series->get_contrast_bolus_total_dose();
           contrast_bolus_total_dose)
        {
            io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0018, 0x1044>(
                *contrast_bolus_total_dose,
                dataset
            );
        }

        if(const auto& contrast_flow_rates = image_series->get_contrast_flow_rates(); !contrast_flow_rates.empty())
        {
            io::dicom::helper::dicom_data_writer::set_tag_values<double, 0x0018, 0x1046>(
                contrast_flow_rates.data(),
                contrast_flow_rates.size(),
                dataset
            );
        }

        if(const auto& contrast_flow_durations = image_series->get_contrast_flow_durations();
           !contrast_flow_durations.empty())
        {
            io::dicom::helper::dicom_data_writer::set_tag_values<double, 0x0018, 0x1047>(
                contrast_flow_durations.data(),
                contrast_flow_durations.size(),
                dataset
            );
        }

        io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x1048>
            (image_series->get_contrast_bolus_ingredient(), dataset);

        if(const auto& contrast_bolus_ingredient_concentration =
               image_series->get_contrast_bolus_ingredient_concentration();
           contrast_bolus_ingredient_concentration)
        {
            io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0018, 0x1049>(
                *contrast_bolus_ingredient_concentration,
                dataset
            );
        }

        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0022>(
            image_series->get_acquisition_date(),
            dataset
        );
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0032>(
            image_series->get_acquisition_time(),
            dataset
        );
    }
}

//------------------------------------------------------------------------------

void series::write_segmentation_series_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // series' modality - Type 1
    dataset.Remove(gdcm::Tag(0x0008, 0x0060));
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0060>("SEG", dataset);
}

//------------------------------------------------------------------------------

void series::write_sr_document_series_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Create generator
    gdcm::UIDGenerator uid_generator;

    // series' modality - Type 1
    dataset.Remove(gdcm::Tag(0x0008, 0x0060));
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0060>("SR", dataset);

    // series' instance UID - Type 1
    dataset.Remove(gdcm::Tag(0x0020, 0x000e));
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x000e>(uid_generator.Generate(), dataset);

    // series' number - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<int, 0x0020, 0x0011>(0, dataset);

    // Referenced Performed Procedure Step Sequence  - Type 2
    io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0008, 0x1111>(dataset);
}

//------------------------------------------------------------------------------

void series::write_spatial_fiducials_series_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Create uid generator
    gdcm::UIDGenerator uid_generator;

    // series' instance UID - Type 1
    dataset.Remove(gdcm::Tag(0x0020, 0x000e));
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x000e>(uid_generator.Generate(), dataset);

    // series' modality - Type 1
    dataset.Remove(gdcm::Tag(0x0008, 0x0060));
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0060>("FID", dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie

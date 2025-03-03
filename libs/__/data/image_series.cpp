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

#include "data/image_series.hpp"

#include "data/detail/series_impl.hxx"

#include <data/exception.hpp>
#include <data/image.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::image_series)

namespace sight::data
{

image_series::image_series() :
    has_fiducials(m_signals)
{
}

//------------------------------------------------------------------------------

void image_series::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const image_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_dicom_reference  = other->m_dicom_reference;
    m_fiducials_series = other->m_fiducials_series;

    series::shallow_copy(other);

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void image_series::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const image_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_dicom_reference = data::object::copy(other->m_dicom_reference);

    m_fiducials_series = std::make_shared<fiducials_series>();
    m_fiducials_series->deep_copy(other->m_fiducials_series, _cache);

    series::deep_copy(other, _cache);

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool image_series::operator==(const image_series& _other) const noexcept
{
    if(!core::is_equal(m_dicom_reference, _other.m_dicom_reference))
    {
        return false;
    }

    // Super class last
    return series::operator==(_other) && base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool image_series::operator!=(const image_series& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

std::vector<double> image_series::window_center() const noexcept
{
    return series::window_center();
}

//------------------------------------------------------------------------------

void image_series::set_window_center(const std::vector<double>& _window_centers)
{
    series::set_window_center(_window_centers);
    image::set_window_center(_window_centers);
}

//------------------------------------------------------------------------------

void image_series::set_window_width(const std::vector<double>& _window_widths)
{
    series::set_window_width(_window_widths);
    image::set_window_width(_window_widths);
}

//------------------------------------------------------------------------------

std::vector<double> image_series::window_width() const noexcept
{
    return series::window_width();
}

//------------------------------------------------------------------------------

void image_series::set_rows(const std::optional<std::uint16_t>& _rows)
{
    series::set_rows(_rows);

    // Resize the image (if possible and needed...)
    if(const auto pixel_format = this->pixel_format(); pixel_format != sight::data::image::pixel_format_t::undefined)
    {
        const auto rows_value = _rows.value_or(1);

        if(const auto& original_size = size(); original_size[0] != rows_value)
        {
            resize({std::size_t(rows_value), original_size[1], original_size[2]}, type(), pixel_format);
        }
    }
}

//------------------------------------------------------------------------------

void image_series::set_columns(const std::optional<std::uint16_t>& _columns)
{
    series::set_columns(_columns);

    // Resize the image (if possible and needed...)
    if(const auto pixel_format = this->pixel_format(); pixel_format != sight::data::image::pixel_format_t::undefined)
    {
        const auto columns_value = _columns.value_or(1);

        if(const auto& original_size = size(); original_size[1] != columns_value)
        {
            resize({original_size[0], std::size_t(columns_value), original_size[2]}, type(), pixel_format);
        }
    }
}

//------------------------------------------------------------------------------

std::size_t image_series::resize(const image::size_t& _size, const core::type& _type, pixel_format_t _format)
{
    series::shrink_frames(_size[2]);
    return image::resize(_size, _type, _format);
}

//------------------------------------------------------------------------------

void image_series::set_image_position_patient(
    const std::vector<double>& _image_position_patient,
    const std::optional<std::size_t>& _frame_index
)
{
    // If we set the position the shared group, we also set the image origin
    if(!_frame_index)
    {
        image::set_origin({_image_position_patient[0], _image_position_patient[1], _image_position_patient[2]});
    }

    series::set_image_position_patient(_image_position_patient, _frame_index);
}

//------------------------------------------------------------------------------

void image_series::set_origin(const origin_t& _origin)
{
    image::set_origin(_origin);

    if(get_sop_keyword() != dicom::sop::Keyword::INVALID && is_multi_frame())
    {
        // We know if we are multi-frame or not, so we can use series method
        // Check if we are an ultrasound volume and if we have a specific acquisition geometry
        switch(get_ultrasound_acquisition_geometry())
        {
            case data::dicom::ultrasound_acquisition_geometry_t::apex:
            {
                // Search for Volume to Transducer Mapping Matrix
                if(auto transducer_mapping = get_volume_to_transducer_mapping_matrix(); transducer_mapping)
                {
                    transducer_mapping->set_position(_origin);
                    set_volume_to_transducer_mapping_matrix(transducer_mapping);
                }
                else
                {
                    matrix4 new_mapping;
                    new_mapping.set_position(_origin);
                    set_volume_to_transducer_mapping_matrix(new_mapping);
                }

                break;
            }

            case data::dicom::ultrasound_acquisition_geometry_t::patient:
            {
                if(auto table_mapping = get_volume_to_table_mapping_matrix(); table_mapping)
                {
                    table_mapping->set_position(_origin);
                    set_volume_to_table_mapping_matrix(table_mapping);
                }
                else
                {
                    matrix4 new_mapping;
                    new_mapping.set_position(_origin);
                    set_volume_to_table_mapping_matrix(new_mapping);
                }

                break;
            }

            default:
            {
                // This is the default case, we set the shared group if multi-frame or the root ImagePositionPatient
                // In either case it won't overwrite per-frame positions
                set_image_position_patient({_origin[0], _origin[1], _origin[2]}, std::nullopt);
                break;
            }
        }
    }
    else
    {
        // Store it as "global" ImagePositionPatient
        m_pimpl->set_values<gdcm::Keywords::ImagePositionPatient>({_origin[0], _origin[1], _origin[2]}, 0);
    }
}

//------------------------------------------------------------------------------

void image_series::set_image_orientation_patient(
    const std::vector<double>& _orientation,
    const std::optional<std::size_t>& _frame_index
)
{
    SIGHT_ASSERT(
        "The image orientation patient must have 6 direction cosines.",
        _orientation.size() == 6
    );

    // If we set the orientation the shared group, we also set the image orientation
    if(!_frame_index)
    {
        image::set_orientation(from_dicom_orientation(_orientation));
    }

    series::set_image_orientation_patient(_orientation, _frame_index);
}

//------------------------------------------------------------------------------

void image_series::set_orientation(const orientation_t& _orientation)
{
    image::set_orientation(_orientation);

    if(get_sop_keyword() != dicom::sop::Keyword::INVALID && is_multi_frame())
    {
        // We know if we are multi-frame or not, so we can use series method
        // Check if we are an ultrasound volume and if we have a specific acquisition geometry
        switch(get_ultrasound_acquisition_geometry())
        {
            case data::dicom::ultrasound_acquisition_geometry_t::apex:
            {
                // Search for Volume to Transducer Mapping Matrix
                if(auto transducer_mapping = get_volume_to_transducer_mapping_matrix(); transducer_mapping)
                {
                    transducer_mapping->set_orientation(_orientation);
                    set_volume_to_transducer_mapping_matrix(transducer_mapping);
                }
                else
                {
                    matrix4 new_mapping;
                    new_mapping.set_orientation(_orientation);
                    set_volume_to_transducer_mapping_matrix(new_mapping);
                }

                break;
            }

            case data::dicom::ultrasound_acquisition_geometry_t::patient:
            {
                if(auto table_mapping = get_volume_to_table_mapping_matrix(); table_mapping)
                {
                    table_mapping->set_orientation(_orientation);
                    set_volume_to_table_mapping_matrix(table_mapping);
                }
                else
                {
                    matrix4 new_mapping;
                    new_mapping.set_orientation(_orientation);
                    set_volume_to_table_mapping_matrix(new_mapping);
                }

                break;
            }

            default:
            {
                // This is the default case, we set the shared group if multi-frame or the root ImageOrientationPatient
                // In either case it won't overwrite per-frame positions
                set_image_orientation_patient(
                    {
                        _orientation[0], _orientation[3], _orientation[6],
                        _orientation[1], _orientation[4], _orientation[7],
                    },
                    std::nullopt
                );
                break;
            }
        }
    }
    else
    {
        // Store it as "global" ImageOrientationPatient
        m_pimpl->set_values<gdcm::Keywords::ImageOrientationPatient>(
            {
                _orientation[0], _orientation[3], _orientation[6],
                _orientation[1], _orientation[4], _orientation[7]
            },
            0
        );
    }
}

//------------------------------------------------------------------------------

void image_series::set_patient_id(const std::string& _patient_id)
{
    this->series::set_patient_id(_patient_id);
    this->m_fiducials_series->set_patient_id(_patient_id);
}

//------------------------------------------------------------------------------

void image_series::set_patient_name(const std::string& _patient_name)
{
    this->series::set_patient_name(_patient_name);
    this->m_fiducials_series->set_patient_id(_patient_name);
}

//------------------------------------------------------------------------------

void image_series::set_study_instance_uid(const std::string& _study_instance_uid)
{
    this->series::set_study_instance_uid(_study_instance_uid);
    this->m_fiducials_series->set_study_instance_uid(_study_instance_uid);
}

//------------------------------------------------------------------------------

void image_series::set_slice_thickness(const std::optional<double>& _slice_thickness)
{
    this->series::set_slice_thickness(_slice_thickness);
    if(_slice_thickness)
    {
        spacing_t spacing = this->spacing();
        spacing[2] = *_slice_thickness;
        this->set_spacing(spacing);
    }
}

} // namespace sight::data

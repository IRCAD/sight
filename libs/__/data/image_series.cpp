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
    if(!core::tools::is_equal(m_dicom_reference, _other.m_dicom_reference))
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
    if(const auto pixel_format = this->pixel_format(); pixel_format != sight::data::image::pixel_format::undefined)
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
    if(const auto pixel_format = this->pixel_format(); pixel_format != sight::data::image::pixel_format::undefined)
    {
        const auto columns_value = _columns.value_or(1);

        if(const auto& original_size = size(); original_size[1] != columns_value)
        {
            resize({original_size[0], std::size_t(columns_value), original_size[2]}, type(), pixel_format);
        }
    }
}

//------------------------------------------------------------------------------

std::size_t image_series::resize(const image::size_t& _size, const core::type& _type, enum pixel_format _format)
{
    series::shrink_frames(_size[2]);
    return image::resize(_size, _type, _format);
}

//------------------------------------------------------------------------------

fiducials_series::csptr image_series::get_fiducials() const
{
    return m_fiducials_series;
}

//------------------------------------------------------------------------------

fiducials_series::sptr image_series::get_fiducials()
{
    return m_fiducials_series;
}

} // namespace sight::data

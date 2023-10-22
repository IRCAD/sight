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

#include "data/detail/SeriesImpl.hxx"

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

    m_dicomReference  = other->m_dicomReference;
    m_fiducialsSeries = other->m_fiducialsSeries;

    series::shallow_copy(other);

    base_class::shallow_copy(other);
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

    m_dicomReference = data::object::copy(other->m_dicomReference);

    m_fiducialsSeries = std::make_shared<fiducials_series>();
    m_fiducialsSeries->deep_copy(other->m_fiducialsSeries, _cache);

    series::deep_copy(other, _cache);

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool image_series::operator==(const image_series& _other) const noexcept
{
    if(!core::tools::is_equal(m_dicomReference, _other.m_dicomReference))
    {
        return false;
    }

    // Super class last
    return series::operator==(_other) && base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool image_series::operator!=(const image_series& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

std::vector<double> image_series::getWindowCenter() const noexcept
{
    return series::getWindowCenter();
}

//------------------------------------------------------------------------------

void image_series::setWindowCenter(const std::vector<double>& _window_centers)
{
    series::setWindowCenter(_window_centers);
    image::setWindowCenter(_window_centers);
}

//------------------------------------------------------------------------------

void image_series::setWindowWidth(const std::vector<double>& _window_widths)
{
    series::setWindowWidth(_window_widths);
    image::setWindowWidth(_window_widths);
}

//------------------------------------------------------------------------------

std::vector<double> image_series::getWindowWidth() const noexcept
{
    return series::getWindowWidth();
}

//------------------------------------------------------------------------------

void image_series::setRows(const std::optional<std::uint16_t>& _rows)
{
    series::setRows(_rows);

    // Resize the image (if possible and needed...)
    if(const auto pixel_format = getPixelFormat(); pixel_format != sight::data::image::PixelFormat::UNDEFINED)
    {
        const auto rows_value = _rows.value_or(1);

        if(const auto& original_size = size(); original_size[0] != rows_value)
        {
            resize({std::size_t(rows_value), original_size[1], original_size[2]}, getType(), pixel_format);
        }
    }
}

//------------------------------------------------------------------------------

void image_series::setColumns(const std::optional<std::uint16_t>& _columns)
{
    series::setColumns(_columns);

    // Resize the image (if possible and needed...)
    if(const auto pixel_format = getPixelFormat(); pixel_format != sight::data::image::PixelFormat::UNDEFINED)
    {
        const auto columns_value = _columns.value_or(1);

        if(const auto& original_size = size(); original_size[1] != columns_value)
        {
            resize({original_size[0], std::size_t(columns_value), original_size[2]}, getType(), pixel_format);
        }
    }
}

//------------------------------------------------------------------------------

std::size_t image_series::resize(const Size& _size, const core::type& _type, PixelFormat _format)
{
    series::shrinkFrames(_size[2]);
    return image::resize(_size, _type, _format);
}

//------------------------------------------------------------------------------

fiducials_series::csptr image_series::getFiducials() const
{
    return m_fiducialsSeries;
}

//------------------------------------------------------------------------------

fiducials_series::sptr image_series::getFiducials()
{
    return m_fiducialsSeries;
}

//------------------------------------------------------------------------------

bool image_series::hasFiducials() const
{
    return !m_fiducialsSeries->getFiducialSets().empty();
}

} // namespace sight::data

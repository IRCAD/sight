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

#include "data/ImageSeries.hpp"

#include "data/detail/SeriesImpl.hxx"

#include <data/Exception.hpp>
#include <data/Image.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::ImageSeries)

namespace sight::data
{

ImageSeries::ImageSeries(data::Object::Key key) :
    Image(key),
    Series(key)
{
}

//------------------------------------------------------------------------------

void ImageSeries::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_dicomReference = other->m_dicomReference;

    Series::shallowCopy(other);

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void ImageSeries::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_dicomReference = data::Object::copy(other->m_dicomReference);

    Series::deepCopy(other, cache);

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

bool ImageSeries::operator==(const ImageSeries& other) const noexcept
{
    if(!core::tools::is_equal(m_dicomReference, other.m_dicomReference))
    {
        return false;
    }

    // Super class last
    return Series::operator==(other) && BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool ImageSeries::operator!=(const ImageSeries& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

std::vector<double> ImageSeries::getWindowCenter() const noexcept
{
    return Series::getWindowCenter();
}

//------------------------------------------------------------------------------

void ImageSeries::setWindowCenter(const std::vector<double>& windowCenters)
{
    Series::setWindowCenter(windowCenters);
    Image::setWindowCenter(windowCenters);
}

//------------------------------------------------------------------------------

void ImageSeries::setWindowWidth(const std::vector<double>& windowWidths)
{
    Series::setWindowWidth(windowWidths);
    Image::setWindowWidth(windowWidths);
}

//------------------------------------------------------------------------------

std::vector<double> ImageSeries::getWindowWidth() const noexcept
{
    return Series::getWindowWidth();
}

//------------------------------------------------------------------------------

void ImageSeries::setRows(const std::optional<std::uint16_t>& rows)
{
    Series::setRows(rows);

    // Resize the image (if possible and needed...)
    if(const auto pixel_format = getPixelFormat(); pixel_format != sight::data::Image::PixelFormat::UNDEFINED)
    {
        const auto rows_value = rows.value_or(1);

        if(const auto& original_size = getSize(); original_size[0] != rows_value)
        {
            resize({std::size_t(rows_value), original_size[1], original_size[2]}, getType(), pixel_format);
        }
    }
}

//------------------------------------------------------------------------------

void ImageSeries::setColumns(const std::optional<std::uint16_t>& columns)
{
    Series::setColumns(columns);

    // Resize the image (if possible and needed...)
    if(const auto pixel_format = getPixelFormat(); pixel_format != sight::data::Image::PixelFormat::UNDEFINED)
    {
        const auto columns_value = columns.value_or(1);

        if(const auto& original_size = getSize(); original_size[1] != columns_value)
        {
            resize({original_size[0], std::size_t(columns_value), original_size[2]}, getType(), pixel_format);
        }
    }
}

} // namespace sight::data

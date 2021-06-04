/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Exception.hpp>
#include <data/Image.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::ImageSeries)

namespace sight::data
{

ImageSeries::ImageSeries(data::Object::Key _key) :
    Series(_key)
{
}

//------------------------------------------------------------------------------

ImageSeries::~ImageSeries()
{
}

//------------------------------------------------------------------------------

void ImageSeries::shallowCopy(const data::Object::csptr& _source)
{
    ImageSeries::csptr other = ImageSeries::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->data::Series::shallowCopy(_source);

    m_image          = other->m_image;
    m_dicomReference = other->m_dicomReference;
}

//------------------------------------------------------------------------------

void ImageSeries::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& _cache)
{
    ImageSeries::csptr other = ImageSeries::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->data::Series::cachedDeepCopy(_source, _cache);

    m_image          = data::Object::copy(other->m_image);
    m_dicomReference = data::Object::copy(other->m_dicomReference);
}

//------------------------------------------------------------------------------

} // namespace sight::data

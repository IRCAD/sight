/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
= default;

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

    m_contrastBolusAgent                   = other->m_contrastBolusAgent;
    m_contrastBolusRoute                   = other->m_contrastBolusRoute;
    m_contrastBolusVolume                  = other->m_contrastBolusVolume;
    m_contrastBolusStartTime               = other->m_contrastBolusStartTime;
    m_contrastBolusStopTime                = other->m_contrastBolusStopTime;
    m_contrastBolusTotalDose               = other->m_contrastBolusTotalDose;
    m_contrastBolusFlowRate                = other->m_contrastBolusFlowRate;
    m_contrastBolusFlowDuration            = other->m_contrastBolusFlowDuration;
    m_contrastBolusIngredient              = other->m_contrastBolusIngredient;
    m_contrastBolusIngredientConcentration = other->m_contrastBolusIngredientConcentration;
    m_acquisitionDate                      = other->m_acquisitionDate;
    m_acquisitionTime                      = other->m_acquisitionTime;
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

    m_contrastBolusAgent                   = other->m_contrastBolusAgent;
    m_contrastBolusRoute                   = other->m_contrastBolusRoute;
    m_contrastBolusVolume                  = other->m_contrastBolusVolume;
    m_contrastBolusStartTime               = other->m_contrastBolusStartTime;
    m_contrastBolusStopTime                = other->m_contrastBolusStopTime;
    m_contrastBolusTotalDose               = other->m_contrastBolusTotalDose;
    m_contrastBolusFlowRate                = other->m_contrastBolusFlowRate;
    m_contrastBolusFlowDuration            = other->m_contrastBolusFlowDuration;
    m_contrastBolusIngredient              = other->m_contrastBolusIngredient;
    m_contrastBolusIngredientConcentration = other->m_contrastBolusIngredientConcentration;
    m_acquisitionDate                      = other->m_acquisitionDate;
    m_acquisitionTime                      = other->m_acquisitionTime;
}

//------------------------------------------------------------------------------

bool ImageSeries::operator==(const ImageSeries& other) const noexcept
{
    if(!core::tools::is_equal(m_image, other.m_image)
       || !core::tools::is_equal(m_dicomReference, other.m_dicomReference)
       || m_contrastBolusAgent != other.m_contrastBolusAgent
       || m_contrastBolusRoute != other.m_contrastBolusRoute
       || m_contrastBolusVolume != other.m_contrastBolusVolume
       || m_contrastBolusStartTime != other.m_contrastBolusStartTime
       || m_contrastBolusStopTime != other.m_contrastBolusStopTime
       || m_contrastBolusTotalDose != other.m_contrastBolusTotalDose
       || m_contrastBolusFlowRate != other.m_contrastBolusFlowRate
       || m_contrastBolusFlowDuration != other.m_contrastBolusFlowDuration
       || m_contrastBolusIngredient != other.m_contrastBolusIngredient
       || m_contrastBolusIngredientConcentration != other.m_contrastBolusIngredientConcentration
       || m_acquisitionDate != other.m_acquisitionDate
       || m_acquisitionTime != other.m_acquisitionTime)
    {
        return false;
    }

    // Super class last
    return Series::operator==(other);
}

//------------------------------------------------------------------------------

bool ImageSeries::operator!=(const ImageSeries& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data

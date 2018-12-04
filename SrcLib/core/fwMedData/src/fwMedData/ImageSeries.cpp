/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwMedData/ImageSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/Image.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::ImageSeries );

namespace fwMedData
{

ImageSeries::ImageSeries(::fwData::Object::Key key) :
    Series(key)
{
}

//------------------------------------------------------------------------------

ImageSeries::~ImageSeries()
{
}

//------------------------------------------------------------------------------

void ImageSeries::shallowCopy(const ::fwData::Object::csptr& _source)
{
    ImageSeries::csptr other = ImageSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);

    m_image          = other->m_image;
    m_dicomReference = other->m_dicomReference;
}

//------------------------------------------------------------------------------

void ImageSeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    ImageSeries::csptr other = ImageSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    m_image          = ::fwData::Object::copy(other->m_image);
    m_dicomReference = ::fwData::Object::copy(other->m_dicomReference);
}

//------------------------------------------------------------------------------

} // namespace fwMedData

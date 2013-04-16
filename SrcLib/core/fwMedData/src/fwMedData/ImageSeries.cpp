/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/Image.hpp>
#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/ImageSeries.hpp"

fwDataRegisterMacro( ::fwMedData::ImageSeries );

namespace fwMedData
{

ImageSeries::ImageSeries(::fwData::Object::Key key): Series(key)
{
}

//------------------------------------------------------------------------------

ImageSeries::~ImageSeries()
{}

//------------------------------------------------------------------------------

void ImageSeries::shallowCopy(const ::fwData::Object::csptr &_source)
{
    ImageSeries::csptr other = ImageSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );

    m_attrImage = other->m_attrImage;
}

//------------------------------------------------------------------------------

void ImageSeries::deepCopy(const ::fwData::Object::csptr &_source)
{
    ImageSeries::csptr other = ImageSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other );

    m_attrImage = ::fwData::Object::copy(other->m_attrImage);
}

//------------------------------------------------------------------------------

} // namespace fwMedData


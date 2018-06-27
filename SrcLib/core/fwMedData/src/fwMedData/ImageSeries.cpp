/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

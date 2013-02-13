/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/Image.hpp>
#include "fwData/registry/macros.hpp"

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

void ImageSeries::shallowCopy(ImageSeries::csptr _src)
{
    this->fieldShallowCopy( _src );

    m_attrImage = _src->m_attrImage;
}

//------------------------------------------------------------------------------

void ImageSeries::deepCopy(ImageSeries::csptr _src)
{
    this->fieldDeepCopy( _src );

    m_attrImage = ::fwData::Object::copy(_src->m_attrImage);
}

//------------------------------------------------------------------------------

} // namespace fwMedData


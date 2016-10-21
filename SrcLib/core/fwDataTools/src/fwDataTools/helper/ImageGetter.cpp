/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/ImageGetter.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Composite.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransferFunction.hpp>

namespace fwDataTools
{
namespace helper
{

//-----------------------------------------------------------------------------

ImageGetter::ImageGetter( ::fwData::Image::csptr ImageGetter )
    : m_image(ImageGetter)
{
    if ( ImageGetter )
    {
        m_lock = ImageGetter->getDataArray()->getBufferObject()->lock();
    }
}

//-----------------------------------------------------------------------------

ImageGetter::~ImageGetter()
{
}

//-----------------------------------------------------------------------------

void* ImageGetter::getBuffer() const
{
    return m_lock.getBuffer();
}

//------------------------------------------------------------------------------

void* ImageGetter::getPixelBuffer( SizeType::value_type x, SizeType::value_type y, SizeType::value_type z ) const
{
    SizeType size    = m_image->getSize();
    IndexType offset = x + size[0]*y + z*size[0]*size[1];
    return this->getPixelBuffer(offset);
}

//------------------------------------------------------------------------------

void* ImageGetter::getPixelBuffer( IndexType index ) const
{
    ::boost::uint8_t ImageGetterPixelSize = m_image->getType().sizeOf();
    BufferType* buf          = static_cast < BufferType* > (this->getBuffer());
    BufferIndexType bufIndex = index * ImageGetterPixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

const std::string ImageGetter::getPixelAsString(SizeType::value_type x,
                                                SizeType::value_type y,
                                                SizeType::value_type z ) const
{
    return m_image->getType().toString(this->getPixelBuffer(x,y,z));
}

//------------------------------------------------------------------------------

} // helper
} // fwDataTools

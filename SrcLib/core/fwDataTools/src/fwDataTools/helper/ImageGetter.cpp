/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

ImageGetter::ImageGetter( ::fwData::Image::csptr ImageGetter ) :
    m_image(ImageGetter)
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
    std::uint8_t imageGetterPixelSize = m_image->getType().sizeOf() * m_image->getNumberOfComponents();
    BufferType* buf                   = static_cast < BufferType* > (this->getBuffer());
    BufferIndexType bufIndex          = index * imageGetterPixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

const std::string ImageGetter::getPixelAsString(SizeType::value_type x,
                                                SizeType::value_type y,
                                                SizeType::value_type z ) const
{
    return m_image->getType().toString(this->getPixelBuffer(x, y, z));
}

//------------------------------------------------------------------------------

} // helper
} // fwDataTools

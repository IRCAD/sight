/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/helper/ImageGetter.hpp"

#include <data/Composite.hpp>
#include <data/PointList.hpp>
#include <data/TransferFunction.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

namespace sight::data
{

namespace helper
{

//-----------------------------------------------------------------------------

ImageGetter::ImageGetter(data::Image::csptr ImageGetter) :
    m_image(ImageGetter)
{
    FW_DEPRECATED_MSG(
        "::data::helper::ImageGetter is no longer supported, the methods have been moved to "
        "::sight::data::Image",
        "22.0"
    )
    if(ImageGetter)
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

void* ImageGetter::getPixelBuffer(SizeType::value_type x, SizeType::value_type y, SizeType::value_type z) const
{
    SizeType size    = m_image->getSize();
    IndexType offset = x + size[0] * y + z * size[0] * size[1];
    return this->getPixelBuffer(offset);
}

//------------------------------------------------------------------------------

void* ImageGetter::getPixelBuffer(IndexType index) const
{
    std::uint8_t imageGetterPixelSize = m_image->getType().sizeOf() * (std::uint8_t) m_image->getNumberOfComponents();
    BufferType* buf                   = static_cast<BufferType*>(this->getBuffer());
    BufferIndexType bufIndex          = index * imageGetterPixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

const std::string ImageGetter::getPixelAsString(
    SizeType::value_type x,
    SizeType::value_type y,
    SizeType::value_type z
) const
{
    return m_image->getType().toString(this->getPixelBuffer(x, y, z));
}

//------------------------------------------------------------------------------

} // helper

} // fwDataTools

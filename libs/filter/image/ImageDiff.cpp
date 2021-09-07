/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "ImageDiff.hpp"

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

ImageDiff::ImageDiff(const size_t imageElementSize, const size_t reservedElements) :
    m_imgEltSize(imageElementSize),
    m_eltSize(imageElementSize * 2 + sizeof(data::Image::IndexType)),
    m_nbElts(0),
    m_reservedSize(reservedElements * imageElementSize),
    m_buffer(nullptr)
{
}

//------------------------------------------------------------------------------

ImageDiff::~ImageDiff()
{
    free(m_buffer);
}

//------------------------------------------------------------------------------

ImageDiff::ImageDiff(const ImageDiff& other) :
    m_imgEltSize(other.m_imgEltSize),
    m_eltSize(other.m_eltSize),
    m_nbElts(other.m_nbElts),
    m_reservedSize(other.m_reservedSize),
    m_buffer(reinterpret_cast<std::uint8_t*>(malloc(other.m_reservedSize)))
{
    std::memcpy(m_buffer, other.m_buffer, m_reservedSize);
}

//------------------------------------------------------------------------------

ImageDiff::ImageDiff(ImageDiff&& other) :
    m_imgEltSize(other.m_imgEltSize),
    m_eltSize(other.m_eltSize),
    m_nbElts(other.m_nbElts),
    m_reservedSize(other.m_reservedSize),
    m_buffer(other.m_buffer)
{
    other.m_buffer = nullptr;
}

//------------------------------------------------------------------------------

ImageDiff& ImageDiff::operator=(const ImageDiff& other)
{
    ImageDiff tmpImageDiffs(other);
    *this = std::move(tmpImageDiffs);

    return *this;
}

//------------------------------------------------------------------------------

ImageDiff& ImageDiff::operator=(ImageDiff&& other)
{
    free(m_buffer);

    m_buffer       = other.m_buffer;
    other.m_buffer = nullptr;

    m_imgEltSize   = other.m_imgEltSize;
    m_eltSize      = other.m_eltSize;
    m_nbElts       = other.m_nbElts;
    m_reservedSize = other.m_reservedSize;

    return *this;
}

//------------------------------------------------------------------------------

void ImageDiff::addDiff(const ImageDiff& diff)
{
    SIGHT_ASSERT("Diff elements must be the same size.", m_eltSize == diff.m_eltSize);

    const size_t oldSize = this->getSize();
    const size_t newSize = oldSize + diff.getSize();

    if(m_reservedSize < newSize || m_buffer == nullptr)
    {
        // Double the reserved size.
        m_reservedSize = newSize + m_reservedSize * 2;
        m_buffer       = reinterpret_cast<std::uint8_t*>(realloc(m_buffer, m_reservedSize));

        SIGHT_FATAL_IF("Reallocation failed.", m_buffer == nullptr);
    }

    std::uint8_t* eltPtr = (m_buffer + oldSize);
    std::memcpy(eltPtr, diff.m_buffer, diff.getSize());

    m_nbElts += diff.m_nbElts;
}

//-----------------------------------------------------------------------------

void ImageDiff::addDiff(
    const data::Image::IndexType index,
    const data::Image::BufferType* oldValue,
    const data::Image::BufferType* newValue
)
{
    const size_t oldSize = this->getSize();
    const size_t newSize = oldSize + m_eltSize;

    if(m_reservedSize < newSize || m_buffer == nullptr)
    {
        // Double the reserved size.
        m_reservedSize = m_reservedSize * 2 + m_eltSize;
        m_buffer       = reinterpret_cast<std::uint8_t*>(realloc(m_buffer, m_reservedSize));

        SIGHT_FATAL_IF("Reallocation failed.", m_buffer == nullptr);
    }

    std::uint8_t* eltPtr = (m_buffer + oldSize);

    std::memcpy(eltPtr, &index, sizeof(index));
    size_t offset = sizeof(index);
    std::memcpy(eltPtr + offset, oldValue, m_imgEltSize);
    offset += m_imgEltSize;
    std::memcpy(eltPtr + offset, newValue, m_imgEltSize);

    m_nbElts++;
}

//------------------------------------------------------------------------------

void ImageDiff::applyDiff(const data::Image::sptr& img) const
{
    const auto dumpLock = img->lock();

    for(size_t i = 0 ; i < m_nbElts ; ++i)
    {
        applyDiffElt(img, i);
    }
}

//------------------------------------------------------------------------------

void ImageDiff::revertDiff(const data::Image::sptr& img) const
{
    const auto dumpLock = img->lock();

    for(size_t i = 0 ; i < m_nbElts ; ++i)
    {
        revertDiffElt(img, i);
    }
}

//------------------------------------------------------------------------------

size_t ImageDiff::getSize() const
{
    return m_nbElts * m_eltSize;
}

//------------------------------------------------------------------------------

size_t ImageDiff::getNumberOfElements() const
{
    return m_nbElts;
}

//------------------------------------------------------------------------------

void ImageDiff::clear()
{
    m_nbElts = 0;
}

//------------------------------------------------------------------------------

void ImageDiff::shrink()
{
    m_reservedSize = this->getSize();
    m_buffer       = reinterpret_cast<std::uint8_t*>(realloc(m_buffer, m_reservedSize));

    SIGHT_FATAL_IF("Reallocation failed.", m_buffer == nullptr);
}

//------------------------------------------------------------------------------

ImageDiff::ElementType ImageDiff::getElement(size_t index) const
{
    std::uint8_t* eltPtr = m_buffer + index * m_eltSize;
    ElementType elt;
    elt.m_index = *reinterpret_cast<data::Image::IndexType*>(eltPtr);

    size_t offset = sizeof(data::Image::IndexType);

    elt.m_oldValue = reinterpret_cast<data::Image::BufferType*>(eltPtr + offset);

    offset += m_imgEltSize;

    elt.m_newValue = reinterpret_cast<data::Image::BufferType*>(eltPtr + offset);

    return elt;
}

//------------------------------------------------------------------------------

void ImageDiff::applyDiffElt(const data::Image::sptr& img, size_t eltIndex) const
{
    std::uint8_t* eltPtr               = m_buffer + eltIndex * m_eltSize;
    const data::Image::IndexType index = *reinterpret_cast<data::Image::IndexType*>(eltPtr);

    const size_t offset = sizeof(index) + m_imgEltSize;

    data::Image::BufferType* newValue = reinterpret_cast<data::Image::BufferType*>(eltPtr + offset);

    img->setPixelBuffer(index, newValue);
}

//------------------------------------------------------------------------------

void ImageDiff::revertDiffElt(const data::Image::sptr& img, size_t eltIndex) const
{
    std::uint8_t* eltPtr               = m_buffer + eltIndex * m_eltSize;
    const data::Image::IndexType index = *reinterpret_cast<data::Image::IndexType*>(eltPtr);

    const size_t offset = sizeof(index);

    data::Image::BufferType* oldValue = reinterpret_cast<data::Image::BufferType*>(eltPtr + offset);

    img->setPixelBuffer(index, oldValue);
}

} // namespace sight::filter::image

/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

ImageDiff::ImageDiff(const std::size_t imageElementSize, const std::size_t reservedElements) :
    m_imgEltSize(imageElementSize),
    m_eltSize(imageElementSize * 2 + sizeof(data::Image::IndexType))
{
    m_buffer.reserve(reservedElements);
}

//------------------------------------------------------------------------------

void ImageDiff::addDiff(const ImageDiff& diff)
{
    SIGHT_ASSERT("Diff elements must be the same size.", m_eltSize == diff.m_eltSize);

    const std::size_t oldSize = this->size();
    const std::size_t newSize = oldSize + diff.size();

    m_buffer.reserve(newSize);
    std::copy(diff.m_buffer.begin(), diff.m_buffer.end(), std::back_inserter(m_buffer));
}

//-----------------------------------------------------------------------------

void ImageDiff::addDiff(
    data::Image::IndexType index,
    const data::Image::BufferType* oldValue,
    const data::Image::BufferType* newValue
)
{
    const std::size_t oldSize = this->size();
    const std::size_t newSize = oldSize + m_eltSize;

    m_buffer.reserve(newSize);
    std::copy_n(reinterpret_cast<std::uint8_t*>(&index), sizeof(data::Image::IndexType), std::back_inserter(m_buffer));
    std::copy_n(oldValue, m_imgEltSize, std::back_inserter(m_buffer));
    std::copy_n(newValue, m_imgEltSize, std::back_inserter(m_buffer));
}

//------------------------------------------------------------------------------

void ImageDiff::applyDiff(const data::Image::sptr& img) const
{
    const auto dumpLock = img->dump_lock();

    for(std::size_t i = 0 ; i < numElements() ; ++i)
    {
        applyDiffElt(img, i);
    }
}

//------------------------------------------------------------------------------

void ImageDiff::revertDiff(const data::Image::sptr& img) const
{
    const auto dumpLock = img->dump_lock();

    for(std::size_t i = 0 ; i < numElements() ; ++i)
    {
        revertDiffElt(img, i);
    }
}

//------------------------------------------------------------------------------

std::size_t ImageDiff::size() const
{
    return m_buffer.size();
}

//------------------------------------------------------------------------------

std::size_t ImageDiff::numElements() const
{
    return size() / m_eltSize;
}

//------------------------------------------------------------------------------

void ImageDiff::clear()
{
    m_buffer.clear();
}

//------------------------------------------------------------------------------

void ImageDiff::shrink()
{
    m_buffer.shrink_to_fit();
}

//------------------------------------------------------------------------------

ImageDiff::ElementType ImageDiff::getElement(std::size_t index) const
{
    ElementType elt {};

    elt.m_index = *reinterpret_cast<const data::Image::IndexType*>(&m_buffer[index * m_eltSize]);

    elt.m_oldValue = &m_buffer[index * m_eltSize + sizeof(data::Image::IndexType)];
    elt.m_newValue = &m_buffer[index * m_eltSize + sizeof(data::Image::IndexType) + m_imgEltSize];

    return elt;
}

//------------------------------------------------------------------------------

void ImageDiff::applyDiffElt(const data::Image::sptr& img, std::size_t eltIndex) const
{
    ElementType elt = getElement(eltIndex);
    img->setPixel(elt.m_index, elt.m_newValue);
}

//------------------------------------------------------------------------------

void ImageDiff::revertDiffElt(const data::Image::sptr& img, std::size_t eltIndex) const
{
    ElementType elt = getElement(eltIndex);
    img->setPixel(elt.m_index, elt.m_oldValue);
}

} // namespace sight::filter::image

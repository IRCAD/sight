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

#include "image_diff.hpp"

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

image_diff::image_diff(const std::size_t _image_element_size, const std::size_t _reserved_elements) :
    m_imgEltSize(_image_element_size),
    m_eltSize(_image_element_size * 2 + sizeof(data::image::index_t))
{
    m_buffer.reserve(_reserved_elements);
}

//------------------------------------------------------------------------------

void image_diff::addDiff(const image_diff& _diff)
{
    SIGHT_ASSERT("Diff elements must be the same size.", m_eltSize == _diff.m_eltSize);

    const std::size_t old_size = this->size();
    const std::size_t new_size = old_size + _diff.size();

    m_buffer.reserve(new_size);
    std::copy(_diff.m_buffer.begin(), _diff.m_buffer.end(), std::back_inserter(m_buffer));
}

//-----------------------------------------------------------------------------

void image_diff::addDiff(
    data::image::index_t _index,
    const data::image::buffer_t* _old_value,
    const data::image::buffer_t* _new_value
)
{
    const std::size_t old_size = this->size();
    const std::size_t new_size = old_size + m_eltSize;

    m_buffer.reserve(new_size);
    std::copy_n(reinterpret_cast<std::uint8_t*>(&_index), sizeof(data::image::index_t), std::back_inserter(m_buffer));
    std::copy_n(_old_value, m_imgEltSize, std::back_inserter(m_buffer));
    std::copy_n(_new_value, m_imgEltSize, std::back_inserter(m_buffer));
}

//------------------------------------------------------------------------------

void image_diff::applyDiff(const data::image::sptr& _img) const
{
    const auto dump_lock = _img->dump_lock();

    for(std::size_t i = 0 ; i < numElements() ; ++i)
    {
        applyDiffElt(_img, i);
    }
}

//------------------------------------------------------------------------------

void image_diff::revertDiff(const data::image::sptr& _img) const
{
    const auto dump_lock = _img->dump_lock();

    for(std::size_t i = 0 ; i < numElements() ; ++i)
    {
        revertDiffElt(_img, i);
    }
}

//------------------------------------------------------------------------------

std::size_t image_diff::size() const
{
    return m_buffer.size();
}

//------------------------------------------------------------------------------

std::size_t image_diff::numElements() const
{
    return size() / m_eltSize;
}

//------------------------------------------------------------------------------

void image_diff::clear()
{
    m_buffer.clear();
}

//------------------------------------------------------------------------------

void image_diff::shrink()
{
    m_buffer.shrink_to_fit();
}

//------------------------------------------------------------------------------

image_diff::element_t image_diff::getElement(std::size_t _index) const
{
    element_t elt {};

    elt.m_index = *reinterpret_cast<const data::image::index_t*>(&m_buffer[_index * m_eltSize]);

    elt.m_oldValue = &m_buffer[_index * m_eltSize + sizeof(data::image::index_t)];
    elt.m_newValue = &m_buffer[_index * m_eltSize + sizeof(data::image::index_t) + m_imgEltSize];

    return elt;
}

//------------------------------------------------------------------------------

void image_diff::applyDiffElt(const data::image::sptr& _img, std::size_t _elt_index) const
{
    element_t elt = getElement(_elt_index);
    _img->setPixel(elt.m_index, elt.m_newValue);
}

//------------------------------------------------------------------------------

void image_diff::revertDiffElt(const data::image::sptr& _img, std::size_t _elt_index) const
{
    element_t elt = getElement(_elt_index);
    _img->setPixel(elt.m_index, elt.m_oldValue);
}

} // namespace sight::filter::image

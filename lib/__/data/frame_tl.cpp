/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/frame_tl.hpp"

#include <core/base.hpp>

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::frame_tl);

namespace sight::data
{

//------------------------------------------------------------------------------

frame_tl::frame_tl()
{
    // Default to 100 frames since images used to eat a lot of memory...
    this->set_maximum_size(100);
}

//------------------------------------------------------------------------------

void frame_tl::shallow_copy(const object::csptr& /*unused*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

void frame_tl::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const frame_tl>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clear_timeline();

    this->init_pool_size(other->m_width, other->m_height, other->m_type, other->m_pixel_format);

    for(const auto& elt : other->m_timeline)
    {
        SPTR(data::timeline::buffer) tl_obj = this->create_buffer(elt.first);
        tl_obj->deep_copy(*elt.second);
        m_timeline.insert(timeline_t::value_type(elt.first, tl_obj));
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

void frame_tl::init_pool_size(
    std::size_t _width,
    std::size_t _height,
    const core::type& _type,
    const enum pixel_format _format,
    unsigned int _max_element_num
)
{
    m_width        = _width;
    m_height       = _height;
    m_type         = _type;
    m_pixel_format = _format;
    switch(_format)
    {
        case data::frame_tl::pixel_format::gray_scale:
            m_number_of_components = 1;
            break;

        case data::frame_tl::pixel_format::bgr:
        case data::frame_tl::pixel_format::rgb:
            m_number_of_components = 3;
            break;

        case data::frame_tl::pixel_format::bgra:
        case data::frame_tl::pixel_format::rgba:
            m_number_of_components = 4;
            break;

        default:
            m_number_of_components = 1;
    }

    std::size_t size = _width * _height * m_number_of_components * _type.size();

    SIGHT_ASSERT("width or height or numberOfComponents is null", size != 0);

    m_max_element_num = _max_element_num;
    this->alloc_pool_size(size * m_max_element_num);
}

//------------------------------------------------------------------------------

void frame_tl::init_pool_size(unsigned int /*maxElementNum*/)
{
    SIGHT_ERROR("This function should not be called");
}

//------------------------------------------------------------------------------

bool frame_tl::operator==(const frame_tl& _other) const noexcept
{
    if(m_width != _other.m_width
       || m_height != _other.m_height
       || m_number_of_components != _other.m_number_of_components
       || m_type != _other.m_type
       || m_pixel_format != _other.m_pixel_format)
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool frame_tl::operator!=(const frame_tl& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data

/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "data/material.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::material);

namespace sight::data
{

//------------------------------------------------------------------------------

const core::com::signals::key_t material::ADDED_TEXTURE_SIG   = "addedTexture";
const core::com::signals::key_t material::REMOVED_TEXTURE_SIG = "removedTexture";

//------------------------------------------------------------------------------

material::material() :

    m_ambient(std::make_shared<color>(0.8F, 0.8F, 0.8F, 1.F)),
    m_diffuse(std::make_shared<color>())
{
    new_signal<added_texture_signal_t>(ADDED_TEXTURE_SIG);
    new_signal<removed_texture_signal_t>(REMOVED_TEXTURE_SIG);
}

//------------------------------------------------------------------------------

void material::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const material>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_ambient         = other->m_ambient;
    m_diffuse         = other->m_diffuse;
    m_diffuse_texture = other->m_diffuse_texture;

    m_shading_mode              = other->m_shading_mode;
    m_representation_mode       = other->m_representation_mode;
    m_options_mode              = other->m_options_mode;
    m_diffuse_texture_filtering = other->m_diffuse_texture_filtering;
    m_diffuse_texture_wrapping  = other->m_diffuse_texture_wrapping;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void material::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const material>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_ambient         = data::object::copy(other->m_ambient, _cache);
    m_diffuse         = data::object::copy(other->m_diffuse, _cache);
    m_diffuse_texture = data::object::copy(other->m_diffuse_texture, _cache);

    m_shading_mode              = other->m_shading_mode;
    m_representation_mode       = other->m_representation_mode;
    m_options_mode              = other->m_options_mode;
    m_diffuse_texture_filtering = other->m_diffuse_texture_filtering;
    m_diffuse_texture_wrapping  = other->m_diffuse_texture_wrapping;

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

color::sptr material::ambient() const
{
    return m_ambient;
}

//------------------------------------------------------------------------------

color::sptr material::diffuse() const
{
    return m_diffuse;
}

//------------------------------------------------------------------------------

image::sptr material::get_diffuse_texture()
{
    return m_diffuse_texture;
}

//------------------------------------------------------------------------------

image::csptr material::get_diffuse_texture() const
{
    return m_diffuse_texture;
}

//------------------------------------------------------------------------------

void material::set_ambient(const color::sptr& _ambient)
{
    m_ambient = _ambient;
}

//------------------------------------------------------------------------------

void material::set_diffuse(const color::sptr& _diffuse)
{
    m_diffuse = _diffuse;
}

//------------------------------------------------------------------------------

void material::set_diffuse_texture(const image::sptr& _diffuse_texture)
{
    m_diffuse_texture = _diffuse_texture;
}

//------------------------------------------------------------------------------

bool material::operator==(const material& _other) const noexcept
{
    // If the attributes are different, then it is not equal
    if(m_shading_mode != _other.m_shading_mode
       || m_representation_mode != _other.m_representation_mode
       || m_options_mode != _other.m_options_mode
       || !core::is_equal(m_ambient, _other.m_ambient)
       || !core::is_equal(m_diffuse, _other.m_diffuse)
       || !core::is_equal(m_diffuse_texture, _other.m_diffuse_texture)
       || m_diffuse_texture_filtering != _other.m_diffuse_texture_filtering
       || m_diffuse_texture_wrapping != _other.m_diffuse_texture_wrapping)
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool material::operator!=(const material& _other) const noexcept
{
    return !(*this == _other);
}

} //namespace sight::data

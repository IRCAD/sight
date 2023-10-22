/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

    m_ambient(std::make_shared<color>(0.05F, 0.05F, 0.05F, 1.F)),
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

    m_ambient        = other->m_ambient;
    m_diffuse        = other->m_diffuse;
    m_diffuseTexture = other->m_diffuseTexture;

    m_shadingMode             = other->m_shadingMode;
    m_representationMode      = other->m_representationMode;
    m_optionsMode             = other->m_optionsMode;
    m_diffuseTextureFiltering = other->m_diffuseTextureFiltering;
    m_diffuseTextureWrapping  = other->m_diffuseTextureWrapping;

    base_class::shallow_copy(other);
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

    m_ambient        = data::object::copy(other->m_ambient, _cache);
    m_diffuse        = data::object::copy(other->m_diffuse, _cache);
    m_diffuseTexture = data::object::copy(other->m_diffuseTexture, _cache);

    m_shadingMode             = other->m_shadingMode;
    m_representationMode      = other->m_representationMode;
    m_optionsMode             = other->m_optionsMode;
    m_diffuseTextureFiltering = other->m_diffuseTextureFiltering;
    m_diffuseTextureWrapping  = other->m_diffuseTextureWrapping;

    base_class::deep_copy(other, _cache);
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

image::sptr material::getDiffuseTexture()
{
    return m_diffuseTexture;
}

//------------------------------------------------------------------------------

image::csptr material::getDiffuseTexture() const
{
    return m_diffuseTexture;
}

//------------------------------------------------------------------------------

void material::setAmbient(const color::sptr& _ambient)
{
    m_ambient = _ambient;
}

//------------------------------------------------------------------------------

void material::setDiffuse(const color::sptr& _diffuse)
{
    m_diffuse = _diffuse;
}

//------------------------------------------------------------------------------

void material::setDiffuseTexture(const image::sptr& _diffuse_texture)
{
    m_diffuseTexture = _diffuse_texture;
}

//------------------------------------------------------------------------------

bool material::operator==(const material& _other) const noexcept
{
    // If the attributes are different, then it is not equal
    if(m_shadingMode != _other.m_shadingMode
       || m_representationMode != _other.m_representationMode
       || m_optionsMode != _other.m_optionsMode
       || !core::tools::is_equal(m_ambient, _other.m_ambient)
       || !core::tools::is_equal(m_diffuse, _other.m_diffuse)
       || !core::tools::is_equal(m_diffuseTexture, _other.m_diffuseTexture)
       || m_diffuseTextureFiltering != _other.m_diffuseTextureFiltering
       || m_diffuseTextureWrapping != _other.m_diffuseTextureWrapping)
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool material::operator!=(const material& _other) const noexcept
{
    return !(*this == _other);
}

} //namespace sight::data

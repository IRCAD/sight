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
    new_signal<AddedTextureSignalType>(ADDED_TEXTURE_SIG);
    new_signal<RemovedTextureSignalType>(REMOVED_TEXTURE_SIG);
}

//------------------------------------------------------------------------------

void material::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const material>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
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

void material::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const material>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_ambient        = data::object::copy(other->m_ambient, cache);
    m_diffuse        = data::object::copy(other->m_diffuse, cache);
    m_diffuseTexture = data::object::copy(other->m_diffuseTexture, cache);

    m_shadingMode             = other->m_shadingMode;
    m_representationMode      = other->m_representationMode;
    m_optionsMode             = other->m_optionsMode;
    m_diffuseTextureFiltering = other->m_diffuseTextureFiltering;
    m_diffuseTextureWrapping  = other->m_diffuseTextureWrapping;

    base_class::deep_copy(other, cache);
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

void material::setAmbient(const color::sptr& ambient)
{
    m_ambient = ambient;
}

//------------------------------------------------------------------------------

void material::setDiffuse(const color::sptr& diffuse)
{
    m_diffuse = diffuse;
}

//------------------------------------------------------------------------------

void material::setDiffuseTexture(const image::sptr& diffuseTexture)
{
    m_diffuseTexture = diffuseTexture;
}

//------------------------------------------------------------------------------

bool material::operator==(const material& other) const noexcept
{
    // If the attributes are different, then it is not equal
    if(m_shadingMode != other.m_shadingMode
       || m_representationMode != other.m_representationMode
       || m_optionsMode != other.m_optionsMode
       || !core::tools::is_equal(m_ambient, other.m_ambient)
       || !core::tools::is_equal(m_diffuse, other.m_diffuse)
       || !core::tools::is_equal(m_diffuseTexture, other.m_diffuseTexture)
       || m_diffuseTextureFiltering != other.m_diffuseTextureFiltering
       || m_diffuseTextureWrapping != other.m_diffuseTextureWrapping)
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool material::operator!=(const material& other) const noexcept
{
    return !(*this == other);
}

} //namespace sight::data

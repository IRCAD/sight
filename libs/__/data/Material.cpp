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

#include "data/Material.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::Material);

namespace sight::data
{

//------------------------------------------------------------------------------

const core::com::signals::key_t Material::ADDED_TEXTURE_SIG   = "addedTexture";
const core::com::signals::key_t Material::REMOVED_TEXTURE_SIG = "removedTexture";

//------------------------------------------------------------------------------

Material::Material() :

    m_ambient(std::make_shared<Color>(0.05F, 0.05F, 0.05F, 1.F)),
    m_diffuse(std::make_shared<Color>())
{
    new_signal<AddedTextureSignalType>(ADDED_TEXTURE_SIG);
    new_signal<RemovedTextureSignalType>(REMOVED_TEXTURE_SIG);
}

//------------------------------------------------------------------------------

void Material::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const Material>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
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

void Material::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const Material>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_ambient        = data::Object::copy(other->m_ambient, cache);
    m_diffuse        = data::Object::copy(other->m_diffuse, cache);
    m_diffuseTexture = data::Object::copy(other->m_diffuseTexture, cache);

    m_shadingMode             = other->m_shadingMode;
    m_representationMode      = other->m_representationMode;
    m_optionsMode             = other->m_optionsMode;
    m_diffuseTextureFiltering = other->m_diffuseTextureFiltering;
    m_diffuseTextureWrapping  = other->m_diffuseTextureWrapping;

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

Color::sptr Material::ambient() const
{
    return m_ambient;
}

//------------------------------------------------------------------------------

Color::sptr Material::diffuse() const
{
    return m_diffuse;
}

//------------------------------------------------------------------------------

Image::sptr Material::getDiffuseTexture()
{
    return m_diffuseTexture;
}

//------------------------------------------------------------------------------

Image::csptr Material::getDiffuseTexture() const
{
    return m_diffuseTexture;
}

//------------------------------------------------------------------------------

void Material::setAmbient(const Color::sptr& ambient)
{
    m_ambient = ambient;
}

//------------------------------------------------------------------------------

void Material::setDiffuse(const Color::sptr& diffuse)
{
    m_diffuse = diffuse;
}

//------------------------------------------------------------------------------

void Material::setDiffuseTexture(const Image::sptr& diffuseTexture)
{
    m_diffuseTexture = diffuseTexture;
}

//------------------------------------------------------------------------------

bool Material::operator==(const Material& other) const noexcept
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

bool Material::operator!=(const Material& other) const noexcept
{
    return !(*this == other);
}

} //namespace sight::data

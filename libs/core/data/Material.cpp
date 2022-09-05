/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA(sight::data::Material);

namespace sight::data
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType Material::s_ADDED_TEXTURE_SIG   = "addedTexture";
const core::com::Signals::SignalKeyType Material::s_REMOVED_TEXTURE_SIG = "removedTexture";

//------------------------------------------------------------------------------

Material::Material(data::Object::Key /*unused*/) :

    m_ambient(Color::New(0.05F, 0.05F, 0.05F, 1.F)),
    m_diffuse(Color::New())
{
    newSignal<AddedTextureSignalType>(s_ADDED_TEXTURE_SIG);
    newSignal<RemovedTextureSignalType>(s_REMOVED_TEXTURE_SIG);
}

//------------------------------------------------------------------------------

Material::~Material()
= default;

//------------------------------------------------------------------------------

void Material::shallowCopy(const Object::csptr& _source)
{
    Material::csptr other = Material::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);

    m_ambient        = other->m_ambient;
    m_diffuse        = other->m_diffuse;
    m_diffuseTexture = other->m_diffuseTexture;

    m_shadingMode             = other->m_shadingMode;
    m_representationMode      = other->m_representationMode;
    m_optionsMode             = other->m_optionsMode;
    m_diffuseTextureFiltering = other->m_diffuseTextureFiltering;
    m_diffuseTextureWrapping  = other->m_diffuseTextureWrapping;
}

//------------------------------------------------------------------------------

void Material::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Material::csptr other = Material::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);

    m_ambient        = data::Object::copy(other->m_ambient, cache);
    m_diffuse        = data::Object::copy(other->m_diffuse, cache);
    m_diffuseTexture = data::Object::copy(other->m_diffuseTexture, cache);

    m_shadingMode             = other->m_shadingMode;
    m_representationMode      = other->m_representationMode;
    m_optionsMode             = other->m_optionsMode;
    m_diffuseTextureFiltering = other->m_diffuseTextureFiltering;
    m_diffuseTextureWrapping  = other->m_diffuseTextureWrapping;
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
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool Material::operator!=(const Material& other) const noexcept
{
    return !(*this == other);
}

} //namespace sight::data

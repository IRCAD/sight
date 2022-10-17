/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "viz/scene3d/Texture.hpp"

#include <viz/scene3d/detail/TextureManager.hpp>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

Texture::Texture(const data::Image::csptr& _image, const std::string& suffixId)
{
    m_resource = detail::TextureManager::get()->instantiate(_image, suffixId);
}

//-----------------------------------------------------------------------------

Texture::~Texture()
{
    if(m_resource)
    {
        detail::TextureManager::get()->release(m_resource);
    }
}

//-----------------------------------------------------------------------------

void Texture::update()
{
    m_window = viz::scene3d::detail::TextureManager::get()->load(m_resource).second;
}

//------------------------------------------------------------------------------

void Texture::bind(
    Ogre::TextureUnitState* _texUnit,
    Ogre::TextureType _type,
    Ogre::TextureFilterOptions _filterType,
    Ogre::TextureAddressingMode _addressMode
)
{
    SIGHT_ASSERT("The texture unit is null.", _texUnit);

    _texUnit->setTextureName(m_resource->getName(), _type);
    _texUnit->setTextureFiltering(_filterType);
    _texUnit->setTextureAddressingMode(_addressMode);
}

//------------------------------------------------------------------------------

void Texture::bind(
    Ogre::Pass* _pass,
    const std::string& _samplerName,
    std::optional<Ogre::TextureFilterOptions> _filterType,
    std::optional<Ogre::TextureAddressingMode> _addressMode
)
{
    SIGHT_ASSERT("The pass is null.", _pass);

    Ogre::TextureUnitState* texUnit = _pass->getTextureUnitState(_samplerName);
    SIGHT_ASSERT("The sampler '" + _samplerName + "' cannot be retrieved.", texUnit);

    if(_filterType != std::nullopt)
    {
        texUnit->setTextureFiltering(_filterType.value());
    }

    if(_addressMode != std::nullopt)
    {
        texUnit->setTextureAddressingMode(_addressMode.value());
    }

    if(m_resource && texUnit->getTextureName() != m_resource->getName())
    {
        texUnit->setTexture(m_resource);
        texUnit->setTextureName(m_resource->getName(), m_resource->getTextureType());
    }

    auto fpParams = _pass->getFragmentProgramParameters();
    if(fpParams && (fpParams->_findNamedConstantDefinition("u_window") != nullptr))
    {
        fpParams->setNamedConstant("u_window", this->window());
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d

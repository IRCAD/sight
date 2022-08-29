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
    viz::scene3d::detail::TextureManager::get()->load(m_resource);
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

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d

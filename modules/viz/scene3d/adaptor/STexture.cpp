/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/STexture.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/tools/Type.hpp>

#include <data/Image.hpp>
#include <data/Material.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::Signals::SignalKeyType module::viz::scene3d::adaptor::STexture::s_TEXTURE_SWAPPED_SIG =
    "textureSwapped";

const std::string DEFAULT_TEXTURE_FILENAME = "default.png";

const service::key_t STexture::s_TEXTURE_INOUT = "image";

static const std::string s_TEXTURE_NAME_CONFIG = "textureName";
static const std::string s_FILTERING_CONFIG    = "filtering";
static const std::string s_WRAPPING_CONFIG     = "wrapping";
static const std::string s_USE_ALPHA_CONFIG    = "useAlpha";
static const std::string s_DYNAMIC_CONFIG      = "dynamic";

//------------------------------------------------------------------------------

STexture::STexture() noexcept
{
    m_sigTextureSwapped = newSignal<TextureSwappedSignalType>(s_TEXTURE_SWAPPED_SIG);
}

//------------------------------------------------------------------------------

STexture::~STexture() noexcept
{
}

//------------------------------------------------------------------------------

void STexture::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    // Choose a default name if not provided, this is very important otherwise
    // the texture may be lost if it is unloaded (which is very likely to happen when playing with techniques)
    m_textureName = config.get<std::string>(s_TEXTURE_NAME_CONFIG, this->getID());

    m_filtering = config.get<std::string>(s_FILTERING_CONFIG, m_filtering);
    m_wrapping  = config.get<std::string>(s_WRAPPING_CONFIG, m_wrapping);
    m_useAlpha  = config.get<bool>(s_USE_ALPHA_CONFIG, m_useAlpha);
    m_isDynamic = config.get<bool>(s_DYNAMIC_CONFIG, m_isDynamic);
}

//------------------------------------------------------------------------------

void STexture::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    m_texture =
        ::Ogre::dynamic_pointer_cast< ::Ogre::Texture>(
            ::Ogre::TextureManager::getSingleton().createOrRetrieve(
                m_textureName,
                sight::viz::scene3d::RESOURCE_GROUP,
                true
            ).first
        );

    this->updating();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap STexture::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_TEXTURE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TEXTURE_INOUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void STexture::updating()
{
    // Retrieves associated Sight image
    const auto image = m_image.lock();

    if(image->getAllocatedSizeInBytes() != 0)
    {
        // Loads the new image
        this->getRenderService()->makeCurrent();
        sight::viz::scene3d::Utils::loadOgreTexture(image.get_shared(), m_texture, ::Ogre::TEX_TYPE_2D, m_isDynamic);

        m_sigTextureSwapped->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STexture::stopping()
{
    this->getRenderService()->makeCurrent();

    // This is necessary, otherwise we have "ghost" textures later when we reload a new texture
    ::Ogre::TextureManager::getSingleton().remove(m_textureName, sight::viz::scene3d::RESOURCE_GROUP);
    m_texture->freeInternalResources();
    m_texture.reset();
}

//------------------------------------------------------------------------------

bool STexture::isValid() const
{
    if(m_texture)
    {
        if(m_texture->getFormat() != ::Ogre::PF_UNKNOWN)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/texture.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/type.hpp>

#include <data/image.hpp>
#include <data/material.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::signals::key_t texture::TEXTURE_SWAPPED_SIG = "textureSwapped";

//------------------------------------------------------------------------------

texture::texture() noexcept
{
    m_sigTextureSwapped = new_signal<TextureSwappedSignalType>(TEXTURE_SWAPPED_SIG);
}

//------------------------------------------------------------------------------

void texture::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_TEXTURE_NAME_CONFIG = s_CONFIG + "textureName";
    static const std::string s_FILTERING_CONFIG    = s_CONFIG + "filtering";
    static const std::string s_WRAPPING_CONFIG     = s_CONFIG + "wrapping";
    static const std::string s_USE_ALPHA_CONFIG    = s_CONFIG + "useAlpha";
    static const std::string s_DYNAMIC_CONFIG      = s_CONFIG + "dynamic";

    // Choose a default name if not provided, this is very important otherwise
    // the texture may be lost if it is unloaded (which is very likely to happen when playing with techniques)
    m_textureName = config.get<std::string>(s_TEXTURE_NAME_CONFIG, this->get_id());

    m_filtering = config.get<std::string>(s_FILTERING_CONFIG, m_filtering);
    m_wrapping  = config.get<std::string>(s_WRAPPING_CONFIG, m_wrapping);
    m_useAlpha  = config.get<bool>(s_USE_ALPHA_CONFIG, m_useAlpha);
    m_isDynamic = config.get<bool>(s_DYNAMIC_CONFIG, m_isDynamic);
}

//------------------------------------------------------------------------------

void texture::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    // Retrieves associated Sight image
    {
        const auto image = m_image.lock();
        m_texture = std::make_shared<sight::viz::scene3d::Texture>(image.get_shared());
    }

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t texture::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_TEXTURE_INOUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_TEXTURE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void texture::updating()
{
    // Retrieves associated Sight image
    const auto image = m_image.lock();

    if(image->getAllocatedSizeInBytes() != 0)
    {
        // Loads the new image
        this->getRenderService()->makeCurrent();
        m_texture->update();

        m_sigTextureSwapped->async_emit();
    }
}

//------------------------------------------------------------------------------

void texture::stopping()
{
    this->getRenderService()->makeCurrent();
    m_texture.reset();
}

//------------------------------------------------------------------------------

bool texture::isValid() const
{
    if(m_texture)
    {
        if(m_texture->get()->getFormat() != Ogre::PF_UNKNOWN)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

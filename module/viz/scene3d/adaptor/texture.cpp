/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "module/viz/scene3d/adaptor/texture.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/type.hpp>

#include <data/image.hpp>
#include <data/material.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::signals::key_t texture::TEXTURE_SWAPPED_SIG = "texture_swapped";

//------------------------------------------------------------------------------

texture::texture() noexcept
{
    m_sig_texture_swapped = new_signal<texture_swapped_signal_t>(TEXTURE_SWAPPED_SIG);
}

//------------------------------------------------------------------------------

void texture::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_TEXTURE_NAME_CONFIG = CONFIG + "textureName";
    static const std::string s_FILTERING_CONFIG    = CONFIG + "filtering";
    static const std::string s_WRAPPING_CONFIG     = CONFIG + "wrapping";
    static const std::string s_USE_ALPHA_CONFIG    = CONFIG + "useAlpha";
    static const std::string s_DYNAMIC_CONFIG      = CONFIG + "dynamic";

    // Choose a default name if not provided, this is very important otherwise
    // the texture may be lost if it is unloaded (which is very likely to happen when playing with techniques)
    m_texture_name = config.get<std::string>(s_TEXTURE_NAME_CONFIG, this->get_id());

    m_filtering  = config.get<std::string>(s_FILTERING_CONFIG, m_filtering);
    m_wrapping   = config.get<std::string>(s_WRAPPING_CONFIG, m_wrapping);
    m_use_alpha  = config.get<bool>(s_USE_ALPHA_CONFIG, m_use_alpha);
    m_is_dynamic = config.get<bool>(s_DYNAMIC_CONFIG, m_is_dynamic);
}

//------------------------------------------------------------------------------

void texture::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    // Retrieves associated Sight image
    {
        const auto image = m_image.lock();
        m_texture = std::make_shared<sight::viz::scene3d::texture>(image.get_shared());
    }

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t texture::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(TEXTURE_INOUT, data::image::BUFFER_MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(TEXTURE_INOUT, data::image::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void texture::updating()
{
    // Retrieves associated Sight image
    const auto image = m_image.lock();

    if(image->allocated_size_in_bytes() != 0)
    {
        // Loads the new image
        this->render_service()->make_current();
        m_texture->update();

        m_sig_texture_swapped->async_emit();
    }

    this->update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void texture::stopping()
{
    this->render_service()->make_current();
    m_texture.reset();

    adaptor::deinit();
}

//------------------------------------------------------------------------------

bool texture::is_valid() const
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

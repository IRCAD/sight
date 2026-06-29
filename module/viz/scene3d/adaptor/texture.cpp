/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include <data/image.hpp>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

texture::texture() noexcept
{
    new_signal<signals::texture_swapped_t>(signals::TEXTURE_SWAPPED);
}

//------------------------------------------------------------------------------

void texture::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_TEXTURE_NAME_CONFIG = CONFIG + "texture_name";
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
    connections.push(TEXTURE_INOUT, data::image::signals::BUFFER_MODIFIED, adaptor::slots::LAZY_UPDATE);
    connections.push(TEXTURE_INOUT, data::signals::MODIFIED, adaptor::slots::LAZY_UPDATE);
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

        this->async_emit(signals::TEXTURE_SWAPPED);
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

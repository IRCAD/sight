/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "stereo_toggler.hpp"

#include <viz/scene3d/compositor/core.hpp>
#include <viz/scene3d/render.hpp>

namespace sight::module::ui::viz
{

static const core::com::signals::key_t STEREO_ACTIVE_SIG = "stereoActive";

//------------------------------------------------------------------------------

stereo_toggler::stereo_toggler() :
    m_stereo_active_sig(new_signal<stereo_active_sig_t>(STEREO_ACTIVE_SIG))
{
}

//------------------------------------------------------------------------------

stereo_toggler::~stereo_toggler()
= default;

//------------------------------------------------------------------------------

void stereo_toggler::configuring()
{
    this->initialize();

    const config_t config = this->get_config().get_child("config.<xmlattr>");

    m_layer_id = config.get<std::string>("layer");
    SIGHT_ASSERT("Empty layer ID.", !m_layer_id.empty());

    const std::string stereo_mode = config.get<std::string>("stereoMode", "");

    if(stereo_mode == "interlaced")
    {
        m_stereo_mode = stereo_mode_t::stereo;
    }
    else if(stereo_mode == "AutoStereo5")
    {
        m_stereo_mode = stereo_mode_t::autostereo_5;
    }
    else if(stereo_mode == "AutoStereo8")
    {
        m_stereo_mode = stereo_mode_t::autostereo_8;
    }
    else
    {
        SIGHT_ERROR("Unknown stereo mode: '" + stereo_mode + "'. stereo_toggler will do nothing.");
    }
}

//------------------------------------------------------------------------------

void stereo_toggler::starting()
{
    this->action_service_starting();
}

//------------------------------------------------------------------------------

void stereo_toggler::updating()
{
    if(this->confirm_action())
    {
        const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

        const bool enable_stereo = this->checked() && this->enabled();
        const auto stereo_mode   = enable_stereo ? m_stereo_mode : stereo_mode_t::none;

        for(const auto& srv : renderers)
        {
            auto render_srv = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);
            auto layer_map  = render_srv->get_layers();

            auto layer_it = layer_map.find(m_layer_id);

            if(layer_it != layer_map.end())
            {
                auto& layer = layer_it->second;
                layer->set_stereo_mode(stereo_mode);
                layer->request_render();
            }
            else
            {
                SIGHT_WARN("No layer named '" + m_layer_id + "' in render service '" + render_srv->get_id() + "'.");
            }
        }

        m_stereo_active_sig->async_emit(enable_stereo);
    }
}

//------------------------------------------------------------------------------

void stereo_toggler::stopping()
{
    this->action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz

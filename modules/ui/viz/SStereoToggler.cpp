/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "SStereoToggler.hpp"

#include <viz/scene3d/compositor/Core.hpp>
#include <viz/scene3d/SRender.hpp>

namespace sight::module::ui::viz
{

static const core::com::Signals::SignalKeyType s_STEREO_ACTIVE_SIG = "stereoActive";

//------------------------------------------------------------------------------

SStereoToggler::SStereoToggler()
{
    m_stereoActiveSig = newSignal<StereoActiveSigType>(s_STEREO_ACTIVE_SIG);
}

//------------------------------------------------------------------------------

SStereoToggler::~SStereoToggler()
{
}

//------------------------------------------------------------------------------

void SStereoToggler::configuring()
{
    this->initialize();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_layerId = config.get<std::string>("layer");
    SIGHT_ASSERT("Empty layer ID.", !m_layerId.empty());

    const std::string stereoMode = config.get<std::string>("stereoMode", "");

    if(stereoMode == "interlaced")
    {
        m_stereoMode = StereoModeType::STEREO;
    }
    else if(stereoMode == "AutoStereo5")
    {
        m_stereoMode = StereoModeType::AUTOSTEREO_5;
    }
    else if(stereoMode == "AutoStereo8")
    {
        m_stereoMode = StereoModeType::AUTOSTEREO_8;
    }
    else
    {
        SIGHT_ERROR("Unknown stereo mode: '" + stereoMode + "'. SStereoToggler will do nothing.");
    }
}

//------------------------------------------------------------------------------

void SStereoToggler::starting()
{
    this->actionServiceStarting();
}

//------------------------------------------------------------------------------

void SStereoToggler::updating()
{
    if(this->confirmAction())
    {
        service::registry::ObjectService::ServiceVectorType renderers =
            service::OSR::getServices("::sight::viz::scene3d::SRender");

        const bool enableStereo = this->getIsActive() && this->getIsExecutable();
        const auto stereoMode   = enableStereo ? m_stereoMode : StereoModeType::NONE;

        for(auto srv : renderers)
        {
            auto renderSrv = sight::viz::scene3d::SRender::dynamicCast(srv);
            auto layerMap  = renderSrv->getLayers();

            auto layerIt = layerMap.find(m_layerId);

            if(layerIt != layerMap.end())
            {
                auto& layer = layerIt->second;
                layer->setStereoMode(stereoMode);
                layer->requestRender();
            }
            else
            {
                SIGHT_WARN("No layer named '" + m_layerId + "' in render service '" + renderSrv->getID() + "'.");
            }
        }

        m_stereoActiveSig->asyncEmit(enableStereo);
    }
}

//------------------------------------------------------------------------------

void SStereoToggler::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

} // uiVisuOgre

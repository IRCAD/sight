/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SStereoToggler.hpp"

#include <fwRenderOgre/compositor/Core.hpp>
#include <fwRenderOgre/SRender.hpp>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiVisuOgre::SStereoToggler);

//------------------------------------------------------------------------------

SStereoToggler::SStereoToggler()
{

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
    SLM_ASSERT("Empty layer ID.", !m_layerId.empty());

    const std::string stereoMode = config.get<std::string>("stereoMode", "");

    if(stereoMode == "interlaced")
    {
        m_stereoMode = StereoModeType::STEREO;
    }
    else if (stereoMode == "AutoStereo5")
    {
        m_stereoMode = StereoModeType::AUTOSTEREO_5;
    }
    else if (stereoMode == "AutoStereo5")
    {
        m_stereoMode = StereoModeType::AUTOSTEREO_8;
    }
    else
    {
        SLM_ERROR("Unknown stereo mode: '" + stereoMode + "'. SStereoToggler will do nothing.");
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
        ::fwServices::registry::ObjectService::ServiceVectorType renderers =
            ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

        for(auto srv : renderers)
        {
            ::fwRenderOgre::SRender::sptr renderSrv        = ::fwRenderOgre::SRender::dynamicCast(srv);
            ::fwRenderOgre::SRender::LayerMapType layerMap = renderSrv->getLayers();

            auto layerIt = layerMap.find(m_layerId);

            if(layerIt != layerMap.end())
            {
                const bool enableStereo = this->getIsActive() && this->getIsExecutable();
                const auto stereoMode   = enableStereo ? m_stereoMode : StereoModeType::NONE;

                auto& layer = layerIt->second;
                layer->setStereoMode(stereoMode);
                layer->requestRender();
            }
            else
            {
                SLM_WARN("No layer named '" + m_layerId + "' in render service '" + renderSrv->getID() + "'.");
            }
        }
    }
}

//------------------------------------------------------------------------------

void SStereoToggler::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

} // uiVisuOgre

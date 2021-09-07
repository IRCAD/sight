/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SRenderStats.hpp"

#include <service/macros.hpp>

#include <viz/scene3d/helper/Font.hpp>
#include <viz/scene3d/IWindowInteractor.hpp>
#include <viz/scene3d/SRender.hpp>

#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTargetListener.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>

#include <sstream>

namespace sight::module::viz::scene3d::adaptor
{

class PostWindowRenderListener : public ::Ogre::RenderTargetListener
{
public:

    PostWindowRenderListener(SRenderStats& _renderStatsAdaptor) :
        m_renderStatsAdaptor(_renderStatsAdaptor)
    {
    }

    //------------------------------------------------------------------------------

    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override
    {
        // update text every 100th frame
        if(m_frameCount++ > 100)
        {
            auto frameStats = evt.source->getStatistics();

            std::ostringstream statStream;
            statStream << "FPS=" << static_cast<int>(frameStats.lastFPS) << std::endl
            << "Triangle count=" << frameStats.triangleCount << std::endl;

            m_renderStatsAdaptor.m_statsText->setText(statStream.str());

            m_frameCount = 1;
        }
    }

private:

    SRenderStats& m_renderStatsAdaptor;

    std::uint8_t m_frameCount {100}; // Start at 100 to trigger text creation after the first frame.
};

static const std::string s_COLOR_CONFIG       = "color";
static const std::string s_FONT_SIZE_CONFIG   = "fontSize";
static const std::string s_FONT_SOURCE_CONFIG = "fontSource";

//------------------------------------------------------------------------------

SRenderStats::SRenderStats() noexcept
{
}

//------------------------------------------------------------------------------

SRenderStats::~SRenderStats() noexcept
{
}

//------------------------------------------------------------------------------

void SRenderStats::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const std::string color      = config.get<std::string>(s_COLOR_CONFIG, "#FFFFFF");
    data::Color::sptr sightColor = data::Color::New();
    sightColor->setRGBA(color);

    m_textColor = ::Ogre::ColourValue(sightColor->red(), sightColor->green(), sightColor->blue());

    m_fontSource = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize   = config.get<size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
}

//------------------------------------------------------------------------------

void SRenderStats::starting()
{
    this->initialize();

    sight::viz::scene3d::SRender::sptr renderSrv = this->getRenderService();
    renderSrv->makeCurrent();

    const float dpi = renderSrv->getInteractorManager()->getLogicalDotsPerInch();

    ::Ogre::OverlayContainer* textContainer = this->getLayer()->getOverlayTextPanel();

    m_statsText = sight::viz::scene3d::Text::New(
        this->getID() + "_fpsText",
        this->getSceneManager(),
        textContainer,
        m_fontSource,
        m_fontSize,
        dpi,
        this->getLayer()->getDefaultCamera()
    );

    m_statsText->setPosition(0.01f, 0.01f);
    m_statsText->setTextColor(m_textColor);

    auto* renderWindow = renderSrv->getInteractorManager()->getRenderTarget();

    m_listener = std::make_unique<module::viz::scene3d::adaptor::PostWindowRenderListener>(*this);
    renderWindow->addListener(m_listener.get());
}

//------------------------------------------------------------------------------

void SRenderStats::updating()
{
}

//------------------------------------------------------------------------------

void SRenderStats::stopping()
{
    sight::viz::scene3d::SRender::sptr renderSrv = this->getRenderService();
    renderSrv->makeCurrent();

    auto* renderWindow = renderSrv->getInteractorManager()->getRenderTarget();
    renderWindow->removeListener(m_listener.get());

    m_listener.reset();

    ::Ogre::SceneManager* sm = this->getLayer()->getSceneManager();
    m_statsText->detachFromParent();
    sm->destroyMovableObject(m_statsText);
    m_statsText = nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
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

#include "visuOgreAdaptor/SRenderStats.hpp"

#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>

#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTargetListener.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>

#include <sstream>

namespace visuOgreAdaptor
{

class PostWindowRenderListener : public ::Ogre::RenderTargetListener
{
public:

    PostWindowRenderListener(SRenderStats& _renderStatsAdaptor) :
        m_renderStatsAdaptor(_renderStatsAdaptor)
    {

    }

    //------------------------------------------------------------------------------

    virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override
    {
        // update text every 100th frame
        if(m_frameCount++ > 100)
        {
            auto frameStats = evt.source->getStatistics();

            std::ostringstream statStream;
            statStream << "FPS=" << static_cast<int>(frameStats.lastFPS) << std::endl
                       << "Triangle count=" << frameStats.triangleCount  << std::endl;

            m_renderStatsAdaptor.m_statsText->setText(statStream.str());

            m_frameCount = 1;
        }
    }

private:

    SRenderStats& m_renderStatsAdaptor;

    std::uint8_t m_frameCount {100}; // Start at 100 to trigger text creation after the first frame.

};

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SRenderStats);

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

    auto configTree = this->getConfigTree();
    auto config     = configTree.get_child("config.<xmlattr>");

    std::string color = config.get<std::string>("color", "#ffffff");

    ::fwData::Color::sptr sightColor = ::fwData::Color::New();
    sightColor->setRGBA(color);

    m_textColor = ::Ogre::ColourValue(sightColor->red(), sightColor->green(), sightColor->blue());

    m_textHeight = config.get<float>("height", 0.03f);
}

//------------------------------------------------------------------------------

void SRenderStats::starting()
{
    this->initialize();

    ::fwRenderOgre::SRender::sptr renderSrv = this->getRenderService();
    renderSrv->makeCurrent();

    ::Ogre::OverlayContainer* textContainer = this->getLayer()->getOverlayTextPanel();
    ::Ogre::FontPtr dejaVuSansFont          = ::fwRenderOgre::helper::Font::getFont("DejaVuSans.ttf", 32);

    m_statsText = ::fwRenderOgre::Text::New(this->getID() + "_fpsText",
                                            this->getSceneManager(),
                                            textContainer,
                                            dejaVuSansFont,
                                            nullptr);

    m_statsText->setPosition(0, 0);
    m_statsText->setTextColor(m_textColor);
    m_statsText->setCharHeight(m_textHeight);

    auto* renderWindow = renderSrv->getInteractorManager()->getRenderTarget();

    m_listener = ::boost::make_unique<visuOgreAdaptor::PostWindowRenderListener>(*this);
    renderWindow->addListener(m_listener.get());
}

//------------------------------------------------------------------------------

void SRenderStats::updating()
{

}

//------------------------------------------------------------------------------

void SRenderStats::stopping()
{
    ::fwRenderOgre::SRender::sptr renderSrv = this->getRenderService();
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

} // namespace visuOgreAdaptor

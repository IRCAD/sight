/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/render_stats.hpp"

#include <viz/scene3d/render.hpp>
#include <viz/scene3d/window_interactor.hpp>

#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTargetListener.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>

#include <sstream>

namespace sight::module::viz::scene3d::adaptor
{

class PostWindowRenderListener : public Ogre::RenderTargetListener
{
public:

    explicit PostWindowRenderListener(render_stats& _render_stats_adaptor) :
        m_renderStatsAdaptor(_render_stats_adaptor)
    {
    }

    //------------------------------------------------------------------------------

    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& _evt) override
    {
        // update text every 100th frame
        if(m_frameCount++ > 100)
        {
            auto frame_stats = _evt.source->getStatistics();

            std::ostringstream stat_stream;
            stat_stream << "FPS=" << static_cast<int>(frame_stats.lastFPS) << std::endl
            << "Triangle count=" << frame_stats.triangleCount << std::endl;

            m_renderStatsAdaptor.m_statsText->setText(stat_stream.str());

            m_frameCount = 1;
        }
    }

private:

    render_stats& m_renderStatsAdaptor;

    std::uint8_t m_frameCount {100}; // Start at 100 to trigger text creation after the first frame.
};

//------------------------------------------------------------------------------

render_stats::render_stats() noexcept  = default;
render_stats::~render_stats() noexcept = default;

//------------------------------------------------------------------------------

void render_stats::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_COLOR_CONFIG     = s_CONFIG + "color";
    static const std::string s_FONT_SIZE_CONFIG = s_CONFIG + "fontSize";

    const std::string color       = config.get<std::string>(s_COLOR_CONFIG, "#FFFFFF");
    data::color::sptr sight_color = std::make_shared<data::color>();
    sight_color->setRGBA(color);

    m_textColor = Ogre::ColourValue(sight_color->red(), sight_color->green(), sight_color->blue());

    m_fontSize = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
}

//------------------------------------------------------------------------------

void render_stats::starting()
{
    this->initialize();

    sight::viz::scene3d::render::sptr render_srv = this->getRenderService();
    render_srv->makeCurrent();

    m_statsText = sight::viz::scene3d::text::make(this->getLayer());
    m_statsText->setFontSize(m_fontSize);
    m_statsText->setPosition(0.01F, 0.01F);
    m_statsText->setTextColor(m_textColor);

    const sight::viz::scene3d::layer::sptr layer = this->getLayer();
    auto* render_window                          = layer->getRenderTarget();

    m_listener = std::make_unique<module::viz::scene3d::adaptor::PostWindowRenderListener>(*this);
    render_window->addListener(m_listener.get());
}

//------------------------------------------------------------------------------

void render_stats::updating()
{
}

//------------------------------------------------------------------------------

void render_stats::stopping()
{
    sight::viz::scene3d::render::sptr render_srv = this->getRenderService();
    render_srv->makeCurrent();

    const sight::viz::scene3d::layer::sptr layer = this->getLayer();
    auto* render_window                          = layer->getRenderTarget();
    render_window->removeListener(m_listener.get());

    m_listener.reset();
    m_statsText = nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

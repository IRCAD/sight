/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include "module/viz/scene3d/adaptor/render_stats.hpp"

#include <viz/scene3d/render.hpp>
#include <viz/scene3d/window_interactor.hpp>

#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTargetListener.h>

#include <sstream>

namespace sight::module::viz::scene3d::adaptor
{

class PostWindowRenderListener : public Ogre::RenderTargetListener
{
public:

    explicit PostWindowRenderListener(render_stats& _render_stats_adaptor) :
        m_render_stats_adaptor(_render_stats_adaptor)
    {
    }

    //------------------------------------------------------------------------------

    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& _evt) override
    {
        // update text every 100th frame
        if(m_frame_count++ > 100)
        {
            auto frame_stats = _evt.source->getStatistics();

            std::ostringstream stat_stream;
            stat_stream << "FPS=" << static_cast<int>(frame_stats.lastFPS) << std::endl
            << "Triangle count=" << frame_stats.triangleCount << std::endl;

            m_render_stats_adaptor.m_stats_text->set_text(stat_stream.str());

            m_frame_count = 1;
        }
    }

private:

    render_stats& m_render_stats_adaptor;

    std::uint8_t m_frame_count {100}; // Start at 100 to trigger text creation after the first frame.
};

//------------------------------------------------------------------------------

render_stats::render_stats() noexcept  = default;
render_stats::~render_stats() noexcept = default;

//------------------------------------------------------------------------------

void render_stats::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_COLOR_CONFIG     = CONFIG + "color";
    static const std::string s_FONT_SIZE_CONFIG = CONFIG + "fontSize";

    const std::string color       = config.get<std::string>(s_COLOR_CONFIG, "#FFFFFF");
    data::color::sptr sight_color = std::make_shared<data::color>();
    sight_color->from_string(color);

    m_text_color = Ogre::ColourValue(sight_color->red(), sight_color->green(), sight_color->blue());

    m_font_size = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
}

//------------------------------------------------------------------------------

void render_stats::starting()
{
    adaptor::init();

    sight::viz::scene3d::render::sptr render_srv = this->render_service();
    render_srv->make_current();

    m_stats_text = sight::viz::scene3d::text::make(this->layer());
    m_stats_text->set_font_size(m_font_size);
    m_stats_text->set_position(0.01F, 0.01F);
    m_stats_text->set_text_color(m_text_color);

    const sight::viz::scene3d::layer::sptr layer = this->layer();
    auto* render_window                          = layer->get_render_target();

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
    sight::viz::scene3d::render::sptr render_srv = this->render_service();
    render_srv->make_current();

    const sight::viz::scene3d::layer::sptr layer = this->layer();
    auto* render_window                          = layer->get_render_target();
    render_window->removeListener(m_listener.get());

    m_listener.reset();
    m_stats_text = nullptr;

    adaptor::deinit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

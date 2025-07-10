/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "module/viz/scene3d/adaptor/resize_viewport.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/layer.hpp>

#include <OGRE/OgreCamera.h>

#include <tuple>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t REVERT_SLOT = "revert";
static const core::com::slots::key_t RESIZE_SLOT = "resize";

//------------------------------------------------------------------------------

resize_viewport::resize_viewport() noexcept
{
    new_slot(REVERT_SLOT, &resize_viewport::revert, this);
    new_slot(RESIZE_SLOT, &resize_viewport::resize, this);
}

//------------------------------------------------------------------------------

void resize_viewport::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_WIDTH_CONFIG    = CONFIG + "width";
    static const std::string s_HEIGHT_CONFIG   = CONFIG + "height";
    static const std::string s_H_OFFSET_CONFIG = CONFIG + "hOffset";
    static const std::string s_V_OFFSET_CONFIG = CONFIG + "vOffset";
    static const std::string s_H_ALIGN_CONFIG  = CONFIG + "hAlign";
    static const std::string s_V_ALIGN_CONFIG  = CONFIG + "vAlign";

    float x_pos = config.get<float>(s_H_OFFSET_CONFIG, 0.F);
    float y_pos = config.get<float>(s_V_OFFSET_CONFIG, 0.F);

    const float width  = config.get<float>(s_WIDTH_CONFIG, 1.F);
    const float height = config.get<float>(s_HEIGHT_CONFIG, 1.F);

    const std::map<std::string, float> horiz_align_to_x {
        {"left", x_pos},
        {"center", 0.5F - width * 0.5F + x_pos},
        {"right", 1.F - width - x_pos}
    };

    const std::map<std::string, float> vert_align_to_y {
        {"bottom", 1.F - height - y_pos},
        {"center", 0.5F - height * 0.5F + y_pos},
        {"top", y_pos}
    };

    const std::string h_align = config.get(s_H_ALIGN_CONFIG, "left");
    const std::string v_align = config.get(s_V_ALIGN_CONFIG, "top");

    x_pos = horiz_align_to_x.at(h_align);
    y_pos = vert_align_to_y.at(v_align);

    m_new_viewport_dimensions = std::tie(x_pos, y_pos, width, height);
}

//------------------------------------------------------------------------------

void resize_viewport::starting()
{
    adaptor::init();
    const auto* const camera = this->layer()->get_default_camera();
    const auto* const vp     = camera->getViewport();

    m_previous_viewport_dimensions = std::make_tuple(vp->getLeft(), vp->getTop(), vp->getWidth(), vp->getHeight());
}

//------------------------------------------------------------------------------

void resize_viewport::updating() noexcept
{
}

//------------------------------------------------------------------------------

void resize_viewport::stopping() noexcept
{
    adaptor::deinit();
}

//------------------------------------------------------------------------------

void resize_viewport::resize(bool _resize = true)
{
    auto* const camera = this->layer()->get_default_camera();
    auto* const vp     = camera->getViewport();

    this->render_service()->make_current();

    const auto& new_dimensions = _resize ? m_new_viewport_dimensions : m_previous_viewport_dimensions;
    const auto& [left, top, width, height] = new_dimensions;
    vp->setDimensions(left, top, width, height);

    this->request_render();
}

//------------------------------------------------------------------------------

void resize_viewport::revert()
{
    this->resize(false);
}

} // namespace sight::module::viz::scene3d::adaptor.

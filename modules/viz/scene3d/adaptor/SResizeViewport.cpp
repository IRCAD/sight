/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SResizeViewport.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/Layer.hpp>

#include <OGRE/OgreCamera.h>

#include <tuple>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t REVERT_SLOT = "revert";
static const core::com::slots::key_t RESIZE_SLOT = "resize";

//------------------------------------------------------------------------------

SResizeViewport::SResizeViewport() noexcept
{
    new_slot(REVERT_SLOT, &SResizeViewport::revert, this);
    new_slot(RESIZE_SLOT, &SResizeViewport::resize, this);
}

//------------------------------------------------------------------------------

void SResizeViewport::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    static const std::string s_WIDTH_CONFIG    = s_CONFIG + "width";
    static const std::string s_HEIGHT_CONFIG   = s_CONFIG + "height";
    static const std::string s_H_OFFSET_CONFIG = s_CONFIG + "hOffset";
    static const std::string s_V_OFFSET_CONFIG = s_CONFIG + "vOffset";
    static const std::string s_H_ALIGN_CONFIG  = s_CONFIG + "hAlign";
    static const std::string s_V_ALIGN_CONFIG  = s_CONFIG + "vAlign";

    float xPos = config.get<float>(s_H_OFFSET_CONFIG, 0.F);
    float yPos = config.get<float>(s_V_OFFSET_CONFIG, 0.F);

    const float width  = config.get<float>(s_WIDTH_CONFIG, 1.F);
    const float height = config.get<float>(s_HEIGHT_CONFIG, 1.F);

    const std::map<std::string, float> horizAlignToX {
        {"left", xPos},
        {"center", 0.5F - width * 0.5F + xPos},
        {"right", 1.F - width - xPos}
    };

    const std::map<std::string, float> vertAlignToY {
        {"bottom", 1.F - height - yPos},
        {"center", 0.5F - height * 0.5F + yPos},
        {"top", yPos}
    };

    const std::string hAlign = config.get(s_H_ALIGN_CONFIG, "left");
    const std::string vAlign = config.get(s_V_ALIGN_CONFIG, "top");

    xPos = horizAlignToX.at(hAlign);
    yPos = vertAlignToY.at(vAlign);

    m_newViewportDimensions = std::tie(xPos, yPos, width, height);
}

//------------------------------------------------------------------------------

void SResizeViewport::starting()
{
    this->initialize();
    const auto* const camera = this->getLayer()->getDefaultCamera();
    const auto* const vp     = camera->getViewport();

    m_previousViewportDimensions = std::make_tuple(vp->getLeft(), vp->getTop(), vp->getWidth(), vp->getHeight());
}

//------------------------------------------------------------------------------

void SResizeViewport::updating() noexcept
{
}

//------------------------------------------------------------------------------

void SResizeViewport::stopping() noexcept
{
}

//------------------------------------------------------------------------------

void SResizeViewport::resizeViewport(bool _resize)
{
    auto* const camera = this->getLayer()->getDefaultCamera();
    auto* const vp     = camera->getViewport();

    this->getRenderService()->makeCurrent();

    const auto& newDimensions = _resize ? m_newViewportDimensions : m_previousViewportDimensions;
    const auto& [left, top, width, height] = newDimensions;
    vp->setDimensions(left, top, width, height);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SResizeViewport::resize()
{
    this->resizeViewport(true);
}

//------------------------------------------------------------------------------

void SResizeViewport::revert()
{
    this->resizeViewport(false);
}

} // namespace sight::module::viz::scene3d::adaptor.

/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "visuOgreAdaptor/SResizeViewport.hpp"

#include <fwCom/Slots.hxx>

#include <fwRenderOgre/Layer.hpp>

#include <OGRE/OgreCamera.h>

#include <tuple>

namespace visuOgreAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_REVERT_SLOT = "revert";
static const ::fwCom::Slots::SlotKeyType s_RESIZE_SLOT = "resize";

//------------------------------------------------------------------------------

SResizeViewport::SResizeViewport() noexcept
{
    newSlot(s_REVERT_SLOT, &SResizeViewport::revert, this);
    newSlot(s_RESIZE_SLOT, &SResizeViewport::resize, this);
}

//------------------------------------------------------------------------------

SResizeViewport::~SResizeViewport() noexcept
{

}

//------------------------------------------------------------------------------

void SResizeViewport::configuring()
{
    this->configureParams();

    const auto serviceCfg = this->getConfigTree();

    const auto vpConfig = serviceCfg.get_child_optional("config.<xmlattr>");

    if(vpConfig.has_value())
    {
        const auto cfg = vpConfig.value();

        float xPos = cfg.get<float>("hOffset", 0.f);
        float yPos = cfg.get<float>("vOffset", 0.f);

        const float width  = cfg.get<float>("width", 1.f);
        const float height = cfg.get<float>("height", 1.f);

        const std::map< std::string, float > horizAlignToX {
            { "left", xPos },
            { "center", 0.5f - width * 0.5f + xPos},
            { "right", 1.f - width - xPos }
        };

        const std::map< std::string, float > vertAlignToY {
            { "bottom", 1.f -height - yPos },
            { "center", 0.5f - height * 0.5f + yPos },
            { "top", yPos }
        };

        const std::string hAlign = cfg.get("hAlign", "left");
        const std::string vAlign = cfg.get("vAlign", "top");

        xPos = horizAlignToX.at(hAlign);
        yPos = vertAlignToY.at(vAlign);

        m_newViewportDimensions = std::tie(xPos, yPos, width, height);
    }
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

} // namespace visuOgreAdaptor.

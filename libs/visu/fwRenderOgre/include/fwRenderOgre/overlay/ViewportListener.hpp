/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#pragma once

#include "fwRenderOgre/Layer.hpp"

#include <OGRE/OgreRenderTargetListener.h>
#include <OGRE/OgreViewport.h>

#include <functional>
#include <map>

namespace fwRenderOgre::overlay
{
/**
 * @brief Listens to render events triggered for a given viewport and activates the overlays enabled for that viewport.
 */
class ViewportListener : public ::Ogre::RenderTargetListener
{
public:

    /// Maps each viewport to the overlays enabled on it.
    using ViewportOverlaysMapType = std::map< ::Ogre::Viewport*, std::reference_wrapper<const Layer::OverlaySetType> >;

    /// Constructor, sets the reference to the map between layers and viewports.
    FWRENDEROGRE_API ViewportListener(ViewportOverlaysMapType& _vpLayerMap);

    /// Destructor.
    FWRENDEROGRE_API virtual ~ViewportListener() final;

private:

    /// Called right before rendering in the viewport. Activates the overlays enabled for this viewport.
    virtual void preViewportUpdate(const ::Ogre::RenderTargetViewportEvent& evt) final;

    /// Called right after rendering in the viewport. Disables the overlays enabled for this viewport.
    virtual void postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) final;

    /// Maps each viewport to the overlays enabled on it.
    ViewportOverlaysMapType& m_viewportLayerMap;
};

} // namespace fwRenderOgre::overlay
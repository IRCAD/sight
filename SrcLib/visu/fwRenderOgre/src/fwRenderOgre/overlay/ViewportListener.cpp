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

#include "fwRenderOgre/overlay/ViewportListener.hpp"

#include "fwRenderOgre/Layer.hpp"

#include <OGRE/Overlay/OgreOverlayManager.h>

namespace fwRenderOgre::overlay
{

//-----------------------------------------------------------------------------

ViewportListener::ViewportListener(ViewportLayerMapType& _vpLayerMap) :
    m_viewportLayerMap(_vpLayerMap)
{
}

//-----------------------------------------------------------------------------

ViewportListener::~ViewportListener()
{
}

//-----------------------------------------------------------------------------

void ViewportListener::preViewportUpdate(const ::Ogre::RenderTargetViewportEvent& evt)
{
    const bool overlaysEnabled = evt.source->getOverlaysEnabled();

    if(overlaysEnabled)
    {
        Layer::sptr ogreLayer       = m_viewportLayerMap.at(evt.source);
        const auto& enabledOverlays = ogreLayer->getEnabledOverlays();

        for(::Ogre::Overlay* const overlay : enabledOverlays)
        {
            overlay->show();
        }
    }

    this->::Ogre::RenderTargetListener::preViewportUpdate(evt);
}

//-----------------------------------------------------------------------------

void ViewportListener::postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
    const bool overlaysEnabled = evt.source->getOverlaysEnabled();

    if(overlaysEnabled)
    {
        Layer::sptr ogreLayer       = m_viewportLayerMap.at(evt.source);
        const auto& enabledOverlays = ogreLayer->getEnabledOverlays();

        for(::Ogre::Overlay* const o : enabledOverlays)
        {
            o->hide();
        }
    }

    this->::Ogre::RenderTargetListener::postViewportUpdate(evt);
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre::overlay
/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <viz/scene3d/utils.hpp>

#include <OgreRenderWindow.h>
#include <OgreRoot.h>

namespace sight::module::viz::scene3d_qt
{

static inline Ogre::RenderWindow* s_render_window {};

//------------------------------------------------------------------------------

inline void init_resources()
{
    if(s_render_window == nullptr)
    {
        Ogre::NameValuePairList const params {
            {"hidden", "true"},
            {"currentGLContext", "true"}
        };

        auto* ogre_root = sight::viz::scene3d::utils::getOgreRoot();
        s_render_window = ogre_root->createRenderWindow("Dummy", 1, 1, false, &params);
        s_render_window->setVisible(false);
        s_render_window->setAutoUpdated(false);
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    }
}

} // namespace sight::module::viz::scene3d_qt

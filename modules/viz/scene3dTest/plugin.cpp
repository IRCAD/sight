/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "modules/viz/scene3dTest/plugin.hpp"

#include <viz/scene3d/utils.hpp>
#include <viz/scene3d/window_manager.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreTextureManager.h>

namespace sight::module::viz::scene3d::test
{

SIGHT_REGISTER_PLUGIN("sight::module::viz::scene3d::test::plugin");

//-----------------------------------------------------------------------------

plugin::~plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void plugin::start()
{
    m_ogre_root = sight::viz::scene3d::utils::get_ogre_root();

    Ogre::NameValuePairList const params {
        {"hidden", "true"},
    };

    // Use a size > 120 because windows will anyway switch to a larger size
    auto* window = m_ogre_root->createRenderWindow(
        "test",
        static_cast<unsigned int>(200),
        static_cast<unsigned int>(200),
        false,
        &params
    );
    sight::viz::scene3d::window_manager::sptr mgr = sight::viz::scene3d::window_manager::get();
    mgr->add(window);

    window->setVisible(false);
    window->setAutoUpdated(false);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
    m_ogre_root = nullptr;
    sight::viz::scene3d::utils::destroy_ogre_root();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::test

/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/scene3d/WindowManager.hpp"

#include "viz/scene3d/Utils.hpp"

#include <OgreResourceGroupManager.h>

namespace sight::viz::scene3d
{

// ----------------------------------------------------------------------------

void WindowManager::registerWindow(::Ogre::RenderWindow* _window)
{
    if(m_windows.empty())
    {
        m_firstWindow = _window;
    }
    m_windows.insert(_window);
}

// ----------------------------------------------------------------------------

void WindowManager::unregisterWindow(::Ogre::RenderWindow* _window)
{
    m_windows.erase(_window);

    if(_window != m_firstWindow)
    {
        ::Ogre::Root* root = viz::scene3d::Utils::getOgreRoot();
        SLM_ASSERT("Ogre root not found", root);
        root->destroyRenderTarget(_window);
    }

    // If we are the last opened window, we shutdown Ogre completely.
    if(m_windows.empty())
    {
        // We don't need to destroy the ogre target, it is done when shutting down ogre.
        // Destroying the render target actually results in a double free segfault.
        viz::scene3d::Utils::destroyOgreRoot();
        m_firstWindow = nullptr;
    }
}

// ----------------------------------------------------------------------------

viz::scene3d::WindowManager::sptr WindowManager::get()
{
    return core::LazyInstantiator< viz::scene3d::WindowManager >::getInstance();
}

// ----------------------------------------------------------------------------

bool WindowManager::hasWindow()
{
    return !m_windows.empty();
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d

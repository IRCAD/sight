/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwRenderOgre/WindowManager.hpp"

#include "fwRenderOgre/Utils.hpp"

#include <OgreResourceGroupManager.h>

namespace fwRenderOgre
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

    ::Ogre::Root* root = ::fwRenderOgre::Utils::getOgreRoot();

    if(_window != m_firstWindow)
    {
        SLM_ASSERT("Ogre root not found", root);
        root->destroyRenderTarget(_window);
    }

    // If we are the last opened window, we shutdown Ogre completely
    if(m_windows.empty())
    {
        // We don't need to destroy the ogre target, it is done when shutting down ogre.
        // Destroying the render target actually results in a double free segfault.
        ::fwRenderOgre::Utils::destroyOgreRoot();
        m_firstWindow = nullptr;
    }
}

// ----------------------------------------------------------------------------

::fwRenderOgre::WindowManager::sptr WindowManager::get()
{
    return ::fwCore::util::LazyInstantiator< ::fwRenderOgre::WindowManager >::getInstance();
}

// ----------------------------------------------------------------------------

bool WindowManager::hasWindow()
{
    return !m_windows.empty();
}

// ----------------------------------------------------------------------------

} // namespace fwRenderOgre

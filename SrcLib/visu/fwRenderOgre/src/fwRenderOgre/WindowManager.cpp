/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
        root->destroyRenderTarget(m_firstWindow);
        m_firstWindow = nullptr;

        ::fwRenderOgre::Utils::destroyOgreRoot();
    }
}

// ----------------------------------------------------------------------------

bool fwRenderOgre::WindowManager::hasWindow()
{
    return !m_windows.empty();
}

// ----------------------------------------------------------------------------

} // namespace fwRenderOgre

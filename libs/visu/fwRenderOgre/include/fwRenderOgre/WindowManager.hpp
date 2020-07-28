/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#pragma once

#include "fwRenderOgre/config.hpp"

#include <fwCore/base.hpp>
#include <fwCore/util/LazyInstantiator.hpp>

#include <OGRE/OgreRenderWindow.h>

namespace fwRenderOgre
{

/**
 * @brief This singleton stores all Ogre render windows and manage their deletion.
 *
 * The problem is that the OpenGLRenderSystem we use, somehow, needs the first window created to be deleted last.
 * This class allows to differ the deletion of the first created window.
 */
class FWRENDEROGRE_CLASS_API WindowManager : public ::fwCore::BaseObject
{

public:

    fwCoreClassMacro(WindowManager, ::fwRenderOgre::WindowManager, new WindowManager)

    fwCoreAllowSharedFromThis()

    /// Registers an Ogre window.
    FWRENDEROGRE_API void registerWindow(::Ogre::RenderWindow* _window);

    /// Unregisters an Ogre window.
    FWRENDEROGRE_API void unregisterWindow(::Ogre::RenderWindow* _window);

    /**
     * @brief Gets the unique instance of this class.
     * @return the singleton instance.
     */
    FWRENDEROGRE_API static ::fwRenderOgre::WindowManager::sptr get();

    /**
     * @brief Returns true if there is at least one window.
     * @@return true if at least one window is registered.
     */
    FWRENDEROGRE_API bool hasWindow();

private:

    /// Contains the first window.
    ::Ogre::RenderWindow* m_firstWindow;

    /// Stores all Ogre windows.
    std::set< ::Ogre::RenderWindow* > m_windows;

};

} // namespace fwRenderOgre.

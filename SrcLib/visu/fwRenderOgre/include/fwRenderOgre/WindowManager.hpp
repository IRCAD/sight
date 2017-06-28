/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_WINDOWMANAGER_HPP__
#define __FWRENDEROGRE_WINDOWMANAGER_HPP__

#include "fwRenderOgre/config.hpp"

#include <fwCore/base.hpp>
#include <fwCore/util/LazyInstantiator.hpp>

#include <OGRE/OgreRenderWindow.h>

namespace fwRenderOgre
{

/**
 * @brief This singleton stores all Ogre render windows and manage their deletion.
 *        The problem is that the OpenGLRendeSystem we use, somehow, needs the
 *        first window created to be deleted last.
 *        This class allows to differ the deletion of the first created window.
 */
class FWRENDEROGRE_CLASS_API WindowManager : public ::fwCore::BaseObject
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (WindowManager)(::fwRenderOgre::WindowManager), (()), new WindowManager);
    fwCoreAllowSharedFromThis();

    /// register an ogre window
    FWRENDEROGRE_API void registerWindow(::Ogre::RenderWindow* _window);

    /// Unregister an ogre window
    FWRENDEROGRE_API void unregisterWindow(::Ogre::RenderWindow* _window);

    /// Return the singleton instance
    FWRENDEROGRE_API static ::fwRenderOgre::WindowManager::sptr get()
    {
        return ::fwCore::util::LazyInstantiator< ::fwRenderOgre::WindowManager >::getInstance();
    }

    /// returns true if at least one window is registered
    FWRENDEROGRE_API bool hasWindow();

private:

    /// Pointer on the first window
    ::Ogre::RenderWindow* m_firstWindow;
    /// Tree containing all the ogre windows
    std::set< ::Ogre::RenderWindow* > m_windows;

};

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_WINDOWMANAGER_HPP__

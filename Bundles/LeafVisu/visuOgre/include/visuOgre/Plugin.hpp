/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGRE_PLUGIN_HPP__
#define __VISUOGRE_PLUGIN_HPP__

#include "visuOgre/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <OgreLog.h>

namespace visuOgre
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
class VISUOGRE_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Destructor
    ~Plugin() throw();

    // Overrides
    VISUOGRE_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    VISUOGRE_API void stop() throw();
};

/**
 * @brief Allows to redirect Ogre logs on FW4SPL logs
 *        We need this class to be declared outside to export DLL symbols on Windows.
 */
class VISUOGRE_CLASS_API F4SOgreListener : public ::Ogre::LogListener
{
public:
    /// Destructor, does nothing
    ~F4SOgreListener()
    {
    }

    /**
     * @brief Set the message on FW4SPL logs.
     *        Set Ogre log on FW4SPL log depending on it's LogLevel.
     */
    VISUOGRE_API virtual void messageLogged(const ::Ogre::String& message, ::Ogre::LogMessageLevel lml,
                                            bool maskDebug, const ::Ogre::String& logName, bool& skipThisMessage);
};

} // namespace visuOgre

#endif //__VISUOGRE_PLUGIN_HPP__

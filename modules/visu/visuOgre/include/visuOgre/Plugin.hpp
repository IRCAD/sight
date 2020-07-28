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

#include "visuOgre/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <OgreLog.h>

namespace visuOgre
{

/**
 * @brief Allows to redirect Ogre logs on Sight logs
 * We need this class to be declared outside to export DLL symbols on Windows.
 */
class VISUOGRE_CLASS_API SightOgreListener final : public ::Ogre::LogListener
{

public:

    /// Does nothing.
    ~SightOgreListener()
    {
    }

    /**
     * @brief Set Ogre log on Sight log depending on it's LogLevel.
     */
    VISUOGRE_API void messageLogged(const ::Ogre::String& _message,
                                    ::Ogre::LogMessageLevel _lml,
                                    bool,
                                    const ::Ogre::String&,
                                    bool& _skipThisMessage) override;
};

/// This class is started when the module is loaded.
class VISUOGRE_CLASS_API Plugin final : public ::fwRuntime::Plugin
{
public:

    /// Destroys the plugin.
    ~Plugin() noexcept override;

    /// Creates the Ogre log manager.
    VISUOGRE_API void start() override;

    /// Stops the plugin, destroys Ogre resources.
    VISUOGRE_API void stop() noexcept override;

private:

    /// Contains the Ogre's log manager.
    ::Ogre::LogManager* m_logManager { nullptr };

    /// Contains the Ogre's log.
    ::Ogre::Log* m_log { nullptr };

    /// Contains the Ogre listener.
    SightOgreListener* m_listener { nullptr };

};

} // namespace visuOgre.

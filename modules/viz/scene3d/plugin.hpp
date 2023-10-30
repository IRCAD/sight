/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/viz/scene3d/config.hpp"

#include <core/runtime/plugin.hpp>

#include <OgreLog.h>

namespace sight::module::viz::scene3d
{

/**
 * @brief Allows to redirect Ogre logs on Sight logs
 * We need this class to be declared outside to export DLL symbols on Windows.
 */
class MODULE_VIZ_SCENE3D_CLASS_API sight_ogre_listener final : public Ogre::LogListener
{
public:

    /// Does nothing.
    ~sight_ogre_listener() override
    = default;

    /**
     * @brief Set Ogre log on Sight log depending on it's LogLevel.
     */
    MODULE_VIZ_SCENE3D_API void messageLogged(
        const Ogre::String& _message,
        Ogre::LogMessageLevel _lml,
        bool /*maskDebug*/,
        const Ogre::String& /*logName*/,
        bool& _skip_this_message
    ) override;
};

/// This class is started when the module is loaded.
class MODULE_VIZ_SCENE3D_CLASS_API plugin final : public core::runtime::plugin
{
public:

    /// Destroys the plugin.
    ~plugin() noexcept override;

    /// Creates the Ogre log manager.
    MODULE_VIZ_SCENE3D_API void start() override;

    /// Stops the plugin, destroys Ogre resources.
    MODULE_VIZ_SCENE3D_API void stop() noexcept override;

private:

    /// Contains the Ogre's log manager.
    Ogre::LogManager* m_log_manager {nullptr};

    /// Contains the Ogre's log.
    Ogre::Log* m_log {nullptr};

    /// Contains the Ogre listener.
    sight_ogre_listener* m_listener {nullptr};
};

} // namespace sight::module::viz::scene3d.

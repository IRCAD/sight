/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "TutoSceneQt3DCpp/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppManager.hpp>

namespace TutoSceneQt3DCpp
{

/// This class is started when the module is loaded.
class TUTOSCENEQT3DCPP_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// Creates the plugin.
    TUTOSCENEQT3DCPP_API Plugin() noexcept;

    /// Destroy the plugin.
    TUTOSCENEQT3DCPP_API ~Plugin() noexcept override;

private:

    /// Does nothing.
    void start() override;

    /// Does nothing.
    void stop() noexcept override;

    /// Creates a window with a Qt3D rendering scene.
    void initialize() override;

    /// Destroys and reset the AppManager.
    void uninitialize() noexcept override;

    /// Contains the AppManager used to instantiate services.
    std::unique_ptr< ::fwServices::AppManager > m_appManager;

};

} // namespace TutoSceneQt3DCpp.

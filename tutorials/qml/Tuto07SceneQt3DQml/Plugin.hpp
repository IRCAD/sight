/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "Tuto07SceneQt3DQml/config.hpp"

#include <core/runtime/Plugin.hpp>

/**
 * Do not mark `TUTO07SCENEQT3DQML` as incorrect.
 * cspell:ignore TUTO07SCENEQT3DQML
 */
namespace Tuto07SceneQt3DQml
{

/**
 * @brief This class starts the software when the bundles is loaded.
 */

class TUTO07SCENEQT3DQML_CLASS_API Plugin : public sight::core::runtime::Plugin
{
public:

    /// Constructor.
    TUTO07SCENEQT3DQML_API Plugin() noexcept;

    /// Destructor. Does nothing.
    TUTO07SCENEQT3DQML_API ~Plugin() noexcept;

private:

    /// Registers fwRenderQt3D features to be used as QML types.
    void start() override;

    /// Does nothing.
    void stop() noexcept override;

    /// Runs 'ui.qml' containing the scene declaration.
    void initialize() override;

    /// Does nothing.
    void uninitialize() noexcept override;
};

} // namespace Tuto07SceneQt3DQml.

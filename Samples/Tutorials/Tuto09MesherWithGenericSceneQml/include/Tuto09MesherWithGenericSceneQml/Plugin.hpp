/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto09MesherWithGenericSceneQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace Tuto09MesherWithGenericSceneQml
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
class TUTO09MESHERWITHGENERICSCENEQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO09MESHERWITHGENERICSCENEQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO09MESHERWITHGENERICSCENEQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO09MESHERWITHGENERICSCENEQML_API void start();

    /// Overrides stop method. Do nothing
    TUTO09MESHERWITHGENERICSCENEQML_API void stop() noexcept;

    TUTO09MESHERWITHGENERICSCENEQML_API void initialize();

    TUTO09MESHERWITHGENERICSCENEQML_API void uninitialize() noexcept;

};

} // namespace Tuto09MesherWithGenericSceneQml

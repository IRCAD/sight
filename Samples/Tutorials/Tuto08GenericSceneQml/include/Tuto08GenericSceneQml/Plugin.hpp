/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto08GenericSceneQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace Tuto08GenericSceneQml
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
class TUTO08GENERICSCENEQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO08GENERICSCENEQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO08GENERICSCENEQML_API ~Plugin() noexcept;

    /// Overrides start method.
    TUTO08GENERICSCENEQML_API void start();

    /// Overrides stop method. Do nothing
    TUTO08GENERICSCENEQML_API void stop() noexcept;

    TUTO08GENERICSCENEQML_API void initialize();

    TUTO08GENERICSCENEQML_API void uninitialize() noexcept;
};

} // namespace Tuto02DataServiceBasicCtrl

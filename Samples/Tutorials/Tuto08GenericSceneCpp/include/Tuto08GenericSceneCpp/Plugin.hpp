/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto08GenericSceneCpp/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppManager.hpp>

namespace Tuto08GenericSceneCpp
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTO08GENERICSCENECPP_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO08GENERICSCENECPP_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO08GENERICSCENECPP_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO08GENERICSCENECPP_API void start();

    /// Overrides stop method. Do nothing
    TUTO08GENERICSCENECPP_API void stop() noexcept;

    TUTO08GENERICSCENECPP_API void initialize();

    TUTO08GENERICSCENECPP_API void uninitialize() noexcept;

private:

    std::unique_ptr< ::fwServices::AppManager > m_appManager;
};

} // namespace Tuto08GenericSceneCpp

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto08GenericSceneCtrl/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppManager.hpp>

namespace Tuto08GenericSceneCtrl
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTO08GENERICSCENECTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO08GENERICSCENECTRL_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO08GENERICSCENECTRL_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO08GENERICSCENECTRL_API void start();

    /// Overrides stop method. Do nothing
    TUTO08GENERICSCENECTRL_API void stop() noexcept;

    TUTO08GENERICSCENECTRL_API void initialize();

    TUTO08GENERICSCENECTRL_API void uninitialize() noexcept;

private:

    std::unique_ptr< ::fwServices::AppManager > m_appManager;
};

} // namespace Tuto08GenericSceneCtrl

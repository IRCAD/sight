/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiNetwork/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace uiNetwork
{

/**
 * @brief   This class is started when the bundles is loaded
 * @deprecated uiNetwork bundle is no longer supported and will be removed in 19.0
 */
struct UINETWORK_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    UINETWORK_API ~Plugin() noexcept;

    /// Overrides
    UINETWORK_API void start();

    /// Overrides
    UINETWORK_API void stop() noexcept;

};

} // namespace uiNetwork

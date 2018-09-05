/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiZMQ/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace uiZMQ
{

/**
 * @brief   This class is started when the bundles is loaded
 * @deprecated uiZMQ bundle is no longer supported and will be removed in 19.0
 */
struct UIZMQ_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    UIZMQ_API ~Plugin() noexcept;

    /// Overrides
    UIZMQ_API void start();

    /// Overrides
    UIZMQ_API void stop() noexcept;

};

} // namespace uiZMQ

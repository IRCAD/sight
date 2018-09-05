/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioZMQ/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ioZMQ
{

/**
 * @brief   This class is started when the bundles is loaded
 * @deprecated ioZMQ bundle is no longer supported and will be removed in 19.0
 */
struct IOZMQ_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    IOZMQ_API ~Plugin() noexcept;

    /// Overrides
    IOZMQ_API void start();

    /// Overrides
    IOZMQ_API void stop() noexcept;

};

} // namespace ioZMQ

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "opDistorter/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace opDistorter
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
struct OPDISTORTER_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    OPDISTORTER_API ~Plugin() noexcept;

    /// Overrides start method. Object declarations.
    OPDISTORTER_API void start();

    /// Overrides stop method. Do nothing
    OPDISTORTER_API void stop() noexcept;

};

} // namespace opDistorter

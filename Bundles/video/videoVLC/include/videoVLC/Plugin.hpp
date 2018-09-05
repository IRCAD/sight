/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoVLC/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace videoVLC
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
struct VIDEOVLC_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VIDEOVLC_API void start();

    // Overrides
    VIDEOVLC_API void stop() noexcept;
};

} // namespace videoVLC

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <videoCharucoCalibration/config.hpp>

#include <fwRuntime/Plugin.hpp>

namespace videoCharucoCalibration
{

struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief Destructor
     */
    ~Plugin() noexcept;

    /**
     * @brief Start method.
     * @exception ::fwRuntime::RuntimeException.
     * This method is used by runtime in order to initialize the bundle.
     */
    void start();

    /**
     * @brief Stop method.
     * This method is used by runtime in order to close the bundle.
     */
    void stop() noexcept;
};

} // namespace videoCharucoCalibration

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwRuntime/Plugin.hpp>

namespace uiMedDataQml
{
/**
 * @brief   Contains editors attached to image.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    void start();

    void initialize();

    // Overrides
    void stop() noexcept;

};

} // namespace uiMedDataQml

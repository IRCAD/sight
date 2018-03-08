/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTK/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace visuVTK
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
struct VISUVTK_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    /// Overrides
    VISUVTK_API void start();

    /// Overrides
    VISUVTK_API void stop() noexcept;

};

} // namespace visuVTK

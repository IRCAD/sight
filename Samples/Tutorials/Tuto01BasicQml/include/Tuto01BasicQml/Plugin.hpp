/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto01BasicQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace Tuto01BasicQml
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
class TUTO01BASICQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO01BASICQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO01BASICQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO01BASICQML_API void start();

    /// Overrides stop method. Do nothing
    TUTO01BASICQML_API void stop() noexcept;

    TUTO01BASICQML_API void initialize();

    TUTO01BASICQML_API void uninitialize() noexcept;
};

} // namespace Tuto01BasicQml

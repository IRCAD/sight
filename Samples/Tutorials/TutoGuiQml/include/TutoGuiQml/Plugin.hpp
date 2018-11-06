/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "TutoGuiQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace TutoGuiQml
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
class TUTOGUIQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTOGUIQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTOGUIQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTOGUIQML_API void start();

    /// Overrides stop method. Do nothing
    TUTOGUIQML_API void stop() noexcept;

    TUTOGUIQML_API void initialize();

    TUTOGUIQML_API void uninitialize() noexcept;
};

} // namespace TutoGuiQml

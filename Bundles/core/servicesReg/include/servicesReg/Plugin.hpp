/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SERVICESREG_PLUGIN_HPP__
#define __SERVICESREG_PLUGIN_HPP__

#include "servicesReg/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace servicesReg
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
struct SERVICESREG_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    SERVICESREG_API ~Plugin() noexcept;

    /// Overrides start method. Service declarations (services declared in fwServices libraries ).
    SERVICESREG_API void start();

    /// Overrides initialize method
    SERVICESREG_API void initialize();

    /// Overrides uninitialize method
    SERVICESREG_API void uninitialize();

    /// Overrides stop method. Do nothing
    SERVICESREG_API void stop() noexcept;

};

} // namespace servicesReg

#endif // __SERVICESREG_PLUGIN_HPP__

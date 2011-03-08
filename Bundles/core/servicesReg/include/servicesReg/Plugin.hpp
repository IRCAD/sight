/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SERVICESREG_PLUGIN_HPP_
#define _SERVICESREG_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "servicesReg/config.hpp"

namespace servicesReg
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
struct SERVICESREG_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    SERVICESREG_API ~Plugin() throw();

    /// Overrides start method. Service declarations (services declared in fwServices / fwComEd libraries ).
    SERVICESREG_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides initialize method
    SERVICESREG_API void initialize() throw( ::fwRuntime::RuntimeException );

    /// Overrides uninitialize method
    SERVICESREG_API void uninitialize() throw( ::fwRuntime::RuntimeException );

    /// Overrides stop method. Do nothing
    SERVICESREG_API void stop() throw();

};

} // namespace servicesReg

#endif // _SERVICESREG_PLUGIN_HPP_

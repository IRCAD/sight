/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __BASICREGISTRATION_PLUGIN_HPP__
#define __BASICREGISTRATION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "basicRegistration/config.hpp"

namespace basicRegistration
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
struct BASICREGISTRATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    BASICREGISTRATION_API ~Plugin() throw();

    /// Overrides
    BASICREGISTRATION_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    BASICREGISTRATION_API void stop() throw();

};


} // namespace basicRegistration


#endif //__BASICREGISTRATION_PLUGIN_HPP__

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPITKREGISTRATION_PLUGIN_HPP__
#define __OPITKREGISTRATION_PLUGIN_HPP__

#include "opItkRegistration/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace opItkRegistration
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
struct OPITKREGISTRATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    OPITKREGISTRATION_API ~Plugin() throw();

    /// Overrides start method. Object declarations.
    OPITKREGISTRATION_API void start() throw( ::fwRuntime::RuntimeException );

    /// Overrides stop method. Do nothing
    OPITKREGISTRATION_API void stop() throw();

};

} // namespace opItkRegistration

#endif //__OPITKREGISTRATION_PLUGIN_HPP__

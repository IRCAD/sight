/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 
#ifndef _POINTLISTREGISTRATION_PLUGIN_HPP_
#define _POINTLISTREGISTRATION_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "pointListRegistration/config.hpp"

namespace pointListRegistration
{

/**
* @brief   This class is started when the bundles is loaded.
* @class   Plugin.
* @author  IRCAD (Research and Development Team).
* @date    2012.
*/
struct POINTLISTREGISTRATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    POINTLISTREGISTRATION_API ~Plugin() throw();

    /// Overrides
    POINTLISTREGISTRATION_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    POINTLISTREGISTRATION_API void stop() throw();

};


} // namespace pointListRegistration


#endif //_POINTLISTREGISTRATION_PLUGIN_HPP_

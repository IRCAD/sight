/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 
#ifndef _UINAVIGATION_PLUGIN_HPP_
#define _UINAVIGATION_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "uiNavigation/config.hpp"

namespace uiNavigation
{

/**
* @brief   This class is started when the bundles is loaded.
* @class   Plugin
*/
struct UINAVIGATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    UINAVIGATION_API ~Plugin() throw();

    /// Overrides
    UINAVIGATION_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    UINAVIGATION_API void stop() throw();

};


} // namespace uiNavigation


#endif //_UINAVIGATION_PLUGIN_HPP_

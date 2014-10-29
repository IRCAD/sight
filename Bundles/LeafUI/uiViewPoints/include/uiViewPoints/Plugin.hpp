/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 
#ifndef _UIVIEWPOINTS_PLUGIN_HPP_
#define _UIVIEWPOINTS_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "uiViewPoints/config.hpp"

namespace uiViewPoints
{

/**
* @brief   This class is started when the bundles is loaded.
* @class   Plugin
*/
struct UIVIEWPOINTS_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    UIVIEWPOINTS_API ~Plugin() throw();

    /// Overrides
    UIVIEWPOINTS_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    UIVIEWPOINTS_API void stop() throw();

};


} // namespace uiViewPoints


#endif //_UIVIEWPOINTS_PLUGIN_HPP_

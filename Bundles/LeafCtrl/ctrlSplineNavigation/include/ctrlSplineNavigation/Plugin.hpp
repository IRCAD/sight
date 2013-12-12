/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 
#ifndef _CTRLSPLINENAVIGATION_PLUGIN_HPP_
#define _CTRLSPLINENAVIGATION_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "ctrlSplineNavigation/config.hpp"

namespace ctrlSplineNavigation
{

/**
* @brief   This class is started when the bundles is loaded.
* @class   Plugin
*/
struct CTRLSPLINENAVIGATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    CTRLSPLINENAVIGATION_API ~Plugin() throw();

    /// Overrides
    CTRLSPLINENAVIGATION_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    CTRLSPLINENAVIGATION_API void stop() throw();

};


} // namespace ctrlSplineNavigation


#endif //_CTRLSPLINENAVIGATION_PLUGIN_HPP_

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 
#ifndef _UICPR_PLUGIN_HPP_
#define _UICPR_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "uiCPR/config.hpp"

namespace uiCPR
{

/**
* @brief   This class is started when the bundles is loaded.
* @class   Plugin
*/
struct UICPR_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    UICPR_API ~Plugin() throw();

    /// Overrides
    UICPR_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    UICPR_API void stop() throw();

};


} // namespace uiCPR


#endif //_UICPR_PLUGIN_HPP_

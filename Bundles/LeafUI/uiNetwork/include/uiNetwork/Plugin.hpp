/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UINETWORK_PLUGIN_HPP__
#define __UINETWORK_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>
#include "uiNetwork/config.hpp"

namespace uiNetwork
{

/**
 * @brief   This class is started when the bundles is loaded
 */
struct UINETWORK_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    UINETWORK_API ~Plugin() throw();

    /// Overrides
    UINETWORK_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    UINETWORK_API void stop() throw();

};


} // namespace uiNetwork


#endif //__UINETWORK_PLUGIN_HPP__

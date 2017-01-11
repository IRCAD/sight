/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIGTL_PLUGIN_HPP__
#define __UIIGTL_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiIGTL/config.hpp"

namespace uiIGTL
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
struct UIIGTL_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    UIIGTL_API ~Plugin() throw();

    /// Overrides
    UIIGTL_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    UIIGTL_API void stop() throw();

};


} // namespace uiIGTL


#endif //__UIIGTL_PLUGIN_HPP__

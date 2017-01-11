/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIZMQ_PLUGIN_HPP__
#define __UIZMQ_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiZMQ/config.hpp"

namespace uiZMQ
{

/**
 * @brief   This class is started when the bundles is loaded
 */
struct UIZMQ_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    UIZMQ_API ~Plugin() throw();

    /// Overrides
    UIZMQ_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    UIZMQ_API void stop() throw();

};


} // namespace uiZMQ


#endif //__UIZMQ_PLUGIN_HPP__

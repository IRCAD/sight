/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOZMQ_PLUGIN_HPP__
#define __IOZMQ_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ioZMQ/config.hpp"

namespace ioZMQ
{

/**
 * @brief   This class is started when the bundles is loaded
 */
struct IOZMQ_CLASS_API Plugin :
    public ::fwRuntime::Plugin
{

    /// Destructor
    IOZMQ_API ~Plugin() throw();

    /// Overrides
    IOZMQ_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    IOZMQ_API void stop() throw();

};


} // namespace ioZMQ


#endif //__IOZMQ_PLUGIN_HPP__

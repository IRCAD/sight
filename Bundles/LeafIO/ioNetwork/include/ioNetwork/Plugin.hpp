/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IONETWORK_PLUGIN_HPP__
#define __IONETWORK_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>
#include "ioNetwork/config.hpp"

namespace ioNetwork
{

/**
 * @brief   This class is started when the bundles is loaded
 */
struct IONETWORK_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    IONETWORK_API ~Plugin() throw();

    /// Overrides
    IONETWORK_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    IONETWORK_API void stop() throw();

};


} // namespace ioNetwork


#endif //__IONETWORK_PLUGIN_HPP__

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __POC07TIMELINE_PLUGIN_HPP__
#define __POC07TIMELINE_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "PoC07TimeLine/config.hpp"

namespace PoC07TimeLine
{

/**
 * @brief Class call when bundle is started/stopped
 *
 */
class POC07TIMELINE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    POC07TIMELINE_API ~Plugin() throw();

    /// This method is used by runtime to initialize the bundle.
    POC07TIMELINE_API void start() throw( ::fwRuntime::RuntimeException );

    /// This method is used by runtime to stop the bundle.
    POC07TIMELINE_API void stop() throw();

};

} // namespace PoC07TimeLine

#endif //__POC07TIMELINE_PLUGIN_HPP__

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EX02TIMELINE_PLUGIN_HPP__
#define __EX02TIMELINE_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "Ex02TimeLine/config.hpp"

namespace Ex02TimeLine
{

/**
 * @brief Class call when bundle is started/stopped
 *
 */
class EX02TIMELINE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    EX02TIMELINE_API ~Plugin() noexcept;

    /// This method is used by runtime to initialize the bundle.
    EX02TIMELINE_API void start();

    /// This method is used by runtime to stop the bundle.
    EX02TIMELINE_API void stop() noexcept;

};

} // namespace Ex02TimeLine

#endif //__EX02TIMELINE_PLUGIN_HPP__

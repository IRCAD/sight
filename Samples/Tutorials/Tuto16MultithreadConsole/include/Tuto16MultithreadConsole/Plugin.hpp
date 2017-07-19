/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO16MULTITHREADCONSOLE_PLUGIN_HPP__
#define __TUTO16MULTITHREADCONSOLE_PLUGIN_HPP__

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace Tuto16MultithreadConsole
{

/**
 * @brief Class call when bundle is started/stopped
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    TUTO16MULTITHREADCONSOLE_API ~Plugin() noexcept;

    /// This method is used by runtime to initialize the bundle.
    TUTO16MULTITHREADCONSOLE_API void start();

    /// This method is used by runtime to stop the bundle.
    TUTO16MULTITHREADCONSOLE_API void stop() noexcept;
};

} // namespace Tuto16MultithreadConsole

#endif //__TUTO16MULTITHREADCONSOLE_PLUGIN_HPP__

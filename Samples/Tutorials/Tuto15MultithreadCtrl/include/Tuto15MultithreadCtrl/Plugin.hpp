/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO15MULTITHREADCTRL_PLUGIN_HPP__
#define __TUTO15MULTITHREADCTRL_PLUGIN_HPP__

#include "Tuto15MultithreadCtrl/config.hpp"

#include <fwRuntime/Plugin.hpp>


namespace Tuto15MultithreadCtrl
{

/**
 * @brief Class call when bundle is started/stopped
 */
class TUTO15MULTITHREADCTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// PLugin destructor
    TUTO15MULTITHREADCTRL_API ~Plugin() throw();

    /// This method is used by runtime to initialize the bundle.
    TUTO15MULTITHREADCTRL_API void start() throw( ::fwRuntime::RuntimeException );

    /// This method is used by runtime to stop the bundle.
    TUTO15MULTITHREADCTRL_API void stop() throw();

    TUTO15MULTITHREADCTRL_API void initialize() throw( ::fwRuntime::RuntimeException );

    TUTO15MULTITHREADCTRL_API void uninitialize() throw();

    TUTO15MULTITHREADCTRL_API int run() throw();

};

} // namespace Tuto15MultithreadCtrl

#endif //__TUTO15MULTITHREADCTRL_PLUGIN_HPP__

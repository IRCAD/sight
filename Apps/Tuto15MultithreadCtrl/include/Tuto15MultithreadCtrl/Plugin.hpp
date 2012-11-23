/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _TUTO15MULTITHREADCTRL_PLUGIN_HPP_
#define _TUTO15MULTITHREADCTRL_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "Tuto15MultithreadCtrl/config.hpp"

namespace Tuto15MultithreadCtrl
{

/**
 * @brief Class call when bundle is started/stopped
 * @author  IRCAD (Research and Development Team).
 * @date    2012
 */
class TUTO15MULTITHREADCTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public :

    /// PLugin destructor
    TUTO15MULTITHREADCTRL_API ~Plugin() throw();

    /// This method is used by runtime to initialize the bundle.
    TUTO15MULTITHREADCTRL_API void start() throw( ::fwRuntime::RuntimeException );

    /// This method is used by runtime to stop the bundle.
    TUTO15MULTITHREADCTRL_API void stop() throw();

    TUTO15MULTITHREADCTRL_API void initialize() throw( ::fwRuntime::RuntimeException );

    TUTO15MULTITHREADCTRL_API void uninitialize() throw();

};

} // namespace Tuto15MultithreadCtrl

#endif //_TUTO15MULTITHREADCTRL_PLUGIN_HPP_

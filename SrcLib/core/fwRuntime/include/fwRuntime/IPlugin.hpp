/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_PLUGIN_HPP
#define _FWRUNTIME_PLUGIN_HPP

#include <string>

#include "fwRuntime/config.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/RuntimeException.hpp"



namespace fwRuntime
{



/**
 * @brief   Defines the plugin interface.
 * @struct  IPlugin
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
struct FWRUNTIME_CLASS_API IPlugin : public IExecutable
{

    /**
     * @brief   Destructor : does nothing.
     */
    FWRUNTIME_API virtual ~IPlugin();

    /**
     * @brief   Notifies the plugin about its start.
     */
    virtual void start() throw(::fwRuntime::RuntimeException) = 0;

    /**
     * @brief   Notifies the plugin about its initialisation.
     */
    virtual void initialize() throw(::fwRuntime::RuntimeException);

    /**
     * @brief   Notifies the plugin about its uninitialisation.
     */
    virtual void uninitialize() throw(::fwRuntime::RuntimeException);

    /**
     * @brief   Notifies the plugin about its stop.
     */
    virtual void stop() throw() = 0;

};



} // namespace fwRuntime

#endif // #define _FWRUNTIME_PLUGIN_HPP

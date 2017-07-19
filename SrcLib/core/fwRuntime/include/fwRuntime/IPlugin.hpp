/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_IPLUGIN_HPP__
#define __FWRUNTIME_IPLUGIN_HPP__

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
 *
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
    virtual void start() = 0;

    /**
     * @brief   Notifies the plugin about its initialisation.
     */
    FWRUNTIME_API virtual void initialize();

    /**
     * @brief   Notifies the plugin about its uninitialisation.
     */
    FWRUNTIME_API virtual void uninitialize();

    /**
     * @brief   Notifies the plugin about its stop.
     */
    virtual void stop() noexcept = 0;

};



} // namespace fwRuntime

#endif // __FWRUNTIME_IPLUGIN_HPP__

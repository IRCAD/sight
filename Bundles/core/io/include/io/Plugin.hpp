/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IO_PLUGIN_HPP__
#define __IO_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace io
{
/**
 * @brief   Implements a plugin for the io bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   Destructor
     */
    ~Plugin() noexcept;

    /**
     * @brief Start method. (does nothing)
     *
     * This method is used by runtime in order to initialize the bundle.
     *
     * @exception ::fwRuntime::RuntimeException
     */
    void start();

    /**
     * @brief Stop method. (does nothing)
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() noexcept;

};

} // namespace io

#endif //__IO_PLUGIN_HPP__

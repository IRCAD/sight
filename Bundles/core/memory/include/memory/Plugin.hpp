/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MEMORY_PLUGIN_HPP__
#define __MEMORY_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace memory
{
/**
 * @brief   Implements a plugin for the memory bundle.
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
     * This method is used by runtime in order to initialise the bundle.
     *
     * @exception ::fwRuntime::RuntimeException.
     */
    void start();

    /**
     * @brief Stop method. (does nothing)
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() noexcept;

};

} // namespace memory

#endif //__MEMORY_PLUGIN_HPP__

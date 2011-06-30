/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MEMORY_PLUGIN_HPP_
#define _MEMORY_PLUGIN_HPP_

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
    ~Plugin() throw();

    /**
     * @brief Start method. (does nothing)
     *
     * This method is used by runtime in order to initialise the bundle.
     *
     * @exception ::fwRuntime::RuntimeException.
     */
    void start() throw(::fwRuntime::RuntimeException);

    /**
     * @brief Stop method. (does nothing)
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() throw();

};

} // namespace memory

#endif //_MEMORY_PLUGIN_HPP_

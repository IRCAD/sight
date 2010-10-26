/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_PLUGIN_HPP_
#define _IODATA_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

namespace ioData
{

struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   Destructor
     */
    ~Plugin() throw();

    /**
     * @brief Start method.
     *
     * @exception ::fwRuntime::RuntimeException.
     * This method is used by runtime in order to initialize the bundle.
     */
    void start() throw(::fwRuntime::RuntimeException);

    /**
     * @brief Stop method.
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() throw();

};

} // namespace ioData

#endif //_IODATA_PLUGIN_HPP_

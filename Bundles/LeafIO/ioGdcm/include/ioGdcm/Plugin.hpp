/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOGDCM_PLUGIN_HPP_
#define _IOGDCM_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

namespace ioGdcm
{
/**
 * @author  Jean-Baptiste Fasquel
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    void stop() throw();

};

} // namespace ioGdcm

#endif //_IOGDCM_PLUGIN_HPP_

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTKGDCM_PLUGIN_HPP__
#define __IOVTKGDCM_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace ioVtkGdcm
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 *
 * @date    2009-2011
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    void start();

    // Overrides
    void stop() noexcept;

};

} // namespace ioVtkGdcm

#endif //__IOVTKGDCM_PLUGIN_HPP__

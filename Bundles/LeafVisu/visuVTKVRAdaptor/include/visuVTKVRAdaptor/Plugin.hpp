/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKVRADAPTOR_PLUGIN_HPP__
#define __VISUVTKVRADAPTOR_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "visuVTKVRAdaptor/config.hpp"

namespace visuVTKVRAdaptor
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 *
 * @date    2009.
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

} // namespace visuVTK

#endif // __VISUVTKVRADAPTOR_PLUGIN_HPP__

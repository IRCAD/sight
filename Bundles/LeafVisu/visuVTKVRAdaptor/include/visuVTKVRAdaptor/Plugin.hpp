/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKVRADAPTOR_PLUGIN_HPP__
#define __VISUVTKVRADAPTOR_PLUGIN_HPP__

#include "visuVTKVRAdaptor/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace visuVTKVRAdaptor
{

/**
 * @brief   This class is started when the bundles is loaded.
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

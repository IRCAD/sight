/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_PLUGIN_HPP__
#define __VISUVTKARADAPTOR_PLUGIN_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace visuVTKARAdaptor
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 *
 */
struct VISUVTKARADAPTOR_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    VISUVTKARADAPTOR_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    VISUVTKARADAPTOR_API void stop() throw();

};

} // namespace visuVTKARAdaptor

#endif //__VISUVTKARADAPTOR_PLUGIN_HPP__


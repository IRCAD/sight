/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKWX_PLUGIN_HPP_
#define _VISUVTKWX_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "visuVTKWx/config.hpp"

namespace visuVTKWx
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
struct VISUVTKWX_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    VISUVTKWX_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    VISUVTKWX_API void stop() throw();

};

} // namespace visuVTKWx

#endif //_VISUVTKWX_PLUGIN_HPP_

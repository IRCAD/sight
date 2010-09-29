/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISU_PLUGIN_HPP_
#define _VISU_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "visu/config.hpp"

namespace visu
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
struct VISU_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    VISU_API ~Plugin() throw();
    
    /// Overrides start method. Do nothing
    VISU_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides stop method. Do nothing
    VISU_API void stop() throw();
    
};

} // namespace visu

#endif //_VISU_PLUGIN_HPP_

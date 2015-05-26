/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATAREG_PLUGIN_HPP__
#define __ARDATAREG_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "arDataReg/config.hpp"

namespace arDataReg
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct ARDATAREG_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    ARDATAREG_API ~Plugin() throw();

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    ARDATAREG_API void start() throw( ::fwRuntime::RuntimeException );

    /// Overrides stop method. Do nothing
    ARDATAREG_API void stop() throw();

};

} // namespace arDataReg

#endif //__ARDATAREG_PLUGIN_HPP__

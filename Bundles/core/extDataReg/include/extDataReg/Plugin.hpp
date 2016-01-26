/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATAREG_PLUGIN_HPP__
#define __EXTDATAREG_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "extDataReg/config.hpp"

namespace extDataReg
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct EXTDATAREG_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    EXTDATAREG_API ~Plugin() throw();

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    EXTDATAREG_API void start() throw( ::fwRuntime::RuntimeException );

    /// Overrides stop method. Do nothing
    EXTDATAREG_API void stop() throw();

};

} // namespace extDataReg

#endif //__EXTDATAREG_PLUGIN_HPP__

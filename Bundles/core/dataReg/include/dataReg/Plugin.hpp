/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DATAREG_PLUGIN_HPP__
#define __DATAREG_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "dataReg/config.hpp"

namespace dataReg
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct DATAREG_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    DATAREG_API ~Plugin() throw();

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    DATAREG_API void start() throw( ::fwRuntime::RuntimeException );

    /// Overrides stop method. Do nothing
    DATAREG_API void stop() throw();

};

} // namespace dataReg

#endif //__DATAREG_PLUGIN_HPP__

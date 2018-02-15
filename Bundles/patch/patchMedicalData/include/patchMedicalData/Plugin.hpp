/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __PATCHMEDICALDATA_PLUGIN_HPP__
#define __PATCHMEDICALDATA_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "patchMedicalData/config.hpp"

namespace patchMedicalData
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct PATCHMEDICALDATA_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    PATCHMEDICALDATA_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    PATCHMEDICALDATA_API void start();

    /// Overrides stop method. Do nothing
    PATCHMEDICALDATA_API void stop() noexcept;

};

} // namespace patchMedicalData

#endif //__PATCHMEDICALDATA_PLUGIN_HPP__

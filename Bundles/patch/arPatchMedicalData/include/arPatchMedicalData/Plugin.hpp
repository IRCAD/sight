/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARPATCHMEDICALDATA_PLUGIN_HPP__
#define __ARPATCHMEDICALDATA_PLUGIN_HPP__

#include "arPatchMedicalData/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace arPatchMedicalData
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct ARPATCHMEDICALDATA_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    ARPATCHMEDICALDATA_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    ARPATCHMEDICALDATA_API void start();

    /// Overrides stop method. Do nothing
    ARPATCHMEDICALDATA_API void stop() noexcept;

};

} // namespace arPatchMedicalData

#endif //__ARPATCHMEDICALDATA_PLUGIN_HPP__

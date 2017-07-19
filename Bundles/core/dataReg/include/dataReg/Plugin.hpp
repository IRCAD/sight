/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DATAREG_PLUGIN_HPP__
#define __DATAREG_PLUGIN_HPP__

#include "dataReg/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace dataReg
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class DATAREG_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Destructor. Do nothing.
    DATAREG_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     * @todo All data are not register in this method, what data must be registered ?
     */
    DATAREG_API void start();

    /// Overrides stop method. Do nothing
    DATAREG_API void stop() noexcept;

private:
    /// Dummy variable members used to force link with fwDataCamp and fwMedDataCamp bundles
    /// Recent GCC versions (>= 5.4) strip local variables with -O2
    int m_fwDataCampVersion;
    int m_fwMedDataCampVersion;
};

} // namespace dataReg

#endif //__DATAREG_PLUGIN_HPP__

/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

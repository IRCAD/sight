/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#ifndef __MATERIAL_PLUGIN_HPP__
#define __MATERIAL_PLUGIN_HPP__

#include "material/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace material
{

static const std::string s_MATERIAL_RESOURCEGROUP_NAME         = "materialsTemplate";
static const std::string s_COMPOSITOR_RESOURCEGROUP_NAME       = "compositorsPostFX";
static const std::string s_DEFAULT_MATERIAL_RESOURCEGROUP_NAME = "Materials";

/**
 * @brief   This class is started when the bundles is loaded.
 */

struct MATERIAL_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    MATERIAL_API ~Plugin() noexcept;

    // Overrides
    MATERIAL_API void start();

    // Overrides
    MATERIAL_API void stop() noexcept;
};

} // namespace material

#endif //__MATERIAL_PLUGIN_HPP__

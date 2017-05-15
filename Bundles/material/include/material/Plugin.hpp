/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    MATERIAL_API ~Plugin() throw();

    // Overrides
    MATERIAL_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    MATERIAL_API void stop() throw();
};

} // namespace material

#endif //__MATERIAL_PLUGIN_HPP__

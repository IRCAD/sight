/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#pragma once

#include "viz/scene3d/config.hpp"

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/R2VBRenderable.hpp>

#include <OGRE/OgreMaterial.h>

namespace sight::viz::scene3d
{

/// This adaptor adapts a data::material, allowing to tweak material parameters.
class VIZ_SCENE3D_CLASS_API IMaterialAdaptor : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(IMaterialAdaptor, sight::viz::scene3d::adaptor);

    /**
     * @name In-Out In-Out API
     * @{
     */
    static inline const std::string_view s_MATERIAL_INOUT = "material";
    /** @} */

    VIZ_SCENE3D_CLASS_API IMaterialAdaptor() noexcept           = default;
    VIZ_SCENE3D_CLASS_API ~IMaterialAdaptor() noexcept override = default;

    /// Configures the adaptor without using the XML configuration.
    VIZ_SCENE3D_CLASS_API virtual void configure(
        const std::string& _id,
        const std::string& _name,
        sight::viz::scene3d::render::sptr _service,
        const std::string& _layer,
        const std::string& _shadingMode = "",
        const std::string& _template    = sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    )                                   = 0;

    /// Gets Ogre associated material.
    VIZ_SCENE3D_CLASS_API virtual Ogre::MaterialPtr getMaterial() = 0;

    /// Gets material name.
    VIZ_SCENE3D_CLASS_API virtual std::string getMaterialName() const = 0;

    /// Retrieves the associated texture name.
    VIZ_SCENE3D_CLASS_API virtual void setTextureName(const std::string& _textureName) = 0;

    /// Sets material name.
    VIZ_SCENE3D_CLASS_API virtual void setMaterialName(const std::string& _materialName) = 0;

    /// Sets material template name.
    VIZ_SCENE3D_CLASS_API virtual void setMaterialTemplateName(const std::string& _materialName) = 0;

    /// Tells if there is a texture currently bound.
    VIZ_SCENE3D_CLASS_API virtual bool hasDiffuseTexture() const = 0;

    /// Gets the shading mode.
    VIZ_SCENE3D_CLASS_API virtual const std::string& getShadingMode() const = 0;

    /// Sets the shading mode.
    VIZ_SCENE3D_CLASS_API virtual void setShadingMode(const std::string& _shadingMode) = 0;

    /// Set the renderable object.
    VIZ_SCENE3D_CLASS_API virtual void setR2VBObject(sight::viz::scene3d::R2VBRenderable* _r2vbObject) = 0;

    /// Gets the internal material code.
    VIZ_SCENE3D_CLASS_API virtual sight::viz::scene3d::material* getMaterialFw() const = 0;
};

} // namespace sight::viz::scene3d.

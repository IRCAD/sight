/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/r2vb_renderable.hpp>

#include <OGRE/OgreMaterial.h>

namespace sight::viz::scene3d
{

/// This adaptor adapts a data::material, allowing to tweak material parameters.
class SIGHT_VIZ_SCENE3D_CLASS_API material_adaptor : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(material_adaptor, sight::viz::scene3d::adaptor);

    /**
     * @name In-Out In-Out API
     * @{
     */
    static inline const std::string_view MATERIAL_INOUT = "material";
    /** @} */

    SIGHT_VIZ_SCENE3D_CLASS_API material_adaptor() noexcept = default;
    SIGHT_VIZ_SCENE3D_CLASS_API ~material_adaptor() noexcept override = default;

    /// Configures the adaptor without using the XML configuration.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual void configure(
        const std::string& _id,
        const std::string& _name,
        sight::viz::scene3d::render::sptr _service,
        const std::string& _layer,
        const std::string& _shading_mode = "",
        const std::string& _template     = sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    )                                    = 0;

    /// Gets Ogre associated material.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual Ogre::MaterialPtr get_material() = 0;

    /// Gets material name.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual std::string get_material_name() const = 0;

    /// Retrieves the associated texture name.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual void set_texture_name(const std::string& _texture_name) = 0;

    /// Sets material name.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual void set_material_name(const std::string& _material_name) = 0;

    /// Sets material template name.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual void set_material_template_name(const std::string& _material_name) = 0;

    /// Tells if there is a texture currently bound.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual bool has_diffuse_texture() const = 0;

    /// Gets the shading mode.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual const std::string& get_shading_mode() const = 0;

    /// Sets the shading mode.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual void set_shading_mode(const std::string& _shading_mode) = 0;

    /// Set the renderable object.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual void set_r2_vb_object(sight::viz::scene3d::r2vb_renderable* _r2vb_object) = 0;

    /// Gets the internal material code.
    SIGHT_VIZ_SCENE3D_CLASS_API virtual sight::viz::scene3d::material* get_material_fw() const = 0;
};

} // namespace sight::viz::scene3d.

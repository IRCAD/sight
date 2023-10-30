/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/material.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <core/com/slot.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/text.hpp>
#include <viz/scene3d/transformable.hpp>

#include <Ogre.h>

namespace sight::data
{

class Material;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a simple coordinate system.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): sets whether the axis is shown or not.
 * - \b toggleVisibility(): toggle whether the axis is shown or not.
 * - \b show(): shows the axis.
 * - \b hide(): hides the axis.
 * - \b updateOriginColor(data::color::sptr): update origin sphere with provided color.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::axis">
        <config transform="transformUID" length="30" label="true" name="A1" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the transform adaptor.
 * - \b visible (optional, bool, default=true): the visibility of the axis.
 * - \b origin (optional, bool, default=false): the origin visibility.
 * - \b originColor (optional, hexadecimal, default=#FFFFFF): the color of the axis origin.
 * - \b length (optional, float, default=50.f): axis length in scene units.
 * - \b label (optional, bool, default=true): display axis names.
 * - \b fontSize (optional, unsigned int, default=16): label font size in points.
 * - \b name (optional, string): displayed name of the axis (default empty).
 */
class MODULE_VIZ_SCENE3D_CLASS_API axis final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(axis, sight::viz::scene3d::adaptor);

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_SCENE3D_API axis() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~axis() noexcept override = default;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Creates manual objects in the default ogre resource group.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /// Sends a render request.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Deletes ogre's resources.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the axis visibility.
     * @param _visible the visibility status of the axis.
     */
    MODULE_VIZ_SCENE3D_API void set_visible(bool _visible) override;

private:

    /// SLOT: internal function to update origin color using provided data::color.
    void update_origin_color(sight::data::color::sptr _new_color);

    /// Contains the material data.
    data::material::sptr m_material {nullptr};

    /// Contains the material for the origin (to change its color dynamically).
    data::material::sptr m_origin_material {nullptr};

    /// Defines the axis length in scene units.
    float m_length {50.F};

    /// Enables the visibility of axis labels.
    bool m_enable_label {true};

    /// Contains the origin of the axis.
    Ogre::ManualObject* m_origin {nullptr};

    /// Enables the origin visibility.
    bool m_origin_visibility {false};

    /// Enables the axes visibility.
    bool m_axis_visibility {true};

    /// Defines the origin color.
    std::string m_origin_color {"#FFFFFF"};

    /// Contains the line along the x axis.
    Ogre::ManualObject* m_x_line {nullptr};

    /// Contains the line along the y axis.
    Ogre::ManualObject* m_y_line {nullptr};

    /// Contains the line along the z axis.
    Ogre::ManualObject* m_z_line {nullptr};

    /// Contains the arrow of the x axis.
    Ogre::ManualObject* m_x_cone {nullptr};

    /// Contains the arrow of the y axis.
    Ogre::ManualObject* m_y_cone {nullptr};

    /// Contains the arrow of the z axis.
    Ogre::ManualObject* m_z_cone {nullptr};

    /// Contains the scene node where all of manual objects are attached.
    Ogre::SceneNode* m_scene_node {nullptr};

    /// Stores labels attached to each axis.
    std::array<sight::viz::scene3d::text::sptr, 3> m_axis_labels {{nullptr, nullptr, nullptr}};

    sight::viz::scene3d::text::sptr m_axis_name_txt {nullptr};

    /// Defines labels font size in points.
    std::size_t m_font_size {12};

    /// Axis name, default empty.
    std::string m_axis_name;
};

} // namespace sight::module::viz::scene3d::adaptor.

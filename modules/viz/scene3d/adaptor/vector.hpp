/************************************************************************
 *
 * Copyright (C) 2019-2025 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <data/color.hpp>
#include <data/real.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/transformable.hpp>

#include <Ogre.h>

namespace sight::data
{

class Material;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a simple vector.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): Sets whether the vector is shown or not.
 * - \b toggle_visibility(): Toggle whether the vector is shown or not.
 * - \b show(): Shows the vector.
 * - \b hide(): Hides the vector.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::vector">
        <config transform="..." />
        <properties transform="..." length="30" color="#FFFFFF" visible="true"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the transform adaptor.
 * @subsection Properties Properties:
 * - \b length (optional, float, default=1.): axis length in mm.
 * - \b color (optional, hexadecimal, default=#FFFFFF): color of the vector.
 * - \b visible (optional, bool, default=true): visibility of the vector.
 */
class vector final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(vector, sight::viz::scene3d::adaptor);

    /// Initialise slots.
    vector() noexcept = default;

    /// Destroys the adaptor.
    ~vector() noexcept final = default;

protected:

    /// Connects the input matrix modified to the update slot.
    sight::service::connections_t auto_connections() const final;

    /// Configures the adaptor
    void configuring() final;

    /// Initialize the main node and the material.
    void starting() final;

    /// Deletes resources.
    void stopping() final;

    /// Create with the specified length the vector.
    void updating() final;

    /**
     * @brief Sets the vector visibility.
     * @param _visible the visibility status of the vector.
     */
    void set_visible(bool _visible) final;

private:

    /// Create the vector.
    void create_vector();

    /// Deletes the vector resources.
    void delete_vector();

    /// Contains the line along the z axis.
    Ogre::ManualObject* m_line {nullptr};

    /// Contains the arrow of the z axis.
    Ogre::ManualObject* m_cone {nullptr};

    /// Contains the scene node where all of our manual objects are attached.
    Ogre::SceneNode* m_scene_node {nullptr};

    /// Contains the material used to draw the vector.
    sight::viz::scene3d::material::standard::uptr m_material;

    sight::data::property<sight::data::real> m_length {this, "length", 1.0};
    sight::data::property<sight::data::color> m_color {this, "color", {1.0, 1.0, 1.0, 1.0}};
};

} // namespace sight::module::viz::scene3d::adaptor.

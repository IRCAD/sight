/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/material.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <core/com/slot.hpp>

#include <viz/scene3d/adaptor.hpp>
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
 * - \b update_length(float): set the length of the vector.
 * - \b show(): Shows the vector.
 * - \b hide(): Hides the vector.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::vector">
        <config transform="..." length="30" color="#FFFFFF" visible="true"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the transform adaptor.
 * - \b length (optional, float, default=1.): axis length in mm.
 * - \b color (optional, hexadecimal, default=#FFFFFF): color of the vector.
 * - \b visible (optional, bool, default=true): visibility of the vector.
 */
class MODULE_VIZ_SCENE3D_CLASS_API vector final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(vector, sight::viz::scene3d::adaptor);

    /// Initialise slots.
    MODULE_VIZ_SCENE3D_API vector() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~vector() noexcept final = default;

protected:

    /// Configures the adaptor
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Initialize the main node and the material.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /// Deletes resources.
    MODULE_VIZ_SCENE3D_API void stopping() final;

    /// Create with the specified length the vector.
    MODULE_VIZ_SCENE3D_API void updating() final;

    /**
     * @brief Sets the vector visibility.
     * @param _visible the visibility status of the vector.
     */
    MODULE_VIZ_SCENE3D_API void set_visible(bool _visible) final;

private:

    /**
     * @brief Updates the length of the vector.
     * @param _length length of the vector.
     */
    void update_length(float _length);

    /// Create the vector.
    void create_vector();

    /// Deletes the vector resources.
    void delete_vector();

    /// Contains the material data.
    data::material::sptr m_material {nullptr};

    /// Defines the axis length (in mm).
    float m_length {1.F};

    /// Defines the color of the vector.
    std::string m_color {"#FFFFFF"};

    /// Contains the line along the z axis.
    Ogre::ManualObject* m_line {nullptr};

    /// Contains the arrow of the z axis.
    Ogre::ManualObject* m_cone {nullptr};

    /// Contains the scene node where all of our manual objects are attached.
    Ogre::SceneNode* m_scene_node {nullptr};

    /// Contains the material used to draw the vector.
    module::viz::scene3d::adaptor::material::sptr m_material_adaptor {nullptr};
};

} // namespace sight::module::viz::scene3d::adaptor.

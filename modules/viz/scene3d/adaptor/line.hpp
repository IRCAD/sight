/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include <data/boolean.hpp>
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
 * @brief This adaptor shows a simple line.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): Sets whether the line is shown or not.
 * - \b toggle_visibility(): Toggle whether the line is shown or not.
 * - \b show(): shows the line.
 * - \b hide(): hides the line.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::line">
        <config transform="transformUID"  />
        <properties length="30" dashLength="2.5" color="#0000FF" dashed="false" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the transform adaptor
 * @subsection Properties Properties:
 * - \b length (optional, float, default=50.0): length of the line in mm (default 50)
 * - \b dash_length (optional, float, default=2.5): length of a dash
 * - \b color (optional, hexadecimal, default=#FFFFFF): color of the line
 * - \b dashed (optional, bool, default=false): display a dashed line instead of a solid line
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class line final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(line, sight::viz::scene3d::adaptor);

    /// Sets default parameters and initializes necessary members.
    line() noexcept = default;

    /// Destroys the adaptor.
    ~line() noexcept override = default;

protected:

    /// Connects the input matrix modified to the update slot.
    sight::service::connections_t auto_connections() const final;

    /// Configures the adaptor
    void configuring() override;

    /// Creates a mesh in the Default Ogre resource group
    void starting() override;

    /// Checks if the data::mesh has changed, and updates it if it has.
    void updating() override;

    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;

    /**
     * @brief Sets the line visibility.
     * @param _visible the visibility status of the line.
     */
    void set_visible(bool _visible) override;

private:

    /**
     * @brief Attaches a node in the scene graph.
     * @param _object node to attach.
     */
    void attach_node(Ogre::MovableObject* _object);

    /**
     * @brief Draws a line.
     * @param _existing_line use true if the line already exist.
     */
    void draw_line(bool _existing_line);

    /**
     * @brief SLOT: updates length of the line.
     * @param _length length of the line (in mm).
     */
    void update_length(float _length);

    /// Contains the material.
    sight::viz::scene3d::material::standard::uptr m_material;

    /// Contains the manual object of the line.
    Ogre::ManualObject* m_line {nullptr};

    sight::data::property<sight::data::real> m_length {this, "length", 50.0};
    sight::data::property<sight::data::color> m_color {this, "color", {1.0, 1.0, 1.0, 1.0}};
    sight::data::property<sight::data::boolean> m_dashed {this, "dashed", false};
    sight::data::property<sight::data::real> m_dash_length {this, "dash_length", 2.5};
};

} // namespace sight::module::viz::scene3d::adaptor.

/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/transformable.hpp>

#include <Ogre.h>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a grid that is intended to provide a horizon.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): Sets whether the line is shown or not.
 * - \b toggle_visibility(): Toggle whether the line is shown or not.
 * - \b show(): shows the line.
 * - \b hide(): hides the line.
 * - \b updateSize(float): Update the size of the grid
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::grid">
        <config transform="transformUID" size="30" dashLength="2.5" color="#0000FF" elevation="200"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the transform adaptor
 * - \b size (optional, float, default=50.0): size of a side of the grid, in mm.
 * - \b dashLength (optional, float, default=2.5): length of a dash, in mm.
 * - \b color (optional, hexadecimal, default=#FFFFFFFF): color of the grid lines.
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class grid final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    SIGHT_DECLARE_SERVICE(grid, sight::viz::scene3d::adaptor);

    grid() noexcept;
    ~grid() noexcept override = default;

protected:

    /// Configures the adaptor
    void configuring() override;

    /// Creates a mesh in the Default Ogre resource group
    void starting() override;

    /// Checks if the data::mesh has changed, and updates it
    void updating() override;

    /// Deletes the mesh after unregistering the service, and shutting down connections
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
     * @brief Draws the grid.
     * @param _existing_line use true if the line already exists.
     */
    void draw_grid(bool _existing_line);

    /**
     * @brief SLOT: updates the size of the grid.
     * @param _size size of one side of the grid.
     */
    void update_size(float _size);

    /// Contains the Ogre material.
    sight::viz::scene3d::material::standard::uptr m_material;

    /// Contains the manual object of the line.
    Ogre::ManualObject* m_line {nullptr};

    /// Defines the size of a side of the grid, in mm.
    float m_size {50.F};

    /// Number of lines in each direction (x,z) of the grid
    std::size_t m_step {10};

    /// Elevation of the grid in the Y axis, in mm.
    float m_elevation {-500.F};

    /// Defines the color of the line.
    Ogre::ColourValue m_color;

    /// Defines the length of one dash, in mm.
    float m_dash_length {2.5F};
};

} // namespace sight::module::viz::scene3d::adaptor.

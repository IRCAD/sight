/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "algo_mesh_deformation.hpp"

#include <data/mesh.hpp>

#include <ui/__/action.hpp>

namespace tuto13_mesh_generator_cpp
{

/**
 * @brief This action modifies a mesh using specified functor in configuration.
 * The purpose is to test all possibilities provide by the new mesh structure.
 *
 * @section XML XML Configuration
 * @code{.xml}
     <service type="tuto13_mesh_generator_cpp::mesh_modifier" >
         <inout key="mesh" uid="..." />
         <config functor="ShakeMeshPoint" />
     </service>
   @endcode
 * @subsection InOut InOut
 * - \b mesh [sight::data::mesh]: mesh to modify.
 *
 * @subsection Configuration Configuration
 * - \b functor (mandatory, string): the functor name used to generate mesh.
 *  - ShakeMeshPoint: shakes mesh points.
 *  - ColorizeMeshPoints: colorizes mesh points.
 *  - ColorizeMeshCells: colorizes mesh cells.
 *  - ComputePointNormals: computes point normals.
 *  - ComputeCellNormals: computes cell normals
 *  - ShakePointNormals: shakes point normals.
 *  - ShakeCellNormals: shakes cell normals.
 *  - MeshDeformation: deforms the mesh.
 */
class mesh_modifier final : public sight::ui::action
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(mesh_modifier, sight::ui::action);

    /// Destroyes the action.
    ~mesh_modifier() noexcept override = default;

protected:

    /// Configures the action.
    void configuring(const config_t& _config) override;

    /// Starts the action.
    void starting() override;

    /// Modifies the mesh using the selected functor.
    void updating() override;

    /// Stops the action.
    void stopping() override;

private:

    /// Defines the functor name used to generate mesh.
    std::string m_functor;

    /// Defines the algorithm used to animate a deformation on the mesh.
    tuto13_mesh_generator_cpp::algo_mesh_deformation m_animator;

    sight::data::ptr<sight::data::mesh, sight::data::access::inout> m_mesh {this, "mesh"};
};

} // namespace tuto13_mesh_generator_cpp.

/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "AlgoMeshDeformation.hpp"

#include <data/Mesh.hpp>

#include <ui/base/IAction.hpp>

namespace Tuto13MeshGeneratorCpp
{

/**
 * @brief This action modifies a mesh using specified functor in configuration.
 * The purpose is to test all possibilities provide by the new mesh structure.
 *
 * @section XML XML Configuration
 * @code{.xml}
     <service type="Tuto13MeshGeneratorCpp::SMeshModifier" >
         <inout key="mesh" uid="..." />
         <config functor="ShakeMeshPoint" />
     </service>
   @endcode
 * @subsection InOut InOut
 * - \b mesh [sight::data::Mesh]: mesh to modify.
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
class SMeshModifier final : public sight::ui::base::IAction
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SMeshModifier, sight::ui::base::IAction);

    /// Creates the action.
    SMeshModifier() noexcept;

    /// Destroyes the action.
    ~SMeshModifier() noexcept override;

protected:

    /// Configures the action.
    void configuring() override;

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
    Tuto13MeshGeneratorCpp::AlgoMeshDeformation m_animator;

    sight::data::ptr<sight::data::Mesh, sight::data::Access::inout> m_mesh {this, "mesh"};
};

} // namespace Tuto13MeshGeneratorCpp.

/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "Tuto14MeshGenerator/AlgoMeshDeformation.hpp"
#include "Tuto14MeshGenerator/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace Tuto14MeshGenerator
{

/**
 * @brief This action modifies a mesh using specified functor in configuration.
 * The purpose is to test all possibilities provide by the new mesh structure.
 *
 * @section XML XML Configuration
 * @code{.xml}
     <service type="::Tuto14MeshGenerator::SMeshModifier" >
         <inout key="mesh" uid="..." />
         <config functor="ShakeMeshPoint" />
     </service>
   @endcode
 * @subsection InOut InOut
 * - \b mesh [::fwData::Mesh]: mesh to modify.
 *
 * @subsection Configuration Configuration
 * - \b functor (mandatory, string): the functor name used to generate mesh.
 *  - ShakeMeshPoint: shakes meshe points.
 *  - ColorizeMeshPoints: colorizes mesh points.
 *  - ColorizeMeshCells: colorizes mesh cells.
 *  - ComputePointNormals: computes point normals.
 *  - ComputeCellNormals: computes cell normals
 *  - ShakePointNormals: shakes point normals.
 *  - ShakeCellNormals: shakes cell normals.
 *  - MeshDeformation: deforms the mesh.
 */
class TUTO14MESHGENERATOR_CLASS_API SMeshModifier final : public ::fwGui::IActionSrv
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SMeshModifier, ::fwGui::IActionSrv)

    /// Creates the action.
    TUTO14MESHGENERATOR_API SMeshModifier() noexcept;

    /// Destroyes the action.
    TUTO14MESHGENERATOR_API ~SMeshModifier() noexcept override;

protected:

    /// Configures the action.
    TUTO14MESHGENERATOR_API void configuring() override;

    /// Starts the action.
    TUTO14MESHGENERATOR_API void starting() override;

    /// Modifies the mesh using the selected functor.
    TUTO14MESHGENERATOR_API void updating() override;

    /// Stops the action.
    TUTO14MESHGENERATOR_API void stopping() override;

private:

    /// Defines the functor name used to generate mesh.
    std::string m_functor;

    /// Defines the algorithm used to animate a deformation on the mesh.
    ::Tuto14MeshGenerator::AlgoMeshDeformation m_animator;

};

} // namespace Tuto14MeshGenerator.

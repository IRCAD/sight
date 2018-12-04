/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
 * @brief   This action modifies a mesh using specified functor in configuration.
 * The purpose is to test all possibilities provide by the new mesh structure.

 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="::Tuto14MeshGenerator::SMeshModifier" >
         <inout key="mesh" uid="..." />
         <config functor="ShakeMeshPoint" />
     </service>
   @endcode
 * @subsection InOut InOut
 * - \b mesh [::fwData::Mesh]: mesh to modify.
 * @subsection Configuration Configuration
 * - \b functor
 * Functor available :
 *  - ShakeMeshPoint
 *  - ColorizeMeshPoints
 *  - ColorizeMeshCells
 *  - ComputePointNormals
 *  - ComputeCellNormals
 *  - ShakePointNormals
 *  - ShakeCellNormals
 *  - MeshDeformation
 */
class TUTO14MESHGENERATOR_CLASS_API SMeshModifier : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMeshModifier)(::fwGui::IActionSrv) )

    /**
     * @brief Constructor. Do nothing.
     */
    TUTO14MESHGENERATOR_API SMeshModifier() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    TUTO14MESHGENERATOR_API virtual ~SMeshModifier() noexcept;

protected:

    TUTO14MESHGENERATOR_API virtual void configuring() override;

    TUTO14MESHGENERATOR_API virtual void starting() override;

    TUTO14MESHGENERATOR_API virtual void stopping() override;

    /**
     * @brief Process the action: modifies the mesh using the selected functor.
     */
    TUTO14MESHGENERATOR_API virtual void updating() override;

    /**
     * @brief This method gives information about the class.
     */
    TUTO14MESHGENERATOR_API virtual void info(std::ostream& _sstream ) override;

    ///@}

private:

    /// Functor name used to generate mesh.
    std::string m_functor;

    /// Algo use to animate a deformation on the mesh
    ::Tuto14MeshGenerator::AlgoMeshDeformation m_animator;
};

} // namespace Tuto14MeshGenerator


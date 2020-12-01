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

#include "opVTKMesh/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace opVTKMesh
{

namespace action
{

/**
 * @brief   Action to create a mesh from an image using VTK library
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::opVTKMesh::action::SMeshCreation">
            <in key="image" uid="..." />
            <inout key="mesh" uid="..." />
            <percentReduction value="0" />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [::fwData::Image]: source image.
 *
 * @subsection In-Out In-Out:
 * - \b mesh [::fwData::Mesh]: target mesh.
 *
 * @subsection Configuration Configuration:
 * - \b percentReduction: Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)..
 */
class OPVTKMESH_CLASS_API SMeshCreation final : public ::fwGui::IActionSrv
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SMeshCreation, ::fwGui::IActionSrv)

    /// Initializes  members variables.
    OPVTKMESH_API SMeshCreation() noexcept;

    /// Destroys service.
    OPVTKMESH_API ~SMeshCreation() noexcept override;

protected:

    /// Starts the action.
    OPVTKMESH_API void starting() override;

    /// Stops the action.
    OPVTKMESH_API void stopping() override;

    /// Configures the service.
    OPVTKMESH_API void configuring() override;

    /// Processes the mesh creation from the image.
    OPVTKMESH_API void updating() override;

private:

    /// image size reduction value.
    unsigned int m_reduction;
};

} // namespace action.
} // namespace opVTKMesh.

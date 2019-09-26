/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "echoEdSimu/config.hpp"

#include <arServices/ISimulator.hpp>

#include <fwCom/Slot.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwTools/UUID.hpp>

namespace echoEdSimu
{

/**
 * @brief   Compute the transformation matrix that match the barycenter of a cell of a mesh.
 *
 * @section Slots Slots
 * - \b updateSingle(int, std::string) : called to update the "craniocaudal" or the "radial" index.
 * - \b updateBoth(int,int) : called to update the craniocaudal and the radial indexes.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::echoEdSimu::SComputeMatrixFromCell">
            <in key="radialMesh" uid="..." />
            <inout key="matrix" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b radialMesh [::fwData::Mesh]: mesh used to compute the cell.
 * @subsection In-Out In-Out:
 * - \b matrix [::fwData::TransformationMatrix3D]: output matrix.
 */
class ECHOEDSIMU_CLASS_API SComputeMatrixFromCell : public ::arServices::ISimulator
{

public:

    fwCoreServiceMacro(SComputeMatrixFromCell, ::arServices::ISimulator);

    /// Constructor.
    ECHOEDSIMU_API SComputeMatrixFromCell() noexcept;

    /// Destructor. Do nothing
    ECHOEDSIMU_API virtual ~SComputeMatrixFromCell() noexcept;

    /**
     * @name Slots API
     * @{
     */
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SINGLE_SLOT;
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_BOTH_SLOT;
    /** @} */

protected:

    /// Configure the service
    ECHOEDSIMU_API virtual void configuring() override;

    /// Get objects and calcute the number of cells per line and the number of line in the skin mesh.
    ECHOEDSIMU_API virtual void starting() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping() override;

    /// Grab the mesh and compute the 2D grid
    ECHOEDSIMU_API virtual void updating() override;

private:

    /// Slot: update the image in the craniocaudal direction or in the radial direction
    void updateSingle(int i, std::string key);

    /// Slot: Common function used to update the image
    void updateBoth(int i, int j);

    /// Key of the mesh or the model series which contains it.
    std::string m_meshKey;

    /// Mesh
    ::fwData::Mesh::csptr m_mesh;

    /// Number of cells in each line of the mesh
    unsigned int m_nbCellsPerLine;

    /// Current craniocaudal index
    int m_cranioCaudalIndex;
    /// Current radial index
    int m_radialIndex;
};

} // echoEdSimu

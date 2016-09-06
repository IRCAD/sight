/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ECHOEDSIMU_SCOMPUTEMATRIXFROMCELL_HPP__
#define __ECHOEDSIMU_SCOMPUTEMATRIXFROMCELL_HPP__

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
 * *
 * @section Slots Slots
 * - \b updateSignal(int, std::string) : called to update the "craniocaudal" or the "radial" index.
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
 * - \b radialMesh [::fwData::Object]: mesh used to compute the cell.
 * @subsection In-Out In-Out:
 * - \b matrix [::fwData::Object]: output matrix.
 */
class ECHOEDSIMU_CLASS_API SComputeMatrixFromCell : public ::arServices::ISimulator
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SComputeMatrixFromCell)(::arServices::ISimulator) );

    /// Constructor.
    ECHOEDSIMU_API SComputeMatrixFromCell() throw();

    /// Destructor. Do nothing
    ECHOEDSIMU_API virtual ~SComputeMatrixFromCell() throw();

    /**
     * @name Slots API
     * @{
     */
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SINGLE_SLOT;
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_BOTH_SLOT;
    /** @} */

protected:

    /// Configure the service
    ECHOEDSIMU_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Get objects and calcute the number of cells per line and the number of line in the skin mesh.
    ECHOEDSIMU_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Grab the mesh and compute the 2D grid
    ECHOEDSIMU_API virtual void updating() throw ( ::fwTools::Failed );

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

#endif // __ECHOEDSIMU_SCOMPUTEMATRIXFROMCELL_HPP__

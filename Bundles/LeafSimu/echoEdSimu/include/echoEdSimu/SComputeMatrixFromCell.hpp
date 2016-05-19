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
 *
 *          Please note that this service must be started AND updated before being able to trigger the slots that
 * generates the matrix for a given cell (i,j) in the mesh.
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
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CRANIOCAUDAL_SLOT;
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_RADIAL_SLOT;
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_BOTH_SLOT;
    /** @} */

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SComputeMatrixFromCellInstance" impl="::echoEdSimu::SComputeMatrixFromCell" >
            <compositeUID>${GENERIC_UID}_Root</compositeUID>
            <meshKey>meshKey</meshKey>
        </service>
       @endcode
     * - \b compositeUID : Key of the composite that contains the mesh.
     * - \b meshKey : key of the mesh or the model series which contains the mesh
     */
    ECHOEDSIMU_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Get objects and calcute the number of cells per line and the number of line in the skin mesh.
    ECHOEDSIMU_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Grab the mesh and compute the 2D grid
    ECHOEDSIMU_API virtual void updating() throw ( ::fwTools::Failed );

private:

    /// Slot: update the image in the craniocaudal direction
    void updateCranioCaudalImage(int i);

    /// Slot: update the image in the radial direction
    void updateRadialImage(int j);

    /// Slot: Common function used to update the image
    void updateBoth(int i, int j);

    /// Key of the mesh or the model series which contains it.
    std::string m_meshKey;

    /// Uid of the composite that contains the mesh.
    std::string m_compositeUID;

    /// Mesh
    ::fwData::Mesh::sptr m_mesh;

    /// Number of cells in each line of the mesh
    unsigned int m_nbCellsPerLine;

    /// Current craniocaudal index
    int m_cranioCaudalIndex;
    /// Current radial index
    int m_radialIndex;
};

} // echoEdSimu

#endif // __ECHOEDSIMU_SCOMPUTEMATRIXFROMCELL_HPP__

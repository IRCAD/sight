/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MATHS_SSWITCHMATRICES_HPP__
#define __MATHS_SSWITCHMATRICES_HPP__

#include "maths/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/helper/SigSlotConnection.hpp>
#include <fwServices/IController.hpp>


namespace maths
{

/**
 * @brief   This service switch between several matrices.
 *
 * Each time the slot s_SWITCH_SLOT is called the next matrix given in the configuration is copied on the matrix bound
 * to the service. Is is also possible to give the index of the matrix to use with the slot s_SWITCH_TO_SLOT.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::maths::SSwitchMatrices" autoConnect="yes">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D]: List of two matrices keys to switch.
 * @subsection In-Out In-Out:
 * - \b output [::fwData::TransformationMatrix3D]: Output matrix.
 *
 */
class MATHS_CLASS_API SSwitchMatrices : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SSwitchMatrices)(::fwServices::IController));

    /**
     * @brief Constructor.
     */
    MATHS_API SSwitchMatrices() throw ();

    /**
     * @brief Destructor.
     */
    virtual ~SSwitchMatrices() throw ()
    {
    }

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_SLOT;
    typedef ::fwCom::Slot< void () > SwitchSlotType;

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_TO_SLOT;
    typedef ::fwCom::Slot<void (size_t)> SwitchToSlotType;

protected:

    /// This method is used to configure the service.
    MATHS_API void configuring() throw (fwTools::Failed);

    /// This method is used to initialize the service.
    MATHS_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    MATHS_API void stopping() throw (fwTools::Failed);

    /// Does nothing.
    void swapping() throw (fwTools::Failed)
    {
    }

    /// Does nothing.
    MATHS_API void updating() throw (fwTools::Failed);

    /// Switch to next Matrix
    MATHS_API void switchMatrix() throw (fwTools::Failed);

    /// Switch to Matrix "index"
    MATHS_API void switchToMatrix(size_t index) throw (fwTools::Failed);


private:

    struct TransformMatrix
    {
        ::fwData::TransformationMatrix3D::sptr m_matrix;
        std::string m_uid;
    };

    typedef std::vector< TransformMatrix > MatrixVectorType;

    MatrixVectorType m_matrixVector; ///< The vector of TransformMatrix struct

    size_t m_indexOfDesiredMatrix;

    ::fwServices::helper::SigSlotConnection::sptr m_connections; ///< connection to matrices
};

} //namespace maths

#endif  // __MATHS_SSWITCHMATRICES_HPP__

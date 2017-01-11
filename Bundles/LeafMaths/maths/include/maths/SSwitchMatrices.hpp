/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MATHS_SSWITCHMATRICES_HPP__
#define __MATHS_SSWITCHMATRICES_HPP__

#include "maths/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>


namespace maths
{

/**
 * @brief   This service switch between several matrices.
 *
 * @section Slots Slots
 * - \b switchMatrix() : Each time the slot switchMatrix() is called the next matrix given in the configuration is copied on the matrix bound
 * to the service
 * - \b switchToMatrix(int index) : switch to the matrix at the given index
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

    size_t m_indexOfDesiredMatrix;
};

} //namespace maths

#endif  // __MATHS_SSWITCHMATRICES_HPP__

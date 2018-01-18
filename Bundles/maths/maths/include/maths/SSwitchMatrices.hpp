/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "maths/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>

namespace maths
{

/**
 * @brief   This service switch between several matrices.
 *
 * @section Slots Slots
 * - \b switchMatrix() : Each time the slot switchMatrix() is called the next matrix given in the configuration is
 * copied on the matrix bound
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

    fwCoreServiceClassDefinitionsMacro((SSwitchMatrices)(::fwServices::IController))

    /**
     * @brief Constructor.
     */
    MATHS_API SSwitchMatrices() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SSwitchMatrices() noexcept
    {
    }

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_SLOT;
    typedef ::fwCom::Slot< void () > SwitchSlotType;

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_TO_SLOT;
    typedef ::fwCom::Slot<void (size_t)> SwitchToSlotType;

protected:

    /// This method is used to configure the service.
    MATHS_API void configuring() override;

    /// This method is used to initialize the service.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Does nothing.
    MATHS_API void updating() override;

    /// Switch to next Matrix
    MATHS_API void switchMatrix();

    /// Switch to Matrix "index"
    MATHS_API void switchToMatrix(size_t index);

private:

    size_t m_indexOfDesiredMatrix;
};

} //namespace maths

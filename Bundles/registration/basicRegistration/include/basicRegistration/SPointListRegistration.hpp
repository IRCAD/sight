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

#include "basicRegistration/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwServices/IRegisterer.hpp>

namespace basicRegistration
{

/**
 * @brief   Register a point list against a reference point list. Each point in the list to register is matched
 *          with a point in the reference list according to their order or to their labels if they have any.
 *          The output is a transformation matrix.
 * @section Signals Signals
 * - \b errorComputed(double): emitted when registration error is computed.
 * @section Slots Slots
 * - \b changeMode(std::string): called when registration mode is changed.
 * - \b computeRegistration(::fwCore::HiResClock::HiResClockType timestamp): computes the registration.
 * Three modes are available : RIGID, SIMILARITY or AFFINE
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="::basicRegistration::SPointListRegistration">
           <inout key="registeredPL" uid="..." />
           <inout key="referencePL" uid="..." />
           <inout key="output" uid="..." />
           <config mode="similarity" />
       </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b registeredPL [::fwData::PointList]: List containing the points to register.
 * - \b referencePL [::fwData::PointList]: List containing the reference points to match with the points to register.
 * - \b output [::fwData::TransformationMatrix3D]: Transformation matrix between the source and target points.
 *
 * @subsection Configuration Configuration
 * - \b mode (optional, values=rigid|similarity|affine, default=rigid): registration mode.
 *
 */
class BASICREGISTRATION_CLASS_API SPointListRegistration : public ::fwServices::IRegisterer
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPointListRegistration)(::fwServices::IRegisterer) );

    /**
     * @name Slots API
     * @{
     */
    BASICREGISTRATION_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_MODE;
    ///@}

    /**
     * @name Signals API
     * @{
     */
    /// Emitted when registration error is computed
    typedef ::fwCom::Signal< void (double) > ErrorComputedSignalType;
    ///@}

    BASICREGISTRATION_API SPointListRegistration();

    BASICREGISTRATION_API virtual ~SPointListRegistration();

protected:

    /// Register a point list slot
    BASICREGISTRATION_API void computeRegistration(::fwCore::HiResClock::HiResClockType timestamp) override;

    /// Registration Mode (default: RIGID)
    typedef enum Mode
    {
        RIGID,      /*!< rigid mode of VTK registration */
        SIMILARITY, /*!< similarity mode of VTK registration */
        AFFINE      /*!< affine mode of VTK registration */
    }RegistrationModeType;

    /// Configures the service
    BASICREGISTRATION_API virtual void configuring() override;

    /// Do nothing
    BASICREGISTRATION_API virtual void starting() override;

    /// Do nothing
    BASICREGISTRATION_API virtual void stopping() override;

    /// Perform the registration
    BASICREGISTRATION_API virtual void updating() override;

    /// Do nothing
    BASICREGISTRATION_API virtual void swapping() override;

private:

    ///SLOT: changeMode
    void changeMode(std::string _value);

    /// Key source point list
    std::string m_registeredPointsKey;

    /// Key dest point list
    std::string m_referencePointsKey;

    /// Key for computed registration matrix
    std::string m_matrixKey;

    ///Registration Mode
    RegistrationModeType m_registrationMode;
};

} // namespace basicRegistration

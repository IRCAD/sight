/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/filter/point/config.hpp"

#include <core/com/Slot.hpp>

#include <service/IRegisterer.hpp>

namespace sight::module::filter::point
{

/**
 * @brief   Register a point list against a reference point list. Each point in the list to register is matched
 *          with a point in the reference list according to their order or to their labels if they have any.
 *          The output is a transformation matrix.
 * @section Signals Signals
 * - \b errorComputed(double): emitted when registration error is computed.
 * @section Slots Slots
 * - \b changeMode(std::string): called when registration mode is changed.
 * - \b computeRegistration(core::HiResClock::HiResClockType timestamp): computes the registration.
 * Three modes are available : RIGID, SIMILARITY or AFFINE
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::filter::point::SPointListRegistration">
           <inout key="registeredPL" uid="..." />
           <inout key="referencePL" uid="..." />
           <inout key="output" uid="..." />
           <config mode="similarity" />
       </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b registeredPL [sight::data::PointList]: List containing the points to register.
 * - \b referencePL [sight::data::PointList]: List containing the reference points to match with the points to register.
 * - \b output [sight::data::Matrix4]: Transformation matrix between the source and target points.
 *
 * @subsection Configuration Configuration
 * - \b mode (optional, values=rigid|similarity|affine, default=rigid): registration mode.
 *
 */
class MODULE_FILTER_POINT_CLASS_API SPointListRegistration : public service::IRegisterer
{

public:

    SIGHT_DECLARE_SERVICE(SPointListRegistration, service::IRegisterer)

    /**
     * @name Slots API
     * @{
     */
    MODULE_FILTER_POINT_API static const core::com::Slots::SlotKeyType s_CHANGE_MODE;
    ///@}

    /**
     * @name Signals API
     * @{
     */
    /// Emitted when registration error is computed
    typedef core::com::Signal< void (double) > ErrorComputedSignalType;
    ///@}

    MODULE_FILTER_POINT_API SPointListRegistration();

    MODULE_FILTER_POINT_API virtual ~SPointListRegistration();

protected:

    /// Register a point list slot
    MODULE_FILTER_POINT_API void computeRegistration(core::HiResClock::HiResClockType timestamp) override;

    /// Registration Mode (default: RIGID)
    typedef enum Mode
    {
        RIGID,      /*!< rigid mode of VTK registration */
        SIMILARITY, /*!< similarity mode of VTK registration */
        AFFINE      /*!< affine mode of VTK registration */
    }RegistrationModeType;

    /// Configures the service
    MODULE_FILTER_POINT_API virtual void configuring() override;

    /// Do nothing
    MODULE_FILTER_POINT_API virtual void starting() override;

    /// Do nothing
    MODULE_FILTER_POINT_API virtual void stopping() override;

    /// Perform the registration
    MODULE_FILTER_POINT_API virtual void updating() override;

    /// Do nothing
    MODULE_FILTER_POINT_API virtual void swapping() override;

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

} // namespace sight::module::filter::point

/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "maths/config.hpp"

#include <data/Landmarks.hpp>

#include <services/IService.hpp>

namespace maths
{

/**
 * @brief  Compute the vector between a point and a target
 *
 * @section Signals Signals
 * - \b lengthChanged(float): Emitted when the length between a point and a target changed
 * - \b lengthChangedStr(string): Emitted when the length between a point and a target changed
 * - \b sameSlice(string): Emitted on update to notify if point and target landmarks have the same Z coordinate
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::maths::SPointToLandmarkVector">
            <in key="landmark" uid="..." />
            <inout key="transform" uid="..." />
            <inout key="computedLandmark" uid="..." />
            <inout key="translationMatrix" uid="..." />
            <originLabel>point</originLabel>
            <endLabel>target</endLabel>
            <computedLandmarkLabel>computedLandmark</computedLandmarkLabel>
            <sameAxialSliceTolerance>0,001</sameAxialSliceTolerance>
            <sameAxialSliceLabel>Same axial slice</sameAxialSliceLabel>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b landmark [data::Landmarks]: landmarks representing a point and target
 *
 * @subsection InOut InOut:
 * - \b transform [data::TransformationMatrix3D]: transform containing the computed vector (in the
 * translation part)
 * - \b computedLandmark [data::Landmarks]: computed landmark
 * - \b translationMatrix [data::TransformationMatrix3D]: translation matrix
 *
 * @subsection Configuration Configuration:
 * - \b originLabel: (std::string) label of the landmark group corresponding to the origin of the vector
 * - \b endLabel: (std::string) label of the landmark group corresponding to the end of the vector
 * - \b computedLandmarkLabel: (std::string) computed landmark group name containing the target point.
 * - \b sameAxialSliceTolerance: (optional) (double, default=0,001) tolerance in millimeter to check if two landmarks
 * are on the same axial slice (i.e. have the same Z coordinates +/- the tolerance).
 * - \b sameAxialSliceLabel: (optional) (std::string, default="Same axial slice") label to prefix the message sent in
 * the sameSlice signal.
 */
class MATHS_CLASS_API SPointToLandmarkVector final : public services::IService
{

public:

    /// Generates default methods like New, dynamicCast, ...
    fwCoreServiceMacro(SPointToLandmarkVector, services::IService)

    /// Initializes signals.
    MATHS_API SPointToLandmarkVector() noexcept;

    /// Destroys the service.
    MATHS_API ~SPointToLandmarkVector() noexcept override;

protected:

    /// Gets parameters from composite.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Computes the vector between a target and a point.
    MATHS_API void updating() override;

    /// Configures the service's parameters.
    MATHS_API void configuring() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Landmarks::s_POINT_ADDED_SIG of s_LANDMARK_INPUT to s_UPDATE_SLOT
     */
    MATHS_API KeyConnectionsMap getAutoConnections() const override;

private:

    typedef core::com::Signal< void (float) > LengthChangedSignalType;
    typedef core::com::Signal< void (std::string) > LengthStrChangedSignalType;
    typedef core::com::Signal< void (std::string) > SameSliceSignalType;

    /// landmarks group's label for the origin of the vector.
    std::string m_originLabel{""};

    /// landmarks group's label for the end of the vector.
    std::string m_endLabel{""};

    /// Landmark containing the target point.
    data::Landmarks::sptr m_computedLandmark;

    /// Computed landmark group name.
    std::string m_groupLabel{""};

    /// Tolerance in mm when checking if the two landmarks are on the same axial slice.
    double m_tolerance{0.001};

    /// Label to prefix the message sent in the sameSlice signal.
    std::string m_sameSliceLabel{"Same axial slice"};
};
} // namespace maths

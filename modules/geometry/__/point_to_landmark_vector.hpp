/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include <data/landmarks.hpp>
#include <data/matrix4.hpp>

#include <service/base.hpp>

namespace sight::module::geometry
{

/**
 * @brief  Computes the vector between a point and a target
 *
 * @section Signals Signals
 * - \b lengthChanged(float): Emitted when the length between a point and a target changed
 * - \b lengthChangedStr(string): Emitted when the length between a point and a target changed
 * - \b sameSlice(string): Emitted on update to notify if point and target landmarks have the same Z coordinate
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::geometry::point_to_landmark_vector">
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
 * - \b landmark [sight::data::landmarks]: landmarks representing a point and target
 *
 * @subsection InOut InOut:
 * - \b transform [sight::data::matrix4]: transform containing the computed vector (in the
 * translation part)
 * - \b computedLandmark [sight::data::landmarks]: computed landmark
 * - \b translationMatrix [sight::data::matrix4]: translation matrix
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
class point_to_landmark_vector final : public service::base
{
public:

    /// Generates default methods like New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(point_to_landmark_vector, service::base);

    /// Initializes signals.
    point_to_landmark_vector() noexcept;

    /// Destroys the service.
    ~point_to_landmark_vector() noexcept override;

protected:

    /// Gets parameters from composite.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Computes the vector between a target and a point.
    void updating() override;

    /// Configures the service's parameters.
    void configuring() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::landmarks::POINT_ADDED_SIG of s_LANDMARK_INPUT to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    using length_changed_signal_t     = core::com::signal<void (float)>;
    using length_str_changed_signal_t = core::com::signal<void (std::string)>;
    using same_slice_signal_t         = core::com::signal<void (std::string)>;

    /// landmarks group's label for the origin of the vector.
    std::string m_origin_label;

    /// landmarks group's label for the end of the vector.
    std::string m_end_label;

    /// Computed landmark group name.
    std::string m_group_label;

    /// Tolerance in mm when checking if the two landmarks are on the same axial slice.
    double m_tolerance {0.001};

    /// Label to prefix the message sent in the sameSlice signal.
    std::string m_same_slice_label {"Same axial slice"};

    static constexpr std::string_view LANDMARK_INPUT = "landmark";
    data::ptr<data::landmarks, sight::data::access::in> m_landmark {this, LANDMARK_INPUT, true};
    data::ptr<data::matrix4, sight::data::access::inout> m_transform {this, "transform"};
    data::ptr<data::landmarks, sight::data::access::inout> m_computed_landmark {this, "computedLandmark"};
    data::ptr<data::matrix4, sight::data::access::inout> m_translation_matrix {this, "translationMatrix"};
};

} // namespace sight::module::geometry

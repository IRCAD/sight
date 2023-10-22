/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/geometry/__/config.hpp"

#include <data/landmarks.hpp>
#include <data/matrix4.hpp>
#include <data/string.hpp>

#include <service/base.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace sight::module::geometry
{

/**
 * @brief Computes the intersection between a mesh and a line.
 *
 * @section Signals Signals
 * - \b distanceChanged(float): Emitted when the distance between the point and the current landmark
 *  is modified
 *
 * @section Slots Slots
 * - \b updateSelectedPoint(std::string , std::size_t ): update the transformation matrix of the point
 * to the landmark when a landmark is selected
 * - \b updatePoint(std::string ): update the transformation matrix of the point
 * to the landmark when a landmark is added
 * - \b removePoint( ): update the transformation matrix when a point is removed
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::geometry::point_to_landmark_distance">
       <in key="pointMatrix" uid="..." />
       <in key="landmark" uid="..." />
       <inout key="pointToLandmarkMatrix" uid="..." />
       <inout key="distanceText" uid="..." />
       <config unit="mm" precision="6"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b pointMatrix [sight::data::matrix4]: point transformation matrix.
 * - \b landmark [sight::data::landmarks]: landmark.
 *
 * @subsection In-Out In-Out
 * - \b pointToLandmarkMatrix [sight::data::matrix4]: point to landmark transformation matrix
 * - \b distanceText [sight::data::string]: string containing the distance
 *
 * @subsection Configuration Configuration
 * - \b config(optional): contains the service configuration
 *    - \b unit: unit of the distance computed by this service
 *    - \b precision: precision of the displayed distance
 */
class MODULE_GEOMETRY_CLASS_API point_to_landmark_distance final : public service::base
{
public:

    /// Generates default methods like New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(point_to_landmark_distance, service::base);

    /// Initializes signals and slots.
    MODULE_GEOMETRY_API point_to_landmark_distance() noexcept;

    /// Destroys the service.
    MODULE_GEOMETRY_API ~point_to_landmark_distance() noexcept override;

protected:

    /// Does nothing.
    MODULE_GEOMETRY_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_API void stopping() override;

    /// Generates and updates the mesh.
    MODULE_GEOMETRY_API void updating() override;

    /// Configures the service's parameters.
    MODULE_GEOMETRY_API void configuring() override;

private:

    typedef core::com::signal<void (float)> distance_changed_signal_t;

    /// SLOT: updates selected point.
    void updateSelectedPoint(std::string _name, std::size_t _index);

    /// SLOT: updates added point.
    void updatePoint(std::string _name);

    /// SLOT: updates removed point.
    void removePoint();

    /// Selected landmark.
    glm::dvec3 m_currentLandmark {};

    /// Bool showing if a landmark is selected.
    bool m_landmarkSelected {false};

    /// Unit to concatenate with the computed result.
    std::string m_unit;

    /// Precision of the displayed distance.
    int m_precision {6};

    data::ptr<data::matrix4, sight::data::Access::in> m_pointMatrix {this, "pointMatrix"};
    data::ptr<data::landmarks, sight::data::Access::in> m_landmark {this, "landmark"};
    data::ptr<data::matrix4, sight::data::Access::inout> m_pointToLandmarkMatrix {this, "pointToLandmarkMatrix"};
    data::ptr<data::string, sight::data::Access::inout> m_distanceText {this, "distanceText"};
};

} // namespace sight::module::geometry

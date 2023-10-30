/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "modules/geometry/__/config.hpp"

#include <data/point.hpp>
#include <data/tools/picking_info.hpp>

#include <geometry/data/matrix4.hpp>
#include <geometry/data/point_list.hpp>

#include <service/controller.hpp>

namespace sight::module::geometry
{

/**
 * @brief Add point in a data::point_list
 *
 * @section Slots Slots
 * - \b pick(data::tools::picking_info): Add or remove the closest point to the picking position, actions occur when
 * `CTRL` is pressed.
 * - \b clearPoints(): Remove all points from the point lists.

 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::manage_point_list">
            <inout key="pointList" uid="..." />
            <in key="matrix" uid="..." />
            <config max="0" removable="true" label="false" tolerance="10.0" />
       </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b pointList [sight::data::point_list]: Target point list.
 *
 * @subsection Input Input:
 * - \b matrix [sight::data::matrix4](optional): Transformation applied to picked positions.
 *
 * @subsection Configuration Configuration:
 * - \b max (optional, std::size_t, default=0): set the maximum number of points contained in the point list, if it's 0,
 * the
 * capacity is set to the maximum. if the maximum is reached, the first point in the list will be removed.
 * - \b removable (optional, bool, default=true): allow points to be removed.
 * - \b label (optional, bool, default=false): add an ID label to added points.
 * - \b tolerance (optional, float, default=10.0): the tolerance used to remove a point.
 */
class MODULE_GEOMETRY_CLASS_API manage_point_list final : public service::controller
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(manage_point_list, sight::service::controller);

    /// Initialize slots.
    MODULE_GEOMETRY_API manage_point_list() noexcept;

    /// Destroys the service.
    MODULE_GEOMETRY_API ~manage_point_list() noexcept final;

private:

    /// Configures the service.
    void configuring() final;

    /// Does nothing.
    void starting() final;

    /// Does nothing.
    void updating() final;

    /// Does nothing.
    void stopping() final;

    /**
     * @brief SLOT: Add or remove a point from picking information.
     * @param _info picking information.
     */
    void pick(data::tools::picking_info _info) const;

    /**
     * @brief Adds a point in the point list
     * @param _point the point added to the list.
     */
    void add_point(const data::point::sptr _point) const;

    /**
     * @brief Removes a point from the point list.
     * @param _point the point removed from the list.
     */
    void remove_point(const data::point::csptr _point) const;

    /// SLOT: Clears the point list.
    void clear_points() const;

    /// Maximum number of contained points.
    std::size_t m_max {0};

    /// Allows points to be removed.
    bool m_removable {true};

    /// Allows to add an ID label the points.
    bool m_label {false};

    float m_tolerance {10.F};

    data::ptr<data::matrix4, sight::data::access::in> m_transform {this, "matrix", false, true};
    data::ptr<data::point_list, sight::data::access::inout> m_point_list {this, "pointList", false};
};

} // namespace sight::module::geometry

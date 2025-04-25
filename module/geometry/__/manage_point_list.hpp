/************************************************************************
 *
 * Copyright (C) 2019-2025 IRCAD France
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

#include <data/point.hpp>
#include <data/tools/picking_info.hpp>

#include <geometry/data/matrix4.hpp>
#include <geometry/data/point_list.hpp>

#include <service/controller.hpp>

namespace sight::module::geometry
{

/**
 * @brief Adds point in a data::point_list.
 * This can work in two different ways. Either by listening to an input transform and calling update(), either by
 * receiving a picking position.
 *
 * @section Slots Slots
 * - \b pick(data::tools::picking_info): Add or remove the closest point to the picking position.
 * - \b clear(): Remove all points from the point lists.

 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::manage_point_list">
            <in key="transform" uid="..." />
            <inout key="point_list" uid="..." />
            <config max="0" removable="true" label="false" tolerance="10.0" modifier="CTRL"/>
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b position [sight::data::matrix4](optional): source transform to listen.
 *
 * @subsection In-Out In-Out:
 * - \b point_list [sight::data::point_list]: Target point list.
 *
 * @subsection Input Input:
 * - \b transform [sight::data::matrix4](optional): Transformation applied to picked positions.
 *
 * @subsection Configuration Configuration:
 * - \b max (optional, std::size_t, default=0): set the maximum number of points contained in the point list, if it's 0,
 * the
 * capacity is set to the maximum. if the maximum is reached, the first point in the list will be removed.
 * - \b removable (optional, bool, default=true): allow points to be removed.
 * - \b label (optional, bool, default=false): add an ID label to added points.
 * - \b tolerance (optional, float, default=10.0): the tolerance used to remove a point.
 * - \b modifier (optional, string, default=""): the modifier to use when picking a point.
 */
class manage_point_list final : public service::controller
{
public:

    struct slots
    {
        static inline const core::com::slots::key_t PICK  = "pick";
        static inline const core::com::slots::key_t CLEAR = "clear";
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(manage_point_list, sight::service::controller);

    /// Initialize slots.
    manage_point_list() noexcept;

    /// Destroys the service.
    ~manage_point_list() noexcept final = default;

private:

    /// Configures the service.
    void configuring(const config_t& _config) final;

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
    void clear() const;

    /// Maximum number of contained points.
    std::size_t m_max {0};

    /// Allows points to be removed.
    bool m_removable {true};

    /// Allows to add an ID label the points.
    bool m_label {false};

    /// Maximum distance to add a new point
    float m_tolerance {10.F};

    /// Key modifier when picking a point
    data::tools::picking_info::modifier m_modifier {data::tools::picking_info::modifier::none};

    data::ptr<data::matrix4, sight::data::access::in> m_position {this, "position"};
    data::ptr<data::matrix4, sight::data::access::in> m_transform {this, "transform", true};
    data::ptr<data::point_list, sight::data::access::inout> m_point_list {this, "point_list"};
};

} // namespace sight::module::geometry

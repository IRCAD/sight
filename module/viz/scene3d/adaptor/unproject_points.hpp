/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/point_list.hpp>
#include <viz/scene3d/adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief Unproject a screen-space point-list into the related scene view-space.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="sight::module::viz::scene3d::adaptor::unproject_points">
            <data pointListIn="${...}" />
            <data pointListOut="${...}" />
            <config layer="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b pointListIn [sight::data::point_list](mandatory) : PointList to unproject (x, y, z).
 *
 * @subsection Output Output
 * - \b pointListOut [sight::data::point_list](mandatory) : The projected PointList; it's in view-space; have float
 * coordinates;
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : layer where the un-projection will be done.
 */
class unproject_points final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(unproject_points, sight::viz::scene3d::adaptor);

    /// Creates the adaptor.
    unproject_points() noexcept = default;

    /// Destroys the adaptor.
    ~unproject_points() noexcept final = default;

protected:

    /// Configures parameters.
    void configuring() final;

    /// Calls adaptor::initialize() and then call updating().
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connects data::signals::MODIFIED of s_POINTLIST_INPUT to service::slots::UPDATE
     */
    sight::service::connections_t auto_connections() const final;

    /// Un-projects the points.
    void updating() final;

    /// Does nothing.
    void stopping() final;

private:

    static constexpr std::string_view POINTLIST_IN  = "data.pointListIn";
    static constexpr std::string_view POINTLIST_OUT = "data.pointListOut";

    sight::data::ptr<sight::data::point_list, sight::data::access::in> m_point_list_in {this, POINTLIST_IN};
    sight::data::ptr<sight::data::point_list, sight::data::access::out> m_point_list_out_lock {this, POINTLIST_OUT};
};

} //namespace sight::module::viz::scene3d::adaptor

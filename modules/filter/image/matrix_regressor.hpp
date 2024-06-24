/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <core/base.hpp>

#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/vector.hpp>

#include <filter/image/matrix_regressor.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service computing the matrix that best fits a list of matrices.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent when the 'average' matrix is computed.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::image::matrix_regressor">
       <in key="matrixList" uid="..." auto_connect="true" />
       <in key="pointList" uid="..." />
       <inout key="optimalMatrix" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b matrixList [sight::data::vector]: The list of matrices used to compute an optimal one.
 * - \b pointList [sight::data::point_list]: A list of relevant points used to evaluate the distance to the optimal
 * matrix.
 *
 * @subsection In-Out In-Out
 * - \b optimalMatrix [sight::data::matrix4]: The optimal matrix.
 */
class matrix_regressor : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(matrix_regressor, service::filter);

    /// Does nothing.
    matrix_regressor();

    /// Destroys the service.
    ~matrix_regressor() override = default;

protected:

    /// Does nothing.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Compute the optimal matrix.
    void updating() override;

    /// Does nothing.
    void stopping() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::vector::ADDED_OBJECTS_SIG of s_MATRIX_LIST_IN to service::slots::UPDATE
     * Connect data::vector::REMOVED_OBJECTS_SIG of s_MATRIX_LIST_IN to service::slots::UPDATE
     * Connect data::vector::MODIFIED_SIG of s_MATRIX_LIST_IN to service::slots::UPDATE
     * Connect data::point_list::POINT_ADDED_SIG of s_POINT_LIST_IN to service::slots::UPDATE
     * Connect data::point_list::POINT_REMOVED_SIG of s_POINT_LIST_IN to service::slots::UPDATE
     * Connect data::point_list::MODIFIED_SIG of s_POINT_LIST_IN to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    static constexpr std::string_view MATRIX_LIST_IN       = "matrixList";
    static constexpr std::string_view POINT_LIST_IN        = "pointList";
    static constexpr std::string_view OPTIMAL_MATRIX_INOUT = "optimalMatrix";

    sight::data::ptr<sight::data::vector, sight::data::access::in> m_matrix_list {this, MATRIX_LIST_IN};
    sight::data::ptr<sight::data::point_list, sight::data::access::in> m_point_list {this, POINT_LIST_IN};
    sight::data::ptr<sight::data::matrix4, sight::data::access::inout> m_optimal_matrix {this, OPTIMAL_MATRIX_INOUT};
};

} // namespace sight::module::filter::image.

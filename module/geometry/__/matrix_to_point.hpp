/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <data/matrix4.hpp>
#include <data/point.hpp>
#include <data/string.hpp>

#include <service/filter.hpp>

namespace sight::module::geometry
{

/**
 * @brief Extracts translation part of a matrix and put it into a point.
 * The pointlist is cleared by default. It can be set to 'appending' mode through the configuration.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::geometry::matrix_to_point">
            <in key="matrix" uid="..." />
            <inout key="point" uid="..." />
            <properties label="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b matrices [sight::data::matrix4]: input matrices (only translation part will be extracted).
 *
 * @subsection Inout Inout
 * - \b point [sight::data::point]: output point
 *
 * @subsection Property Property
 * - \b label [sight::data::string]: label of the point.
 */
class matrix_to_point final : public sight::service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(matrix_to_point, sight::service::filter);

    /// Initializes a member variable
    matrix_to_point();

    /// Destroys the service
    ~matrix_to_point() final = default;

protected:

    /// Sets the append mode on or off.
    void configuring() final;

    /// Does nothing.
    void starting() final;

    /// Fills the PointList.
    void updating() final;

    /// Does nothing.
    void stopping() final;

private:

    data::ptr<data::matrix4, data::access::in> m_matrix {this, "matrix"};
    data::ptr<data::point, data::access::inout> m_point {this, "point"};
    data::property<data::string> m_label {this, "label", std::string()};
};

} // namespace sight::module::geometry.

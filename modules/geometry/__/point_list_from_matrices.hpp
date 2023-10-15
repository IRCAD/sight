/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/geometry/__/config.hpp"

#include <data/matrix4.hpp>
#include <data/point_list.hpp>

#include <service/filter.hpp>

namespace sight::module::geometry
{

/**
 * @brief Extracts translation part of multiple matrices and put it into a pointlist.
 * The pointlist is cleared by default. It can be set to 'appending' mode through the configuration.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::geometry::point_list_from_matrices">
            <in group="matrices">
                <key uid="matrix1" />
                <key uid="matrix2" />
                <key uid="matrix3" />
            </in>
            <inout key="pointList"/>
            <config append="false"/>
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b matrices [sight::data::matrix4]: input matrices (only translation part will be extracted).
 *
 * @subsection Inout Inout
 * - \b pointList [sight::data::point_list]: output pointlist, each point represent translation part of corresponding
 * matrix
 *
 * @subsection Configuration Configuration
 * - \b append (optional, default=false): does the service append points in the pointlist on update (true) or clear it
 * (false)?
 */
class MODULE_GEOMETRY_CLASS_API point_list_from_matrices final : public sight::service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(point_list_from_matrices, sight::service::filter);

    /// Initializes a member variable
    MODULE_GEOMETRY_API point_list_from_matrices();

    /// Destroys the service
    MODULE_GEOMETRY_API ~point_list_from_matrices() override;

protected:

    /// Sets the append mode on or off.
    MODULE_GEOMETRY_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_API void starting() override;

    /// Fills the PointList.
    MODULE_GEOMETRY_API void updating() override;

    /// Does nothing.
    MODULE_GEOMETRY_API void stopping() override;

private:

    /// Append mode
    bool m_append {false};

    data::ptr_vector<data::matrix4, data::Access::in> m_matrices {this, "matrices"};
    data::ptr<data::point_list, data::Access::inout> m_pointList {this, "pointList"};
};

} // namespace sight::module::geometry.

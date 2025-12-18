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
#include <data/mesh.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::mesh
{

/**
 * @brief Service computing the center of the axis-aligned bounding box of a mesh.
 *        Outputs a transform containing a translation from world origin to the bbox center.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after computing center.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::mesh::center">
       <in key="mesh" uid="..." />
       <inout key="transform" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b mesh [sight::data::mesh]: Input mesh from which the bbox center will be computed.
 *
 * @subsection In-Out In-Out
 * - \b transform [sight::data::matrix4]: Will be reset to identity then contain translation
 *   from world origin (0,0,0) to the mesh axis-aligned bounding box center (in the mesh coordinates).
 */
class center : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(center, service::filter);

    center();
    ~center() override = default;

protected:

    void configuring() override;
    void starting() override;
    void updating() override;
    void stopping() override;

    connections_t auto_connections() const override;

private:

    sight::data::ptr<sight::data::mesh, sight::data::access::inout> m_mesh {this, "mesh"};
    sight::data::ptr<sight::data::matrix4, sight::data::access::inout> m_transform {this, "transform"};
};

} // namespace sight::module::filter::mesh

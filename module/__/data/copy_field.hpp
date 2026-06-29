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

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief This service copies only the fields of an object and updates its own object or the defined target.
 * The fields are copied when updating.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::data::copy_field" >
           <in key="source" uid="..." />
           <inout key="target" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b source [sight::data::object]: define the source object whose fields to copy.
 *
 * @subsection In-Out In-Out
 * - \b target [sight::data::object]: define the target object to update.
 */
class copy_field final : public service::controller
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(copy_field, service::controller);

    /// Creates the service.
    copy_field() = default;

    /// Destroys the service.
    ~copy_field() final = default;

protected:

    /// Defines the connections to automatically update the copy when source or target fields are changed.
    connections_t auto_connections() const final;

    /// Does nothing at start.
    void starting() final;

    /// Copies the fields from source to target.
    void updating() final;

    /// Sets the output to null.
    void stopping() final;

private:

    sight::data::ptr<sight::data::object, sight::data::access::in> m_source {this, "source"};
    sight::data::ptr<sight::data::object, sight::data::access::inout> m_target {this, "target", true};
};

} // namespace sight::module::data

/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
 * @brief This service copies an object and updates its own object or the defined target.
 * It can either copy the data when starting or when updating (default).
 * It can also copy collections of objects in batch using the "sources" and "targets" groups.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::data::copy" >
           <in key="source" uid="..." />
           <inout key="target" uid="..." />
           <mode>copyOnStart</mode>
       </service>

       <service uid="..." type="sight::module::data::copy" >
           <in key="source" uid="..." />
           <out key="outTarget" uid="..." />
           <mode>copyOnStart</mode>
       </service>

       <service uid="..." type="sight::module::data::copy" >
           <in group="sources">
              <key uid="..." />
              <key uid="..." />
           </in>
           <inout group="targets">
              <key uid="..." />
              <key uid="..." />
           </inout>
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b source [sight::data::object]: define the source object to copy (singular).
 * - \b sources [sight::data::object]: define the source objects to copy (batch mode).
 *
 * @subsection In-Out In-Out
 * - \b target [sight::data::object]: define the target object to update, can't be used with Output (singular).
 * - \b targets [sight::data::object]: define the target objects to update (batch mode).
 *
 * @subsection Output Output
 * - \b outTarget [sight::data::object]: define the target object to create, can't be used with In-Out (singular).
 *
 * @subsection Configuration Configuration
 * - \b mode (optional) : The service can copy the data either when starting ("copyOnStart") or when
 * updating ("copyOnUpdate" - default).
 */
class copy final : public service::controller
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(copy, service::controller);

    /// Creates the service.
    copy() = default;

    /// Destroys the service.
    ~copy() final = default;

protected:

    //// Configures the service
    void configuring() final;

    /// Calls copy() if the mode if START.
    void starting() final;

    /// Calls copy() if the mode if UPDATE.
    void updating() final;

    /// Sets the output to null.
    void stopping() final;

private:

    enum class mode_t : std::int8_t
    {
        start,
        update
    };

    /// Copies the object to the output (single mode).
    void make_copy();

    /// Copies collections of objects in batch (group mode).
    void make_copy_batch();

    /// Determines when the data is copied (start or update)
    mode_t m_mode {mode_t::update};

    // Singular mode
    sight::data::ptr<sight::data::object, sight::data::access::in> m_source {this, "source"};
    sight::data::ptr<sight::data::object, sight::data::access::inout> m_target {this, "target", true};
    sight::data::ptr<sight::data::object, sight::data::access::out> m_out_target {this, "outTarget"};

    // Batch mode (groups)
    sight::data::ptr_vector<sight::data::object, sight::data::access::in> m_sources {this, "sources"};
    sight::data::ptr_vector<sight::data::object, sight::data::access::inout> m_targets {this, "targets"};
};

} // namespace sight::module::data.

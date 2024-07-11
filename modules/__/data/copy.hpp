/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
   @endcode
 *
 * @subsection Input Input
 * - \b source [sight::data::object]: define the source object to copy.
 *
 * @subsection In-Out In-Out
 * - \b target [sight::data::object]: define the target object to update, can't be used with Output.
 *
 * @subsection Output Output
 * - \b target [sight::data::object]: define the target object to create, can't be used with In-Out.
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

    /// Copies the object to the output.
    void make_copy();

    /// Determines when the data is copied (start or update)
    mode_t m_mode {mode_t::update};

    sight::data::ptr<sight::data::object, sight::data::access::in> m_source {this, "source"};
    sight::data::ptr<sight::data::object, sight::data::access::inout> m_target {this, "target", false, true};
    sight::data::ptr<sight::data::object, sight::data::access::out> m_out_target {this, "outTarget", false};
};

} // namespace sight::module::data.

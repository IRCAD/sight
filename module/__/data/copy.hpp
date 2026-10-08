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
 * @brief This service copies one or several objects into the defined targets.
 * It can either copy the data when starting or when updating (default).
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::data::copy" >
           <data source="..." target="..." />
           <data source="..." target="..." />
           <data source="..." target="..." />
           <mode>copyOnStart</mode>
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data.source [sight::data::object]: define the source objects to copy.
 *
 * @subsection In-Out In-Out
 * - \b data.target [sight::data::object]: define the target objects to update.
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

    /// Does nothing.
    void stopping() final;

private:

    enum class mode_t : std::int8_t
    {
        start,
        update
    };

    /// Copies each source into the target of the same index.
    void make_copy();

    /// Determines when the data is copied (start or update)
    mode_t m_mode {mode_t::update};

    sight::data::ptr_vector<sight::data::object, sight::data::access::in> m_sources {this, "data.source"};
    sight::data::ptr_vector<sight::data::object, sight::data::access::inout> m_targets {this, "data.target"};
};

} // namespace sight::module::data.

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

#include <data/activity_set.hpp>

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief This service is used to extract objects from a activity_set.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::get_activity_object" >
           <in key="activity_set" uid="..."/>
           <id>...</id>
           <out group="objects">
               <key name="requirement1" uid="..."/>
               <key name="requirement2" uid="..."/>
           </out>
        </service>
       @endcode
 * @subsection Input Input
 * - \b activity_set [sight::data::activity_set]: activity_set from which the series extracted.
 *
 * @subsection Output Output
 * - \b series [sight::data::object]: output object extracted from the activity_set.
 */
class get_activity_object final : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(get_activity_object, service::controller);

    /// Constructor
    get_activity_object() = default;

    /// Destructor
    ~get_activity_object() final = default;

protected:

/// Configure the service
    void configuring(const service::config_t& _config) final;

    /// Does nothing
    void starting() final;

    /// Does nothing
    void stopping() final;

    /// Extract the object(s)
    void updating() final;

    /// Updates when activity set is modified
    service::connections_t auto_connections() const final;

private:

    std::string m_id;
    std::vector<std::string> m_keys_names;
    sight::data::ptr<sight::data::activity_set, sight::data::access::in> m_activity_set {this, "activity_set"};
    sight::data::ptr_vector<sight::data::object, sight::data::access::out> m_objects {this, "objects"};
};

} // namespace sight::module::data

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

#include <sight/ui/test/config.hpp>

#include <data/object.hpp>

#include <service/base.hpp>
#include <service/registry.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace sight::ui::test::detail
{

/// A name of the application XML with its optional locator scope: `axial_scene_view >> image`.
struct scoped_uid
{
    /// The widgets the configuration of the name is launched in, in locator syntax. Empty for no scope.
    std::string scope;
    std::string uid;
};

/// Splits at the last `>>`: the name of an object or a service has no widget of its own to locate.
[[nodiscard]] SIGHT_UI_TEST_API scoped_uid parse_scoped_uid(std::string_view _expression);

/// The running services bearing this uid, in the configurations launched inside the scope. Call on the GUI thread.
[[nodiscard]] SIGHT_UI_TEST_API std::vector<sight::service::base::sptr> find_services(const scoped_uid& _name);

/// What the GUI thread tells of a name: its ids in the configurations launched inside the scope, the objects created
/// with these ids, and the running services, which may wait for a deferred object of these ids.
struct object_lookup
{
    std::vector<std::string> ids;
    std::vector<data::object::csptr> created;
    sight::service::service_vector_t services;
};

/// Looks a name up. Call on the GUI thread.
[[nodiscard]] SIGHT_UI_TEST_API object_lookup look_up_objects(const scoped_uid& _name);

/// The deferred objects these services bind to one of the ids. Call on the worker of the services, which alone changes
/// their data.
[[nodiscard]] SIGHT_UI_TEST_API std::vector<data::object::csptr> deferred_objects(
    const sight::service::service_vector_t& _services,
    const std::vector<std::string>& _ids
);

} // namespace sight::ui::test::detail

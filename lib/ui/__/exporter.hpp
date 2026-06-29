/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include <core/com/slots.hpp>

#include <data/object.hpp>

#include <ui/__/action.hpp>

namespace sight::ui
{

/// This is the base class for Export actions that allow to export a data to a container.
template<class C>
class exporter : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(exporter, sight::ui::action);

    struct slots
    {
        static inline const slot_key_t CHECK_ADDED_OBJECTS   = "checkAddedObjects";
        static inline const slot_key_t CHECK_REMOVED_OBJECTS = "checkRemovedObjects";
    };

protected:

    /// Constructor
    constexpr exporter();

    /// Destructor
    ~exporter() noexcept override = default;

    /// Connect data::container<C>::signals::ADDED_OBJECTS to this::CHECK_ADDED_OBJECTS.
    /// Connect data::container<C>::signals::REMOVED_OBJECTS to this::CHECK_REMOVED_OBJECTS.
    connections_t auto_connections() const override;

    /// This method is used to configure the service parameters
    void configuring() override;

    /// Starts service. If object exists in container, this action is not executable.
    void starting() override;

    /// Stops service. Does nothing.
    void stopping() override;

    /// Adds the object in the container.
    void updating() override;

    void info(std::ostream& _sstream) override;

    static constexpr std::string_view CONTAINER_INOUT = "container";

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    data::ptr<C, data::access::inout> m_container {this, CONTAINER_INOUT};

    using shared_pointer = C::value_type;
    data::ptr<typename shared_pointer::element_type, data::access::inout> m_data {this, "data"};
// NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

private:

    /// Slots
    /// @{
    /// Slot: check if specified object is added and set action not executable
    constexpr void check_added_objects(C::container_t _added_objects);

    /// Slot: check if specified object is removed and set action executable
    constexpr void check_removed_objects(C::container_t _removed_objects);
    /// @}
};

} // namespace sight::ui

#include "exporter.hxx"

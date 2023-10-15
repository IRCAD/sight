/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "ui/__/config.hpp"

#include <core/com/slots.hpp>

#include <data/object.hpp>

#include <ui/__/action.hpp>

namespace sight::ui
{

/// This is the base class for Export actions that allow to export a data to a container.
template<class C>
class MODULE_UI_CLASS_API exporter : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(exporter, sight::ui::action);

    /// Connect data::container<C>::ADDED_OBJECTS_SIG to this::CHECK_ADDED_OBJECTS_SLOT.
    /// Connect data::container<C>::REMOVED_OBJECTS_SIG to this::CHECK_REMOVED_OBJECTS_SLOT.
    connections_t auto_connections() const override;

protected:

    /// Constructor
    constexpr exporter();

    /// Destructor
    ~exporter() noexcept override = default;

    /// This method is used to configure the service parameters
    void configuring() override;

    /// Starts service. If object exists in container, this action is not executable.
    void starting() override;

    /// Stops service. Does nothing.
    void stopping() override;

    /// Adds the object in the container.
    void updating() override;

    void info(std::ostream& _sstream) override;

    static constexpr std::string_view s_CONTAINER_INOUT = "container";

    data::ptr<C, data::Access::inout> m_container {this, s_CONTAINER_INOUT};

    using shared_pointer = typename C::value_type;
    data::ptr<typename shared_pointer::element_type, data::Access::inout> m_data {this, "data"};

private:

    /// Slots
    /// @{
    inline static const core::com::slots::key_t CHECK_ADDED_OBJECTS_SLOT   = "checkAddedObjects";
    inline static const core::com::slots::key_t CHECK_REMOVED_OBJECTS_SLOT = "checkRemovedObjects";

    /// Slot: check if specified object is added and set action not executable
    constexpr void checkAddedObjects(typename C::container_type addedObjects);

    /// Slot: check if specified object is removed and set action executable
    constexpr void checkRemovedObjects(typename C::container_type removedObjects);
    /// @}
};

} // namespace sight::ui

#include "exporter.hxx"

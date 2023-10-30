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

#include "exporter.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <ui/__/dialog/input.hpp>

namespace sight::ui
{

template<class C>
constexpr exporter<C>::exporter()
{
    new_slot(exporter<C>::CHECK_ADDED_OBJECTS_SLOT, &exporter<C>::check_added_objects, this);
    new_slot(exporter<C>::CHECK_REMOVED_OBJECTS_SLOT, &exporter<C>::check_removed_objects, this);
}

//------------------------------------------------------------------------------
template<class C>
typename exporter<C>::connections_t exporter<C>::auto_connections() const
{
    return {
        {CONTAINER_INOUT, C::ADDED_OBJECTS_SIG, CHECK_ADDED_OBJECTS_SLOT},
        {CONTAINER_INOUT, C::REMOVED_OBJECTS_SIG, CHECK_REMOVED_OBJECTS_SLOT}
    };
}

//------------------------------------------------------------------------------

template<class C>
void exporter<C>::configuring()
{
    sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

template<class C>
void exporter<C>::starting()
{
    action_service_starting();

    auto container = m_container.lock();
    auto data      = m_data.lock();

    if(const auto& it = std::find(container->cbegin(), container->cend(), data.get_shared());
       it != container->cend())
    {
        set_enabled(false);
    }
}

//------------------------------------------------------------------------------

template<class C>
void exporter<C>::stopping()
{
    action_service_stopping();
}

//------------------------------------------------------------------------------

template<class C>
void exporter<C>::updating()
{
    auto data = m_data.lock();

    std::string description = data->get_description();

    const auto& [input, ok] = sight::ui::dialog::input::show_input_dialog(
        "exporter activity",
        "Enter description",
        description
    );

    if(ok)
    {
        data->set_description(input);

        {
            auto container            = m_container.lock();
            const auto scoped_emitter = container->scoped_emit();
            container->push_back(data.get_shared());
        }

        set_enabled(false);
    }
}

//------------------------------------------------------------------------------

template<class C>
void exporter<C>::info(std::ostream& _sstream)
{
    // Update message
    _sstream << std::string("exporter");
}

//------------------------------------------------------------------------------

template<class C>
constexpr void exporter<C>::check_added_objects(typename C::container_t _added_objects)
{
    auto data = m_data.lock();

    if(const auto& it = std::find(_added_objects.cbegin(), _added_objects.cend(), data.get_shared());
       it != _added_objects.cend())
    {
        set_enabled(false);
    }
}

//------------------------------------------------------------------------------

template<class C>
constexpr void exporter<C>::check_removed_objects(typename C::container_t _removed_objects)
{
    auto data = m_data.lock();

    if(const auto& it = std::find(_removed_objects.cbegin(), _removed_objects.cend(), data.get_shared());
       it != _removed_objects.cend())
    {
        set_enabled(true);
    }
}

} // namespace sight::ui

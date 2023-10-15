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
    new_slot(exporter<C>::CHECK_ADDED_OBJECTS_SLOT, &exporter<C>::checkAddedObjects, this);
    new_slot(exporter<C>::CHECK_REMOVED_OBJECTS_SLOT, &exporter<C>::checkRemovedObjects, this);
}

//------------------------------------------------------------------------------
template<class C>
typename exporter<C>::connections_t exporter<C>::auto_connections() const
{
    return {
        {s_CONTAINER_INOUT, C::ADDED_OBJECTS_SIG, CHECK_ADDED_OBJECTS_SLOT},
        {s_CONTAINER_INOUT, C::REMOVED_OBJECTS_SIG, CHECK_REMOVED_OBJECTS_SLOT}
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
    actionServiceStarting();

    auto container = m_container.lock();
    auto data      = m_data.lock();

    if(const auto& it = std::find(container->cbegin(), container->cend(), data.get_shared());
       it != container->cend())
    {
        setEnabled(false);
    }
}

//------------------------------------------------------------------------------

template<class C>
void exporter<C>::stopping()
{
    actionServiceStopping();
}

//------------------------------------------------------------------------------

template<class C>
void exporter<C>::updating()
{
    auto data = m_data.lock();

    std::string description = data->getDescription();

    const auto& [input, ok] = sight::ui::dialog::input::showInputDialog(
        "exporter activity",
        "Enter description",
        description
    );

    if(ok)
    {
        data->setDescription(input);

        {
            auto container            = m_container.lock();
            const auto scoped_emitter = container->scoped_emit();
            container->push_back(data.get_shared());
        }

        setEnabled(false);
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
constexpr void exporter<C>::checkAddedObjects(typename C::container_type addedObjects)
{
    auto data = m_data.lock();

    if(const auto& it = std::find(addedObjects.cbegin(), addedObjects.cend(), data.get_shared());
       it != addedObjects.cend())
    {
        setEnabled(false);
    }
}

//------------------------------------------------------------------------------

template<class C>
constexpr void exporter<C>::checkRemovedObjects(typename C::container_type removedObjects)
{
    auto data = m_data.lock();

    if(const auto& it = std::find(removedObjects.cbegin(), removedObjects.cend(), data.get_shared());
       it != removedObjects.cend())
    {
        setEnabled(true);
    }
}

} // namespace sight::ui

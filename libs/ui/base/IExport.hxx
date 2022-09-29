/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "IExport.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <ui/base/dialog/InputDialog.hpp>

namespace sight::ui::base
{

template<class C>
constexpr IExport<C>::IExport()
{
    newSlot(IExport<C>::s_CHECK_ADDED_OBJECTS_SLOT, &IExport<C>::checkAddedObjects, this);
    newSlot(IExport<C>::s_CHECK_REMOVED_OBJECTS_SLOT, &IExport<C>::checkRemovedObjects, this);
}

//------------------------------------------------------------------------------
template<class C>
typename IExport<C>::KeyConnectionsMap IExport<C>::getAutoConnections() const
{
    return {
        {s_CONTAINER_INOUT, C::s_ADDED_OBJECTS_SIG, s_CHECK_ADDED_OBJECTS_SLOT},
        {s_CONTAINER_INOUT, C::s_REMOVED_OBJECTS_SIG, s_CHECK_REMOVED_OBJECTS_SLOT}
    };
}

//------------------------------------------------------------------------------

template<class C>
void IExport<C>::configuring()
{
    sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

template<class C>
void IExport<C>::starting()
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
void IExport<C>::stopping()
{
    actionServiceStopping();
}

//------------------------------------------------------------------------------

template<class C>
void IExport<C>::updating()
{
    auto data = m_data.lock();

    std::string description = data->getDescription();

    description = sight::ui::base::dialog::InputDialog::showInputDialog(
        "Export activity",
        "Enter description",
        description
    );

    if(!description.empty())
    {
        data->setDescription(description);

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
void IExport<C>::info(std::ostream& _sstream)
{
    // Update message
    _sstream << std::string("IExport");
}

//------------------------------------------------------------------------------

template<class C>
constexpr void IExport<C>::checkAddedObjects(typename C::container_type addedObjects)
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
constexpr void IExport<C>::checkRemovedObjects(typename C::container_type removedObjects)
{
    auto data = m_data.lock();

    if(const auto& it = std::find(removedObjects.cbegin(), removedObjects.cend(), data.get_shared());
       it != removedObjects.cend())
    {
        setEnabled(true);
    }
}

} // namespace sight::ui::base

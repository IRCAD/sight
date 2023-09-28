/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/__/cursor.hpp"

#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

namespace sight::ui
{

//-----------------------------------------------------------------------------

cursor::cursor()
{
    ui::object::sptr guiObj = ui::factory::make(cursor_base::REGISTRY_KEY);
    m_implementation = std::dynamic_pointer_cast<ui::cursor_base>(guiObj);
}

//-----------------------------------------------------------------------------

void cursor::setCursor(ui::cursor_base::state_t cursor, bool setOverridenAsDefault)
{
    if(m_implementation)
    {
        // Copy shared pointer to keep the object alive during the call.
        auto cursor_implementation = m_implementation;

        // Go to main thread....
        core::thread::get_default_worker()->post_task<void>(
            [cursor_implementation, cursor, setOverridenAsDefault]()
            {
                cursor_implementation->setCursor(cursor, setOverridenAsDefault);
            });
    }
}

//-----------------------------------------------------------------------------

void cursor::setDefaultCursor()
{
    if(m_implementation)
    {
        // Copy shared pointer to keep the object alive during the call.
        auto cursor_implementation = m_implementation;

        // Go to main thread....
        core::thread::get_default_worker()->post_task<void>(
            [cursor_implementation]
            {
                cursor_implementation->setDefaultCursor();
            });
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui

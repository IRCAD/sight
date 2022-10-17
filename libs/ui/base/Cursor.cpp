/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/Cursor.hpp"

#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

namespace sight::ui::base
{

//-----------------------------------------------------------------------------

Cursor::Cursor()
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ICursor::REGISTRY_KEY);
    m_implementation = ui::base::ICursor::dynamicCast(guiObj);
}

//-----------------------------------------------------------------------------

Cursor::~Cursor()
= default;

//-----------------------------------------------------------------------------

void Cursor::setCursor(ui::base::ICursor::CursorType cursor)
{
    if(m_implementation)
    {
        // Copy shared pointer to keep the object alive during the call.
        auto cursor_implementation = m_implementation;

        // Go to main thread....
        core::thread::getDefaultWorker()->postTask<void>(
            [cursor_implementation, cursor]()
            {
                cursor_implementation->setCursor(cursor);
            });
    }
}

//-----------------------------------------------------------------------------

void Cursor::setDefaultCursor()
{
    if(m_implementation)
    {
        // Copy shared pointer to keep the object alive during the call.
        auto cursor_implementation = m_implementation;

        // Go to main thread....
        core::thread::getDefaultWorker()->postTask<void>(
            [cursor_implementation]
            {
                cursor_implementation->setDefaultCursor();
            });
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base

/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "quit.hpp"

#include <service/macros.hpp>

#include <ui/__/application.hpp>
#include <ui/__/cursor.hpp>

namespace sight::module::ui
{

//-----------------------------------------------------------------------------

quit::quit() noexcept =
    default;

//-----------------------------------------------------------------------------

quit::~quit() noexcept =
    default;

//-----------------------------------------------------------------------------

void quit::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void quit::starting()
{
    this->action_service_starting();
}

//-----------------------------------------------------------------------------

void quit::stopping()
{
    this->action_service_stopping();
}

//-----------------------------------------------------------------------------

void quit::info(std::ostream& _sstream)
{
    _sstream << "Quit action" << std::endl;
}

//-----------------------------------------------------------------------------

void quit::updating()
{
    sight::ui::cursor cursor;
    cursor.set_cursor(sight::ui::cursor_base::busy);
    sight::ui::application::get()->exit(0);
    cursor.set_default_cursor();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui

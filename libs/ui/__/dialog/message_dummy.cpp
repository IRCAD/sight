/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "message_dummy.hpp"

namespace sight::ui::dialog
{

std::queue<sight::ui::dialog::message_dummy::Buttons> message_dummy::actions;

//------------------------------------------------------------------------------

void message_dummy::setTitle(const std::string& /*title*/)
{
}

//------------------------------------------------------------------------------

void message_dummy::setMessage(const std::string& /*msg*/)
{
}

//------------------------------------------------------------------------------

void message_dummy::setIcon(Icons /*icon*/)
{
}

//------------------------------------------------------------------------------

void message_dummy::addButton(Buttons /*button*/)
{
}

//------------------------------------------------------------------------------

void message_dummy::setDefaultButton(Buttons /*button*/)
{
}

//------------------------------------------------------------------------------

void message_dummy::addCustomButton(const std::string& /*label*/, std::function<void()> /*clickedFn*/)
{
}

//------------------------------------------------------------------------------

void message_dummy::pushAction(Buttons _action)
{
    actions.push(_action);
}

//------------------------------------------------------------------------------

bool message_dummy::clear()
{
    if(actions.empty())
    {
        return true;
    }

    while(actions.empty())
    {
        actions.pop();
    }

    return false;
}

//------------------------------------------------------------------------------

message_dummy::Buttons message_dummy::show()
{
    message_dummy::Buttons res = NOBUTTON;
    if(!actions.empty())
    {
        res = actions.front();
        actions.pop();
    }

    return res;
}

} // namespace sight::ui::dialog

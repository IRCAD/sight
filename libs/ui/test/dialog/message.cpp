/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "message.hpp"

#include <ui/__/macros.hpp>

SIGHT_REGISTER_GUI(sight::ui::test::dialog::message, sight::ui::dialog::message_base::REGISTRY_KEY);

namespace sight::ui::test::dialog
{

std::queue<sight::ui::test::dialog::message::buttons> message::s_actions;

//------------------------------------------------------------------------------

void message::set_title(const std::string& /*title*/)
{
}

//------------------------------------------------------------------------------

void message::set_message(const std::string& /*msg*/)
{
}

//------------------------------------------------------------------------------

void message::set_icon(icons /*icon*/)
{
}

//------------------------------------------------------------------------------

void message::add_button(buttons /*button*/)
{
}

//------------------------------------------------------------------------------

void message::set_default_button(buttons /*button*/)
{
}

//------------------------------------------------------------------------------

void message::add_custom_button(const std::string& /*label*/, std::function<void()>/*clickedFn*/)
{
}

//------------------------------------------------------------------------------

void message::push_action(buttons _action)
{
    s_actions.push(_action);
}

//------------------------------------------------------------------------------

bool message::clear()
{
    if(s_actions.empty())
    {
        return true;
    }

    while(s_actions.empty())
    {
        s_actions.pop();
    }

    return false;
}

//------------------------------------------------------------------------------

message::buttons message::show()
{
    message::buttons res = nobutton;
    if(!s_actions.empty())
    {
        res = s_actions.front();
        s_actions.pop();
    }

    return res;
}

} // namespace sight::ui::test::dialog

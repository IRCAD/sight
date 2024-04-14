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

#include "input_dummy.hpp"

namespace sight::ui::dialog
{

std::queue<std::string> input_dummy::s_inputs;

//------------------------------------------------------------------------------

void input_dummy::set_title(const std::string& /*title*/)
{
}

//------------------------------------------------------------------------------

void input_dummy::set_message(const std::string& /*msg*/)
{
}

//------------------------------------------------------------------------------

void input_dummy::set_echo_mode(echo_mode /*echoMode*/)
{
}

//------------------------------------------------------------------------------

void input_dummy::set_input(const std::string& /*text*/)
{
}

//------------------------------------------------------------------------------

std::pair<std::string, bool> input_dummy::get_input()
{
    std::string res;
    if(!s_inputs.empty())
    {
        res = s_inputs.front();
        s_inputs.pop();
    }

    return {res, true};
}

//------------------------------------------------------------------------------

void input_dummy::push_input(const std::string& _input)
{
    s_inputs.push(_input);
}

//------------------------------------------------------------------------------

bool input_dummy::clear()
{
    if(s_inputs.empty())
    {
        return true;
    }

    while(!s_inputs.empty())
    {
        s_inputs.pop();
    }

    return false;
}

} // namespace sight::ui::dialog

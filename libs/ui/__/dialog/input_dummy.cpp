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

std::queue<std::string> input_dummy::inputs;

//------------------------------------------------------------------------------

void input_dummy::setTitle(const std::string& /*title*/)
{
}

//------------------------------------------------------------------------------

void input_dummy::setMessage(const std::string& /*msg*/)
{
}

//------------------------------------------------------------------------------

void input_dummy::setEchoMode(EchoMode /*echoMode*/)
{
}

//------------------------------------------------------------------------------

void input_dummy::set_input(const std::string& /*text*/)
{
}

//------------------------------------------------------------------------------

std::pair<std::string, bool> input_dummy::getInput()
{
    std::string res;
    if(!inputs.empty())
    {
        res = inputs.front();
        inputs.pop();
    }

    return {res, true};
}

//------------------------------------------------------------------------------

void input_dummy::pushInput(const std::string& _input)
{
    inputs.push(_input);
}

//------------------------------------------------------------------------------

bool input_dummy::clear()
{
    if(inputs.empty())
    {
        return true;
    }

    while(!inputs.empty())
    {
        inputs.pop();
    }

    return false;
}

} // namespace sight::ui::dialog

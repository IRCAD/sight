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

#include "DummyInputDialog.hpp"

namespace sight::ui::base::dialog
{

std::queue<std::string> DummyInputDialog::inputs;

DummyInputDialog::DummyInputDialog(sight::ui::base::GuiBaseObject::Key /*key*/)
{
}

//------------------------------------------------------------------------------

void DummyInputDialog::setTitle(const std::string& /*title*/)
{
}

//------------------------------------------------------------------------------

void DummyInputDialog::setMessage(const std::string& /*msg*/)
{
}

//------------------------------------------------------------------------------

void DummyInputDialog::setEchoMode(sight::ui::base::dialog::IInputDialog::EchoMode /*echoMode*/)
{
}

//------------------------------------------------------------------------------

void DummyInputDialog::setInput(const std::string& /*text*/)
{
}

//------------------------------------------------------------------------------

std::pair<std::string, bool> DummyInputDialog::getInput()
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

void DummyInputDialog::pushInput(const std::string& input)
{
    inputs.push(input);
}

} // namespace sight::ui::base::dialog

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

#include "DummyMessageDialog.hpp"

namespace sight::ui::base::dialog
{

std::queue<sight::ui::base::dialog::IMessageDialog::Buttons> DummyMessageDialog::actions;

DummyMessageDialog::DummyMessageDialog(sight::ui::base::factory::Key /*key*/)
{
}

//------------------------------------------------------------------------------

void DummyMessageDialog::setTitle(const std::string& /*title*/)
{
}

//------------------------------------------------------------------------------

void DummyMessageDialog::setMessage(const std::string& /*msg*/)
{
}

//------------------------------------------------------------------------------

void DummyMessageDialog::setIcon(Icons /*icon*/)
{
}

//------------------------------------------------------------------------------

void DummyMessageDialog::addButton(Buttons /*button*/)
{
}

//------------------------------------------------------------------------------

void DummyMessageDialog::setDefaultButton(Buttons /*button*/)
{
}

//------------------------------------------------------------------------------

void DummyMessageDialog::addCustomButton(const std::string& /*label*/, std::function<void()> /*clickedFn*/)
{
}

//------------------------------------------------------------------------------

void DummyMessageDialog::pushAction(sight::ui::base::dialog::IMessageDialog::Buttons action)
{
    actions.push(action);
}

//------------------------------------------------------------------------------

bool DummyMessageDialog::clear()
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

sight::ui::base::dialog::IMessageDialog::Buttons DummyMessageDialog::show()
{
    sight::ui::base::dialog::IMessageDialog::Buttons res = NOBUTTON;
    if(!actions.empty())
    {
        res = actions.front();
        actions.pop();
    }

    return res;
}

} // namespace sight::ui::base::dialog

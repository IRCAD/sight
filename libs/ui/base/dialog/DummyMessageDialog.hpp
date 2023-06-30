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

#pragma once

#include "IMessageDialog.hpp"

#include <queue>

namespace sight::ui::base::dialog
{

class UI_BASE_CLASS_API DummyMessageDialog : public IMessageDialog
{
public:

    UI_BASE_API explicit DummyMessageDialog(sight::ui::base::factory::Key key);

    void setTitle(const std::string& title) override;
    void setMessage(const std::string& msg) override;
    void setIcon(Icons icon) override;
    void addButton(Buttons button) override;
    void setDefaultButton(Buttons button) override;
    void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

    sight::ui::base::dialog::IMessageDialog::Buttons show() override;

    UI_BASE_API static void pushAction(sight::ui::base::dialog::IMessageDialog::Buttons action);
    UI_BASE_API static bool clear();

private:

    static std::queue<sight::ui::base::dialog::IMessageDialog::Buttons> actions;
};

}

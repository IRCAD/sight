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

#include "message_base.hpp"

#include <queue>

namespace sight::ui::dialog
{

class UI_CLASS_API message_dummy : public message_base
{
public:

    UI_API void setTitle(const std::string& title) override;
    UI_API void setMessage(const std::string& msg) override;
    UI_API void setIcon(Icons icon) override;
    UI_API void addButton(Buttons button) override;
    UI_API void setDefaultButton(Buttons button) override;
    UI_API void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

    UI_API Buttons show() override;

    UI_API static void pushAction(Buttons action);
    UI_API static bool clear();

private:

    static std::queue<Buttons> actions;
};

}

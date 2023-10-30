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

    UI_API void set_title(const std::string& _title) override;
    UI_API void set_message(const std::string& _msg) override;
    UI_API void set_icon(icons _icon) override;
    UI_API void add_button(buttons _button) override;
    UI_API void set_default_button(buttons _button) override;
    UI_API void add_custom_button(const std::string& _label, std::function<void()> _clicked_fn) override;

    UI_API buttons show() override;

    UI_API static void push_action(buttons _action);
    UI_API static bool clear();

private:

    static std::queue<buttons> s_actions;
};

}

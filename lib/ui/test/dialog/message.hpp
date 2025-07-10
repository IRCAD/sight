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

#pragma once

#include <sight/ui/test/config.hpp>

#include <ui/__/dialog/message_base.hpp>

#include <queue>

namespace sight::ui::test::dialog
{

class SIGHT_UI_TEST_CLASS_API message : public sight::ui::dialog::message_base
{
public:

    SIGHT_UI_TEST_API void set_title(const std::string& _title) override;
    SIGHT_UI_TEST_API void set_message(const std::string& _msg) override;
    SIGHT_UI_TEST_API void set_icon(icons _icon) override;
    SIGHT_UI_TEST_API void add_button(buttons _button) override;
    SIGHT_UI_TEST_API void set_default_button(buttons _button) override;
    SIGHT_UI_TEST_API void add_custom_button(const std::string& _label, std::function<void()> _clicked_fn) override;

    SIGHT_UI_TEST_API buttons show() override;

    SIGHT_UI_TEST_API static void push_action(buttons _action);
    SIGHT_UI_TEST_API static bool clear();

private:

    static std::queue<buttons> s_actions;
};

}

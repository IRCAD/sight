/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "input_base.hpp"

#include <queue>

namespace sight::ui::dialog
{

class SIGHT_UI_CLASS_API input_dummy : public sight::ui::dialog::input_base
{
public:

    SIGHT_DECLARE_CLASS(input_dummy, sight::ui::dialog::input_base);

    SIGHT_UI_API ~input_dummy() override = default;
    SIGHT_UI_API void set_title(const std::string& _title) override;
    SIGHT_UI_API void set_message(const std::string& _msg) override;
    SIGHT_UI_API void set_echo_mode(echo_mode _echo_mode) override;
    SIGHT_UI_API void set_input(const std::string& _text) override;

    SIGHT_UI_API std::pair<std::string, bool> get_input() override;

    SIGHT_UI_API static void push_input(const std::string& _input);

    SIGHT_UI_API static bool clear();

private:

    static std::queue<std::string> s_inputs;
};

}

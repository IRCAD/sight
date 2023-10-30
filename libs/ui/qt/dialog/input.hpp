/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ui/qt/config.hpp"

#include <core/base.hpp>

#include <ui/__/dialog/input_base.hpp>

#include <string>

namespace sight::ui::qt::dialog
{

/**
 * @brief   Defines an Input dialog.
 *
 * Example of use:
 * @code
   ui::dialog::input inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.set_input ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class UI_QT_CLASS_API input : public ui::dialog::input_base
{
public:

    SIGHT_DECLARE_CLASS(input, ui::dialog::input_base, ui::factory::make<input>);

    UI_QT_API ~input() override = default;

    /// Set the title of the message box
    UI_QT_API void set_title(const std::string& _title) override;

    /// Set the message
    UI_QT_API void set_message(const std::string& _msg) override;

    /// Sets the echo mode used to display input field content
    UI_QT_API void set_echo_mode(input::echo_mode _echo_mode) override;

    /// Set the input text in the input field
    UI_QT_API void set_input(const std::string& _text) override;

    /// Get the input text in the input field
    UI_QT_API std::pair<std::string, bool> get_input() override;

protected:

    /// Dialog title
    std::string m_title;

    /// Dialog box message
    std::string m_message;

    /// Text input
    std::string m_input;

    /// Echo mode
    input::echo_mode m_echo_mode {input::echo_mode::DEFAULT};
};

} // namespace sight::ui::qt::dialog

/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/__/config.hpp"
#include "ui/__/dialog/input_base.hpp"

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic input dialog for GUI.
 * Use the Delegate design pattern
 *
 * Example of use:
 * @code{.xml}
   ui::dialog::input inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.set_input ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class UI_CLASS_API input final : public input_base
{
public:

    SIGHT_DECLARE_CLASS(input, input_base);

    /**
     * Opens a default input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     * @return Returns text in the input field
     */
    UI_API static std::pair<std::string, bool> show_input_dialog(
        const std::string& _title,
        const std::string& _message,
        const std::string& _text = "",
        echo_mode _echo_mode     = echo_mode::DEFAULT
    );

    /** @brief Constructor. Create the implementation of the specific input dialog box.
     *
     * Example of use:
     * @code{.xml}
       ui::dialog::input inputDlg;
       inputDlg.setTitle("Identification dialog");
       inputDlg.setMessage("Enter Pin Code: ");
       inputDlg.set_input ("<Enter your code here>");
       std::string inputText = inputDlg.getInput();
       @endcode
     */
    UI_API input();

    /**
     * Constructor. Creates a input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     */
    UI_API input(
        const std::string& _title,
        const std::string& _message,
        const std::string& _text = "",
        echo_mode _echo_mode     = echo_mode::DEFAULT
    );

    /// Destructor. Do nothing
    UI_API ~input() override;

    /// Set the title of the input dialog
    UI_API void set_title(const std::string& _title) override;

    /// Set the message
    UI_API void set_message(const std::string& _msg) override;

    /// Sets the echo mode used to display input field content
    UI_API void set_echo_mode(echo_mode _echo_mode) override;

    /// Set the input text in the input field
    UI_API void set_input(const std::string& _text) override;

    /// Get the input text in the input field
    UI_API std::pair<std::string, bool> get_input() override;

protected:

    /// Implementation of an input dialog in a specific IHM (wx/Qt)
    ui::dialog::input_base::sptr m_implementation;
};

} // namespace sight::ui::dialog

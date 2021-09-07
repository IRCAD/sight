/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/dialog/ISelectorDialog.hpp"

namespace sight::ui::base
{

namespace dialog
{

/**
 * @brief   SelectorDialog allows the choice of an element among several (_selections)
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class UI_BASE_CLASS_API SelectorDialog : public ISelectorDialog
{
public:

    SIGHT_DECLARE_CLASS(SelectorDialog, ui::base::dialog::ISelectorDialog, new SelectorDialog);

    /**
     * Opens a default selector with the specified title, message and choices.
     *
     * @param title Title of the selector
     * @param message Message of the selector
     * @param _selections list of choices in the selector
     * @return Returns selected choice
     */
    UI_BASE_API static std::string showSelectorDialog(
        const std::string& title,
        const std::string& message,
        std::vector<std::string> _selections
    );

    /// will instanciate the concrete implementation
    UI_BASE_API SelectorDialog();

    /// Called by the constructor in the GUI thread.
    UI_BASE_API void create();

    /**
     * Constructor. Create a selector with the specified title, message and choices.
     *
     * @param title Title of the selector
     * @param message Message of the selector
     * @param _selections list of choices in the selector
     */
    UI_BASE_API SelectorDialog(
        const std::string& title,
        const std::string& message,
        std::vector<std::string> _selections
    );

    /// Sets the selector title.
    UI_BASE_API void setTitle(std::string title) override;

    /// Set the string list that can be chosen by the selector.
    UI_BASE_API void setSelections(std::vector<std::string> _selections) override;

    /// Show the selector and return the selection.
    UI_BASE_API std::string show() override;

    /// Set the message
    UI_BASE_API void setMessage(const std::string& msg) override;

    /// Add a custom button to this dialog
    UI_BASE_API void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

protected:

    ui::base::dialog::ISelectorDialog::sptr m_implementation;
};

} //namespace dialog

} // namespace sight::ui::base

/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/dialog/selector_base.hpp"

namespace sight::ui::dialog
{

/**
 * @brief   selector allows the choice of an element among several (_selections)
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class SIGHT_UI_CLASS_API selector : public selector_base
{
public:

    SIGHT_DECLARE_CLASS(selector, ui::dialog::selector_base);

    /// will instantiate the concrete implementation
    SIGHT_UI_API selector();

    /**
     * Constructor. Create a selector with the specified title, message and choices.
     *
     * @param _title Title of the selector
     * @param _message Message of the selector
     * @param _selections list of choices in the selector
     * @param _multiple allow multiple selections
     */
    SIGHT_UI_API selector(
        const std::string& _title,
        const std::string& _message,
        std::vector<std::string> _selections,
        bool _multiple = false
    );

    /// Sets the selector title.
    SIGHT_UI_API void set_title(std::string _title) override;

    ///  The string list that can be chosen by the selector, which can be optionally preselected.
    SIGHT_UI_API void set_choices_preset(choices_preset_t _choices) override;

    /// Allows multiple selections (default = false).
    SIGHT_UI_API void set_multiple(bool _multiple) override;

    /// Show the selector and return the selection.
    SIGHT_UI_API selections_t show() override;

    /// Set the message
    SIGHT_UI_API void set_message(const std::string& _msg) override;

    /// Add a custom button to this dialog
    SIGHT_UI_API void add_custom_button(const std::string& _label, std::function<void()> _clicked_fn) override;

protected:

    ui::dialog::selector_base::sptr m_implementation;
};

} // namespace sight::ui::dialog

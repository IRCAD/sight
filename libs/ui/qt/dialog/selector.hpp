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

#include "ui/qt/config.hpp"

#include <ui/__/dialog/selector_base.hpp>

#include <QPushButton>
#include <QVector>

#include <vector>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   Allows choosing an element among several selections.
 */
class UI_QT_CLASS_API selector : public ui::dialog::selector_base
{
public:

    SIGHT_DECLARE_CLASS(selector, ui::dialog::selector_base, ui::factory::make<selector>);

    UI_QT_API ~selector() override = default;

    /// The string list that can be chosen by the selector.
    UI_QT_API void set_choices_preset(choices_preset_t _selections) override;

    /// Sets the selector title.
    UI_QT_API void setTitle(std::string _title) override;

    /// Allows multiple selections (default = false).
    UI_QT_API void set_multiple(bool _multiple) override;

    /// Show the selector and return the selection.
    UI_QT_API selections_t show() override;

    /// Set the message
    UI_QT_API void setMessage(const std::string& msg) override;

    /// Add a custom button to this dialog
    UI_QT_API void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

private:

    choices_preset_t m_choices;

    /// Dialog box message
    std::string m_message;
    std::string m_title;
    bool m_multiple {false};

    /// Stores custom buttons
    QVector<QPushButton*> m_customButtons;
};

} // namespace sight::ui::qt::dialog

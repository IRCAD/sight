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

#include "ui/qt/config.hpp"

#include <ui/base/dialog/IMessageDialog.hpp>

#include <QVector>

#include <string>

class QPushButton;

namespace sight::ui::qt
{
namespace dialog
{

/**
 * @brief Defines the generic message box.
 */
class UI_QT_CLASS_API MessageDialog : public ui::base::dialog::IMessageDialog
{

public:

    SIGHT_DECLARE_CLASS(MessageDialog, ui::base::dialog::IMessageDialog, ui::base::factory::New< MessageDialog > )

    UI_QT_API MessageDialog(ui::base::GuiBaseObject::Key key);

    UI_QT_API ~MessageDialog() override;

    /// Set the title of the message box
    UI_QT_API void setTitle( const std::string& title ) override;

    /// Set the message
    UI_QT_API void setMessage( const std::string& msg ) override;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    UI_QT_API void setIcon( IMessageDialog::Icons icon) override;

    /// Add a button (OK, YES_NO, CANCEL)
    UI_QT_API void addButton( IMessageDialog::Buttons button ) override;

    /// Add a custom button to this dialog
    UI_QT_API void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

    /// Set the default buttons
    UI_QT_API void setDefaultButton( IMessageDialog::Buttons button ) override;

    /// Show the message box and return the clicked button.
    UI_QT_API Buttons show() override;

protected:

    /// Dialog title
    std::string m_title;

    /// Dialog box message
    std::string m_message;

    /// List of the button
    ui::base::dialog::IMessageDialog::Buttons m_buttons;

    /// default buttons
    ui::base::dialog::IMessageDialog::Buttons m_defaultButton;

    /// Icon
    ui::base::dialog::IMessageDialog::Icons m_icon;

    /// Stores custom buttons
    QVector< QPushButton* > m_customButtons;
};

} // namespace dialog
} // namespace sight::ui::qt

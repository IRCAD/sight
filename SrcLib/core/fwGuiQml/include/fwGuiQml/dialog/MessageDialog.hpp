/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGuiQml/config.hpp"

#include <fwGui/dialog/IMessageDialog.hpp>

#include <QVector>

#include <string>

class QPushButton;

namespace fwGuiQml
{
namespace dialog
{

/**
 * @brief Defines the generic message box for IHM.
 */
class FWGUIQML_CLASS_API MessageDialog : public ::fwGui::dialog::IMessageDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MessageDialog)(::fwGui::dialog::IMessageDialog),
                                            (()),
                                            ::fwGui::factory::New< MessageDialog > )

    FWGUIQML_API MessageDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~MessageDialog() override;

    /// Set the title of the message box
    FWGUIQML_API virtual void setTitle( const std::string& title ) override;

    /// Set the message
    FWGUIQML_API virtual void setMessage( const std::string& msg ) override;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    FWGUIQML_API virtual void setIcon( IMessageDialog::Icons icon) override;

    /// Add a button (OK, YES_NO, CANCEL)
    FWGUIQML_API virtual void addButton( IMessageDialog::Buttons button ) override;

    /// Add a custom button to this dialog
    FWGUIQML_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

    /// Set the default buttons
    FWGUIQML_API virtual void setDefaultButton( IMessageDialog::Buttons button ) override;

    /// Show the message box and return the clicked button.
    FWGUIQML_API virtual Buttons show() override;

protected:

    /// Dialog title
    std::string m_title;

    /// Dialog box message
    std::string m_message;

    /// List of the button
    ::fwGui::dialog::IMessageDialog::Buttons m_buttons;

    /// default buttons
    ::fwGui::dialog::IMessageDialog::Buttons m_defaultButton;

    /// Icon
    ::fwGui::dialog::IMessageDialog::Icons m_icon;
};

} // namespace dialog
} // namespace fwGuiQml

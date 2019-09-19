/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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
#include "fwGuiQml/dialog/StandardButton.hpp"

#include <fwGui/dialog/IMessageDialog.hpp>

#include <QObject>
#include <QUrl>

#include <string>

namespace fwGuiQml
{
namespace dialog
{

/**
 * @brief Defines the generic message box for IHM.
 */
class FWGUIQML_CLASS_API MessageDialog : public QObject,
                                         public ::fwGui::dialog::IMessageDialog
{
Q_OBJECT
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)
Q_PROPERTY(QUrl icon MEMBER m_iconImage NOTIFY iconChanged)

public:

    fwCoreClassMacro(MessageDialog, ::fwGui::dialog::IMessageDialog, ::fwGui::factory::New< MessageDialog > );

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

Q_SIGNALS:
    /// notify the qml of property change
    void buttonsChanged();
    void messageChanged();
    void iconChanged();

protected Q_SLOTS:
    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(int button);

private:

    /// Dialog title
    QString m_title;

    /// Dialog box message
    QString m_message;

    ::fwGui::dialog::IMessageDialog::Buttons m_buttons {::fwGui::dialog::IMessageDialog::NOBUTTON};

    /// Icon
    ::fwGui::dialog::IMessageDialog::Icons m_icon {::fwGui::dialog::IMessageDialog::NONE};

    /// Resume the biggest error get with an icon
    QUrl m_iconImage {""};

    /// boolean to check if button was pressed
    ::fwGui::dialog::IMessageDialog::Buttons m_clicked {::fwGui::dialog::IMessageDialog::NOBUTTON};

    /// Setter to QProperty and emit signal
    FWGUIQML_API void emitButtons(StandardButton*);
    FWGUIQML_API void emitIcon(const QUrl& iconPath);
};

} // namespace dialog
} // namespace fwGuiQml

/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwGui/config.hpp"
#include "fwGui/dialog/INotificationDialog.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic message box for IHM.
 * Use the Delegate design pattern.
 */
class FWGUI_CLASS_API NotificationDialog : public ::fwGui::dialog::INotificationDialog
{

public:

    fwCoreClassMacro(NotificationDialog, ::fwGui::dialog::INotificationDialog, new NotificationDialog)

    /**
     * Opens a default message box with the specified title, text and icon.
     *
     * @param _message Message of the notification box
     * @param _type : type of the notification box (SUCESS, FAILURE or INFO)
     * @param _pos : position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
     */
    FWGUI_API static void showNotificationDialog( const std::string& _message,
                                                  INotificationDialog::Type _type =
                                                      INotificationDialog::Type::DEFAULT,
                                                  INotificationDialog::Position _pos = INotificationDialog::Position::DEFAULT );

    /// Constructor. Create the implementation of the specific message box
    FWGUI_API NotificationDialog();

    /**
     * Constructor
     * Creates a notification box with the specified text and type.
     *
     * @param message Message of the notification box
     * @param _type : type of the notification box (SUCESS, FAILURE or INFO)
     * @param _pos : position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
     */
    FWGUI_API NotificationDialog( const std::string& _message,
                                  INotificationDialog::Type _type    = INotificationDialog::Type::DEFAULT,
                                  INotificationDialog::Position _pos = INotificationDialog::Position::DEFAULT);

    /// Destructor. Do nothing
    FWGUI_API virtual ~NotificationDialog() override;

    /// Show the message box and return the clicked button.
    FWGUI_API virtual void show() override;

protected:

    /// Implementation of a message box in a specific IHM (wx/Qt)
    ::fwGui::dialog::INotificationDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace fwGui

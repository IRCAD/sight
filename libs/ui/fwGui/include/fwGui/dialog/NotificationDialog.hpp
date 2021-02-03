/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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
 * @brief Defines the generic message box for IHM.
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
     * @param _type type of the notification box (SUCESS, FAILURE or INFO)
     * @param _pos position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
     */
    FWGUI_API static void show( const std::string& _message,
                                INotificationDialog::Type _type =
                                    INotificationDialog::Type::DEFAULT,
                                INotificationDialog::Position _pos = INotificationDialog::Position::DEFAULT );

    /**
     * Opens a default message box with the specified title, text and icon.
     *
     * @param _message Message of the notification box
     * @param _type type of the notification box (SUCESS, FAILURE or INFO)
     * @param _pos position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
     * @deprecated use show() instead
     */
    [[deprecated("Sight 22.0, use show() instead.")]]
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
     * @param _message message of the notification box
     * @param _type type of the notification box (SUCCESS, FAILURE or INFO)
     * @param _pos position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
     */
    FWGUI_API NotificationDialog( const std::string& _message,
                                  INotificationDialog::Type _type    = INotificationDialog::Type::DEFAULT,
                                  INotificationDialog::Position _pos = INotificationDialog::Position::DEFAULT);

    /// Destructor. Does nothing
    FWGUI_API virtual ~NotificationDialog() override;

    /// Shows the message box and return the clicked button.
    FWGUI_API virtual void show() override;

    /// Sets the message.
    FWGUI_API virtual void setMessage(const std::string& _msg) override;

    /// Sets the notification type.
    FWGUI_API virtual void setType( Type _type) override;

    /// Sets the position.
    FWGUI_API virtual void setPosition( Position _position) override;

    /// Sets the size
    FWGUI_API virtual void setSize(unsigned int _width, unsigned int _height) override;

    /// Sets the index
    FWGUI_API virtual void setIndex(unsigned int _index) override;

    /// Sets the duration in ms.
    FWGUI_API virtual void setDuration(int _durationInMs) override;

    /**
     * @brief Gets the visibility
     * @return Visibility, true = visible.
     */
    FWGUI_API virtual bool isVisible() const override;

    /// Closes the notification.
    FWGUI_API virtual void close() const override;

    /**
     * @brief Sets the parent container.
     * @param _container const pointer to a ::fwGui::container::fwContainer.
     */
    FWGUI_API virtual void setContainer(::fwGui::container::fwContainer::csptr _container) override;

protected:

    /// Implementation of a message box in a specific IHM (wx/Qt)
    ::fwGui::dialog::INotificationDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace fwGui

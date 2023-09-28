/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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
#include "ui/__/dialog/notification_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief Defines the generic message box for IHM.
 * Use the Delegate design pattern.
 */
class UI_CLASS_API notification : public ui::dialog::notification_base
{
public:

    SIGHT_DECLARE_CLASS(notification, ui::dialog::notification_base);

    /// Opens a default message box with the specified title, text and icon.
    /// @param _message Message of the notification box
    /// @param _type type of the notification box (SUCCESS, FAILURE or INFO)
    /// @param _pos position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
    /// @{
    UI_API static void show(sight::service::Notification notification);
    inline static void show(
        std::string _message,
        sight::service::Notification::Type _type    = sight::service::Notification::Type::INFO,
        sight::service::Notification::Position _pos = sight::service::Notification::Position::TOP_RIGHT
    );
    /// @}

    /// Constructor. Create the implementation of the specific message box
    UI_API notification();

    /// Constructor. Creates a notification box with the specified text and type.
    /// @param _message message of the notification box
    /// @param _type type of the notification box (SUCCESS, FAILURE or INFO)
    /// @param _pos position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
    /// @{
    UI_API notification(sight::service::Notification notification);
    inline explicit notification(
        std::string _message,
        sight::service::Notification::Type _type    = sight::service::Notification::Type::INFO,
        sight::service::Notification::Position _pos = sight::service::Notification::Position::TOP_RIGHT
    ) :
        notification({.type = _type, .position = _pos, .message = std::move(_message)})
    {
    }

    /// @}

    /// Destructor. Does nothing
    UI_API ~notification() override = default;

    /// Shows the notification.
    UI_API void show() override;

    /// Sets the message.
    UI_API void setMessage(std::string _msg) override;

    /// Sets the notification type.
    UI_API void setType(Type _type) override;

    /// Sets the position.
    UI_API void setPosition(Position _position) override;

    /// Sets the size
    UI_API void setSize(std::array<int, 2> _size) override;
    UI_API std::array<int, 2> size() const override;

    /// Sets the index
    UI_API void setIndex(unsigned int _index) override;

    /// Sets the duration in ms.
    UI_API void setDuration(std::optional<std::chrono::milliseconds> _durationInMs) override;
    UI_API std::optional<std::chrono::milliseconds> getDuration() const override;

    /// Sets the channel. Empty string for default global channel
    UI_API void setChannel(std::string) override;
    UI_API std::string getChannel() const override;

    /// Sets the closable property. std::nullopt means finite duration is closable
    UI_API void setClosable(std::optional<bool> _closable) override;
    UI_API std::optional<bool> isClosable() const override;

    /**
     * @brief Gets the visibility
     * @return Visibility, true = visible.
     */
    UI_API bool isVisible() const override;

    /// Closes the notification.
    UI_API void close() const override;

    /// Move the notification to the lower index
    UI_API void moveDown() override;

    /**
     * @brief Sets the parent container.
     * @param _container const pointer to a ui::container::widget.
     */
    UI_API void setContainer(ui::container::widget::csptr _container) override;

    /// Define the callback called when the dialog is closed
    UI_API void setClosedCallback(std::function<void()> f) override;

protected:

    /// Implementation of a message box in a specific IHM (wx/Qt)
    ui::dialog::notification_base::sptr m_implementation;
};

//------------------------------------------------------------------------------

inline void notification::show(
    std::string _message,
    sight::service::Notification::Type _type,
    sight::service::Notification::Position _pos
)
{
    show({.type = _type, .position = _pos, .message = std::move(_message)});
}

} // namespace sight::ui::dialog

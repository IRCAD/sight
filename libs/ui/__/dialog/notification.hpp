/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/dialog/notification_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief Defines the generic message box for UI.
 * Use the Delegate design pattern.
 */
class SIGHT_UI_CLASS_API notification : public ui::dialog::notification_base
{
public:

    SIGHT_DECLARE_CLASS(notification, ui::dialog::notification_base);

    /// Opens a default message box with the specified title, text and icon.
    /// @param _message Message of the notification box
    /// @param _type type of the notification box (SUCCESS, FAILURE or INFO)
    /// @param _pos position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
    /// @{
    inline static void show(
        std::string _message,
        enum sight::service::notification::type _type    = sight::service::notification::type::info,
        enum sight::service::notification::position _pos = sight::service::notification::position::top_right
    );
    SIGHT_UI_API static void show(sight::service::notification _notification);
    /// @}

    /// Constructor. Create the implementation of the specific message box
    SIGHT_UI_API notification();

    /// Constructor. Creates a notification box with the specified text and type.
    /// @param _message message of the notification box
    /// @param _type type of the notification box (SUCCESS, FAILURE or INFO)
    /// @param _pos position where notification is displayed (TOP_LEFT, TOP_RIGHT, ...)
    /// @{
    inline explicit notification(
        std::string _message,
        enum sight::service::notification::type _type    = sight::service::notification::type::info,
        enum sight::service::notification::position _pos = sight::service::notification::position::top_right
    ) :
        notification({.type = _type, .position = _pos, .message = std::move(_message)})
    {
    }

    SIGHT_UI_API notification(sight::service::notification _notification);
    /// @}

    /// Destructor. Does nothing
    SIGHT_UI_API ~notification() override = default;

    /// Shows the notification.
    SIGHT_UI_API void show() override;

    /// Sets the message.
    SIGHT_UI_API void set_message(std::string _msg) override;

    /// Sets the notification type.
    SIGHT_UI_API void set_type(type _type) override;

    /// Sets the position.
    SIGHT_UI_API void set_position(position _position) override;

    /// Sets the size
    SIGHT_UI_API void set_size(std::array<int, 2> _size) override;
    SIGHT_UI_API std::array<int, 2> size() const override;

    /// Sets the index
    SIGHT_UI_API void set_index(unsigned int _index) override;

    /// Sets the duration in ms.
    SIGHT_UI_API void set_duration(std::optional<std::chrono::milliseconds> _duration_in_ms) override;
    SIGHT_UI_API std::optional<std::chrono::milliseconds> get_duration() const override;

    /// Sets the channel. Empty string for default global channel
    SIGHT_UI_API void set_channel(std::string) override;
    SIGHT_UI_API std::string get_channel() const override;

    /// Sets the closable property. std::nullopt means finite duration is closable
    SIGHT_UI_API void set_closable(std::optional<bool> _closable) override;
    SIGHT_UI_API std::optional<bool> is_closable() const override;

    /**
     * @brief Gets the visibility
     * @return Visibility, true = visible.
     */
    SIGHT_UI_API bool is_visible() const override;

    /// Closes the notification.
    SIGHT_UI_API void close() const override;

    /// Move the notification to the lower index
    SIGHT_UI_API void move_down() override;

    /**
     * @brief Sets the parent container.
     * @param _container const pointer to a ui::container::widget.
     */
    SIGHT_UI_API void set_container(ui::container::widget::csptr _container) override;

    /// Define the callback called when the dialog is closed
    SIGHT_UI_API void set_closed_callback(std::function<void()> _f) override;

protected:

    /// Implementation of a message box in a specific UI (wx/Qt)
    ui::dialog::notification_base::sptr m_implementation;
};

//------------------------------------------------------------------------------

inline void notification::show(
    std::string _message,
    enum sight::service::notification::type _type,
    enum sight::service::notification::position _pos
)
{
    show({.type = _type, .position = _pos, .message = std::move(_message)});
}

} // namespace sight::ui::dialog

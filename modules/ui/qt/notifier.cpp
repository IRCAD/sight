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

#include "modules/ui/qt/notifier.hpp"

#include <core/base.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <service/macros.hpp>

#include <ui/__/registry.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <QApplication>

namespace sight::module::ui::qt
{

static const core::com::slots::key_t POP_NOTIFICATION_SLOT   = "pop";
static const core::com::slots::key_t CLOSE_NOTIFICATION_SLOT = "close_notification";
static const core::com::slots::key_t SET_ENUM_PARAMETER_SLOT = "set_enum_parameter";

static const std::string POSITION_KEY("position");
static const std::string DURATION_KEY("duration");
static const std::string SIZE_KEY("size");
static const std::string MAX_KEY("max");
static const std::string CLOSABLE_KEY("closable");

static const std::string INFINITE("infinite");

static const std::vector<std::filesystem::path> SOUND_BOARD = {
    std::filesystem::canonical(
        sight::core::runtime::get_resource_file_path("sight::module::ui::qt/sounds/info_beep.wav")
    ),
    std::filesystem::canonical(
        sight::core::runtime::get_resource_file_path("sight::module::ui::qt/sounds/success_beep.wav")
    ),
    std::filesystem::canonical(
        sight::core::runtime::get_resource_file_path("sight::module::ui::qt/sounds/failure_beep.wav")
    )
};

static const std::map<const std::string, const sight::ui::dialog::notification::position> POSITION_MAP = {
    {"TOP_RIGHT", service::notification::position::top_right},
    {"TOP_LEFT", service::notification::position::top_left},
    {"CENTERED_TOP", service::notification::position::centered_top},
    {"CENTERED", service::notification::position::centered},
    {"BOTTOM_RIGHT", service::notification::position::bottom_right},
    {"BOTTOM_LEFT", service::notification::position::bottom_left},
    {"CENTERED_BOTTOM", service::notification::position::centered_bottom}
};

//-----------------------------------------------------------------------------

notifier::notifier() noexcept
{
    new_slot(POP_NOTIFICATION_SLOT, &notifier::pop, this);
    new_slot(CLOSE_NOTIFICATION_SLOT, &notifier::close_notification, this);
    new_slot(SET_ENUM_PARAMETER_SLOT, &notifier::set_enum_parameter, this);
}

//-----------------------------------------------------------------------------

void notifier::configuring()
{
    const auto& config = this->get_config();

    if(const auto& channels = config.get_child_optional("channels"); channels)
    {
        for(const auto& channel : boost::make_iterator_range(channels->equal_range("channel")))
        {
            configuration channel_config {};

            // UID
            const auto& uid = channel.second.get_optional<std::string>("<xmlattr>.uid").value_or("");

            // Position
            if(const auto& position = channel.second.get_optional<std::string>("<xmlattr>." + POSITION_KEY);
               position)
            {
                if(POSITION_MAP.contains(*position))
                {
                    channel_config.position = POSITION_MAP.at(*position);
                }
                else
                {
                    SIGHT_ERROR(
                        "Position '"
                        + *position
                        + "' isn't a valid position value, accepted values are:"
                          "TOP_RIGHT, TOP_LEFT, CENTERED_TOP, CENTERED, BOTTOM_RIGHT, BOTTOM_LEFT, CENTERED_BOTTOM."
                    )
                }
            }

            // Duration
            if(const auto& duration = channel.second.get_optional<std::string>("<xmlattr>." + DURATION_KEY);
               duration)
            {
                if(*duration == INFINITE)
                {
                    channel_config.duration = std::chrono::milliseconds(0);
                }
                else
                {
                    try
                    {
                        channel_config.duration = std::chrono::milliseconds(std::stoul(*duration));
                    }
                    catch(...)
                    {
                        SIGHT_ERROR(
                            "Duration '"
                            + *duration
                            + "' is not valid. Accepted values are: '"
                            + INFINITE
                            + "' or a positive number of milliseconds."
                        )
                    }
                }
            }

            // Size
            if(const auto& size = channel.second.get_optional<std::string>("<xmlattr>." + SIZE_KEY); size)
            {
                try
                {
                    if(const auto pos = size->find_first_of('x'); pos != std::string::npos)
                    {
                        const auto width  = std::stoul(size->substr(0, pos));
                        const auto height = std::stoul(size->substr(pos + 1));

                        channel_config.size = {int(width), int(height)};
                    }
                    else
                    {
                        throw std::runtime_error("No 'x' found.");
                    }
                }
                catch(...)
                {
                    SIGHT_ERROR(
                        "Size '"
                        + *size
                        + "' is not valid. Accepted values are: `n x n` where 'n' is a positive number."
                    )
                }
            }

            // Max
            if(const auto& max = channel.second.get_optional<std::string>("<xmlattr>." + MAX_KEY); max)
            {
                try
                {
                    channel_config.max = std::stoul(*max);
                }
                catch(...)
                {
                    SIGHT_ERROR(
                        "Maximum '"
                        + *max
                        + "' is not valid. Accepted values are positive numbers."
                    )
                }
            }

            // Closable
            if(const auto& closable = channel.second.get_optional<std::string>("<xmlattr>." + CLOSABLE_KEY);
               closable)
            {
                channel_config.closable = *closable == "true";
            }

            m_channels.insert_or_assign(uid, channel_config);
        }
    }

    // Lastly, initialize sound strutures.
    m_sound = std::make_unique<QSoundEffect>(qApp);
    m_sound->setVolume(1.0);

    m_default_message     = config.get<std::string>("message", m_default_message);
    m_parent_container_id = config.get<std::string>("parent.<xmlattr>.uid", m_parent_container_id);
}

//-----------------------------------------------------------------------------

void notifier::starting()
{
    if(!m_parent_container_id.empty())
    {
        auto container = sight::ui::registry::get_sid_container(m_parent_container_id);

        if(!container)
        {
            container = sight::ui::registry::get_wid_container(m_parent_container_id);
        }

        // If we have an SID/WID set the container.
        if(container)
        {
            m_container_where_to_display_notifs = container;
        }
    }
}

//-----------------------------------------------------------------------------

void notifier::stopping()
{
    for(const auto& [position, stack] : m_stacks)
    {
        for(const auto& popup : stack.popups)
        {
            popup->close();
        }
    }

    m_stacks.clear();
}

//-----------------------------------------------------------------------------

void notifier::updating()
{
}

//-----------------------------------------------------------------------------

void notifier::set_enum_parameter(std::string _val, std::string _key)
{
    try
    {
        if(_key == POSITION_KEY)
        {
            m_channels[""].position = POSITION_MAP.at(_val);
        }
        else if(_key == DURATION_KEY)
        {
            if(_val == INFINITE)
            {
                m_channels[""].duration = std::chrono::milliseconds(0);
            }
            else
            {
                m_channels[""].duration = std::chrono::milliseconds(std::stoul(_val));
            }
        }
        else if(_key == SIZE_KEY)
        {
            if(const auto pos = _val.find_first_of('x'); pos != std::string::npos)
            {
                const auto width  = std::stoul(_val.substr(0, pos));
                const auto height = std::stoul(_val.substr(pos + 1));

                m_channels[""].size = {int(width), int(height)};
            }
        }
        else if(_key == MAX_KEY)
        {
            m_channels[""].max = std::stoul(_val);
        }
        else if(_key == CLOSABLE_KEY)
        {
            m_channels[""].closable = _val == "true";
        }
    }
    catch(...)
    {
        SIGHT_ERROR(std::string("Value '") + _val + "' is not handled for key " + _key);
    }
}

//-----------------------------------------------------------------------------

void notifier::pop(service::notification _notification)
{
    const bool channel_configured = m_channels.contains(_notification.channel);

    // Get channel configuration (or global configuration if there is no channel)
    const auto& channel_configuration = channel_configured
                                        ? m_channels[_notification.channel]
                                        : m_channels[""];

    const auto& default_configuration = m_channels[""];

    // Get the stack configuration. First try the channel, then the default, then the notification itself
    // If you want that services totally control the notification, associate them to an unconfigured notifier
    const auto& position = channel_configured && channel_configuration.position
                           ? *channel_configuration.position
                           : (channel_configured && !channel_configuration.position) || !default_configuration.position
                           ? _notification.position
                           : *default_configuration.position;

    const auto& duration = channel_configured && channel_configuration.duration
                           ? *channel_configuration.duration
                           : (channel_configured && !channel_configuration.duration) || !default_configuration.duration
                           ? _notification.duration
                           : *default_configuration.duration;

    const auto& size = channel_configured && channel_configuration.size
                       ? *channel_configuration.size
                       : (channel_configured && !channel_configuration.size) || !default_configuration.size
                       ? _notification.size
                       : *default_configuration.size;

    const auto& max = channel_configuration.max
                      ? *channel_configuration.max
                      : default_configuration.max
                      ? *default_configuration.max
                      : 0;

    const auto& closable = channel_configured && channel_configuration.closable
                           ? *channel_configuration.closable
                           : (channel_configured && !channel_configuration.closable) || !default_configuration.closable
                           ? _notification.closable
                           : *default_configuration.closable;

    // Get the wanted stack
    auto& target_stack = m_stacks[position];

    // Compute harmonized max and size
    target_stack.max = target_stack.max
                       ? std::max(*target_stack.max, max)
                       : max;

    target_stack.size = target_stack.size
                        ? std::array<int, 2>
    {
        std::max((*target_stack.size)[0], size[0]),
        std::max((*target_stack.size)[1], size[1])
    }
                        : size;

    // If the maximum number of notification is reached, remove the oldest one.
    clean_notifications(position, *target_stack.max, *target_stack.size);

    // Get or create the notification
    const auto& popup =
        [&]
        {
            // If a channel is present, try to retrieve the associated dialog
            if(!_notification.channel.empty())
            {
                for(auto& [old_position, stack] : m_stacks)
                {
                    for(const auto& popup : stack.popups)
                    {
                        if(popup->get_channel() == _notification.channel)
                        {
                            // If the position doesn't match, fix it
                            if(old_position != position)
                            {
                                // Explicit copy
                                auto copy = popup;
                                copy->set_index(static_cast<unsigned int>(target_stack.popups.size()));
                                target_stack.popups.emplace_back(copy);

                                // Remove the original
                                stack.popups.remove(popup);

                                return copy;
                            }

                            return popup;
                        }
                    }
                }
            }

            // No channel or the dialog was not found, create a new one
            auto popup = std::make_shared<sight::ui::dialog::notification>();
            popup->set_index(static_cast<unsigned int>(target_stack.popups.size()));
            target_stack.popups.emplace_back(popup);

            return popup;
        }();

    popup->set_container(m_container_where_to_display_notifs);

    const std::string& message_to_show = _notification.message.empty() ? m_default_message : _notification.message;
    popup->set_message(message_to_show);

    popup->set_type(_notification.type);
    popup->set_position(position);
    popup->set_duration(duration);
    popup->set_size(*target_stack.size);
    std::weak_ptr<sight::core::base_object> weak_notifier = this->shared_from_this();
    popup->set_closed_callback(
        [weak_notifier, popup](auto&& ...)
        {
            if(auto notifier = std::dynamic_pointer_cast<sight::module::ui::qt::notifier>(weak_notifier.lock());
               notifier)
            {
                notifier->on_notification_closed(popup);
            }
        });
    popup->set_channel(_notification.channel);
    popup->set_closable(closable);
    popup->show();

    if(_notification.sound.has_value() && _notification.sound.value())
    {
        m_sound->setSource(QUrl::fromLocalFile(QString::fromStdString(SOUND_BOARD[_notification.type].string())));
        m_sound->play();
    }
}

//------------------------------------------------------------------------------

void notifier::close_notification(std::string _channel)
{
    bool found = false;

    for(const auto& [position, stack] : m_stacks)
    {
        for(const auto& popup : stack.popups)
        {
            if(popup->get_channel() == _channel)
            {
                found = true;
                popup->close();
            }
        }
    }

    SIGHT_WARN_IF("Notification on channel '" << _channel << "' is already closed.", !found);
}

//------------------------------------------------------------------------------

void notifier::on_notification_closed(const sight::ui::dialog::notification::sptr& _notif)
{
    // If the notification still exist
    for(auto& [position, stack] : m_stacks)
    {
        if(auto it = std::find(stack.popups.begin(), stack.popups.end(), _notif); it != stack.popups.end())
        {
            erase_notification(position, it);
        }
    }
}

//------------------------------------------------------------------------------

std::list<sight::ui::dialog::notification::sptr>::iterator notifier::erase_notification(
    const enum service::notification::position& _position,
    const std::list<sight::ui::dialog::notification::sptr>::iterator& _it
)
{
    // Remove the notification from the container
    auto& stack        = m_stacks[_position];
    const auto next_it = stack.popups.erase(_it);
    auto remaining_it  = next_it;

    // Move all the remaining notifications one index lower
    while(remaining_it != stack.popups.end())
    {
        (*remaining_it)->move_down();
        ++remaining_it;
    }

    // Return the it pointing after the erased one
    return next_it;
}

//------------------------------------------------------------------------------

void notifier::clean_notifications(
    const enum service::notification::position& _position,
    std::size_t _max,
    std::array<int, 2> _size,
    bool _skip_permanent
)
{
    // Get the correct "stack"
    auto& stack = m_stacks[_position];

    std::size_t removable_popups = 0;

    // Count how many popups that can be removed there are
    for(const auto& popup : stack.popups)
    {
        if(!_skip_permanent || popup->get_duration())
        {
            ++removable_popups;
        }
    }

    for(auto it = stack.popups.begin() ; removable_popups >= _max && it != stack.popups.end() ; )
    {
        // If the popup is removable
        if(const auto& duration = (*it)->get_duration(); !_skip_permanent || (duration && duration->count() > 0))
        {
            // Remove it
            (*it)->close();
            it = erase_notification(_position, it);
            --removable_popups;
        }
        else
        {
            ++it;
        }
    }

    // Adjust sizes
    for(const auto& popup : stack.popups)
    {
        popup->set_size(_size);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt

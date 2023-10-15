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

#include "modules/ui/qt/notifier.hpp"

#include <core/base.hpp>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/__/registry.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::module::ui::qt
{

static const core::com::slots::key_t POP_NOTIFICATION_SLOT   = "pop";
static const core::com::slots::key_t CLOSE_NOTIFICATION_SLOT = "closeNotification";
static const core::com::slots::key_t SET_ENUM_PARAMETER_SLOT = "setEnumParameter";

static const std::string s_POSITION_KEY("position");
static const std::string s_DURATION_KEY("duration");
static const std::string s_SIZE_KEY("size");
static const std::string s_MAX_KEY("max");
static const std::string s_CLOSABLE_KEY("closable");

static const std::string s_INFINITE("infinite");

static const std::map<const std::string, const sight::ui::dialog::notification::Position> s_POSITION_MAP = {
    {"TOP_RIGHT", service::Notification::Position::TOP_RIGHT},
    {"TOP_LEFT", service::Notification::Position::TOP_LEFT},
    {"CENTERED_TOP", service::Notification::Position::CENTERED_TOP},
    {"CENTERED", service::Notification::Position::CENTERED},
    {"BOTTOM_RIGHT", service::Notification::Position::BOTTOM_RIGHT},
    {"BOTTOM_LEFT", service::Notification::Position::BOTTOM_LEFT},
    {"CENTERED_BOTTOM", service::Notification::Position::CENTERED_BOTTOM}
};

//-----------------------------------------------------------------------------

notifier::notifier() noexcept
{
    new_slot(POP_NOTIFICATION_SLOT, &notifier::pop, this);
    new_slot(CLOSE_NOTIFICATION_SLOT, &notifier::closeNotification, this);
    new_slot(SET_ENUM_PARAMETER_SLOT, &notifier::setEnumParameter, this);
}

//-----------------------------------------------------------------------------

void notifier::configuring()
{
    const auto& config = this->get_config();

    if(const auto& channels = config.get_child_optional("channels"); channels)
    {
        for(const auto& channel : boost::make_iterator_range(channels->equal_range("channel")))
        {
            Configuration channel_config {};

            // UID
            const auto& uid = channel.second.get_optional<std::string>("<xmlattr>.uid").value_or("");

            // Position
            if(const auto& position = channel.second.get_optional<std::string>("<xmlattr>." + s_POSITION_KEY);
               position)
            {
                if(s_POSITION_MAP.contains(*position))
                {
                    channel_config.position = s_POSITION_MAP.at(*position);
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
            if(const auto& duration = channel.second.get_optional<std::string>("<xmlattr>." + s_DURATION_KEY);
               duration)
            {
                if(*duration == s_INFINITE)
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
                            + s_INFINITE
                            + "' or a positive number of milliseconds."
                        )
                    }
                }
            }

            // Size
            if(const auto& size = channel.second.get_optional<std::string>("<xmlattr>." + s_SIZE_KEY); size)
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
            if(const auto& max = channel.second.get_optional<std::string>("<xmlattr>." + s_MAX_KEY); max)
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
            if(const auto& closable = channel.second.get_optional<std::string>("<xmlattr>." + s_CLOSABLE_KEY);
               closable)
            {
                channel_config.closable = *closable == "true";
            }

            m_channels.insert_or_assign(uid, channel_config);
        }
    }

    m_defaultMessage    = config.get<std::string>("message", m_defaultMessage);
    m_parentContainerID = config.get<std::string>("parent.<xmlattr>.uid", m_parentContainerID);
}

//-----------------------------------------------------------------------------

void notifier::starting()
{
    if(!m_parentContainerID.empty())
    {
        auto container = sight::ui::registry::getSIDContainer(m_parentContainerID);

        if(!container)
        {
            container = sight::ui::registry::getWIDContainer(m_parentContainerID);
        }

        // If we have an SID/WID set the container.
        if(container)
        {
            m_containerWhereToDisplayNotifs = container;
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

void notifier::setEnumParameter(std::string _val, std::string _key)
{
    try
    {
        if(_key == s_POSITION_KEY)
        {
            m_channels[""].position = s_POSITION_MAP.at(_val);
        }
        else if(_key == s_DURATION_KEY)
        {
            if(_val == s_INFINITE)
            {
                m_channels[""].duration = std::chrono::milliseconds(0);
            }
            else
            {
                m_channels[""].duration = std::chrono::milliseconds(std::stoul(_val));
            }
        }
        else if(_key == s_SIZE_KEY)
        {
            if(const auto pos = _val.find_first_of('x'); pos != std::string::npos)
            {
                const auto width  = std::stoul(_val.substr(0, pos));
                const auto height = std::stoul(_val.substr(pos + 1));

                m_channels[""].size = {int(width), int(height)};
            }
        }
        else if(_key == s_MAX_KEY)
        {
            m_channels[""].max = std::stoul(_val);
        }
        else if(_key == s_CLOSABLE_KEY)
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

void notifier::pop(service::Notification notification)
{
    const bool channel_configured = m_channels.contains(notification.channel);

    // Get channel configuration (or global configuration if there is no channel)
    const auto& channel_configuration = channel_configured
                                        ? m_channels[notification.channel]
                                        : m_channels[""];

    const auto& default_configuration = m_channels[""];

    // Get the stack configuration. First try the channel, then the default, then the notification itself
    // If you want that services totally control the notification, associate them to an unconfigured notifier
    const auto& position = channel_configured && channel_configuration.position
                           ? *channel_configuration.position
                           : (channel_configured && !channel_configuration.position) || !default_configuration.position
                           ? notification.position
                           : *default_configuration.position;

    const auto& duration = channel_configured && channel_configuration.duration
                           ? *channel_configuration.duration
                           : (channel_configured && !channel_configuration.duration) || !default_configuration.duration
                           ? notification.duration
                           : *default_configuration.duration;

    const auto& size = channel_configured && channel_configuration.size
                       ? *channel_configuration.size
                       : (channel_configured && !channel_configuration.size) || !default_configuration.size
                       ? notification.size
                       : *default_configuration.size;

    const auto& max = channel_configuration.max
                      ? *channel_configuration.max
                      : default_configuration.max
                      ? *default_configuration.max
                      : 0;

    const auto& closable = channel_configured && channel_configuration.closable
                           ? *channel_configuration.closable
                           : (channel_configured && !channel_configuration.closable) || !default_configuration.closable
                           ? notification.closable
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
    cleanNotifications(position, *target_stack.max, *target_stack.size);

    // Get or create the notification
    const auto& popup =
        [&]
        {
            // If a channel is present, try to retrieve the associated dialog
            if(!notification.channel.empty())
            {
                for(auto& [old_position, stack] : m_stacks)
                {
                    for(const auto& popup : stack.popups)
                    {
                        if(popup->getChannel() == notification.channel)
                        {
                            // If the position doesn't match, fix it
                            if(old_position != position)
                            {
                                // Explicit copy
                                auto copy = popup;
                                copy->setIndex(static_cast<unsigned int>(target_stack.popups.size()));
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
            popup->setIndex(static_cast<unsigned int>(target_stack.popups.size()));
            target_stack.popups.emplace_back(popup);

            return popup;
        }();

    popup->setContainer(m_containerWhereToDisplayNotifs);

    const std::string& messageToShow = notification.message.empty() ? m_defaultMessage : notification.message;
    popup->setMessage(messageToShow);

    popup->setType(notification.type);
    popup->setPosition(position);
    popup->setDuration(duration);
    popup->setSize(*target_stack.size);
    popup->setClosedCallback([this, popup](auto&& ...){onNotificationClosed(popup);});
    popup->setChannel(notification.channel);
    popup->setClosable(closable);
    popup->show();
}

//------------------------------------------------------------------------------

void notifier::closeNotification(std::string channel)
{
    bool found = false;

    for(const auto& [position, stack] : m_stacks)
    {
        for(const auto& popup : stack.popups)
        {
            if(popup->getChannel() == channel)
            {
                found = true;
                popup->close();
            }
        }
    }

    SIGHT_WARN_IF("Notification on channel '" << channel << "' is already closed.", !found);
}

//------------------------------------------------------------------------------

void notifier::onNotificationClosed(const sight::ui::dialog::notification::sptr& _notif)
{
    // If the notification still exist
    for(auto& [position, stack] : m_stacks)
    {
        if(auto it = std::find(stack.popups.begin(), stack.popups.end(), _notif); it != stack.popups.end())
        {
            eraseNotification(position, it);
        }
    }
}

//------------------------------------------------------------------------------

std::list<sight::ui::dialog::notification::sptr>::iterator notifier::eraseNotification(
    const service::Notification::Position& position,
    const std::list<sight::ui::dialog::notification::sptr>::iterator& it
)
{
    // Remove the notification from the container
    auto& stack        = m_stacks[position];
    const auto next_it = stack.popups.erase(it);
    auto remaining_it  = next_it;

    // Move all the remaining notifications one index lower
    while(remaining_it != stack.popups.end())
    {
        (*remaining_it)->moveDown();
        ++remaining_it;
    }

    // Return the it pointing after the erased one
    return next_it;
}

//------------------------------------------------------------------------------

void notifier::cleanNotifications(
    const service::Notification::Position& position,
    std::size_t max,
    std::array<int, 2> size,
    bool skipPermanent
)
{
    // Get the correct "stack"
    auto& stack = m_stacks[position];

    std::size_t removable_popups = 0;

    // Count how many popups that can be removed there are
    for(const auto& popup : stack.popups)
    {
        if(!skipPermanent || popup->getDuration())
        {
            ++removable_popups;
        }
    }

    for(auto it = stack.popups.begin() ; removable_popups >= max && it != stack.popups.end() ; )
    {
        // If the popup is removable
        if(const auto& duration = (*it)->getDuration(); !skipPermanent || (duration && duration->count() > 0))
        {
            // Remove it
            (*it)->close();
            it = eraseNotification(position, it);
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
        popup->setSize(size);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt

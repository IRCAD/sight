/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "notification_composer.hpp"

#include <core/notification/error.hpp>
#include <core/notification/information.hpp>
#include <core/notification/instruction.hpp>
#include <core/notification/warning.hpp>
#include <core/spy_log.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <iterator>
#include <optional>
#include <ranges>

namespace sight::module::ui
{

/// The notification kind matching an XML tag, absent if the tag names none of the four known ones.
static std::optional<notification_kind> kind_for(const std::string& _tag)
{
    if(_tag == "instruction")
    {
        return notification_kind::instruction;
    }

    if(_tag == "information")
    {
        return notification_kind::information;
    }

    if(_tag == "warning")
    {
        return notification_kind::warning;
    }

    if(_tag == "error")
    {
        return notification_kind::error;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

/// Build a notification of the given kind and text, with the duration and icon resolved at configuring().
static sight::core::notification::message::sptr build_notification(
    notification_kind _kind,
    std::string _text,
    const std::optional<std::chrono::milliseconds>& _duration,
    const std::optional<std::filesystem::path>& _icon
)
{
    namespace notification = sight::core::notification;

    const notification::message::params params {
        .text = std::move(_text), .icon = _icon, .cancelable = true, .duration = _duration
    };

    if(_kind == notification_kind::instruction)
    {
        return std::make_shared<notification::instruction>(params);
    }

    if(_kind == notification_kind::information)
    {
        return std::make_shared<notification::information>(params);
    }

    if(_kind == notification_kind::warning)
    {
        return std::make_shared<notification::warning>(params);
    }

    return std::make_shared<notification::error>(params);
}

//------------------------------------------------------------------------------

/// Collect the ids of the `${id}` placeholders of the given message.
static std::vector<std::string> placeholders(const std::string& _template)
{
    std::vector<std::string> ids;

    for(std::size_t begin = _template.find("${") ; begin != std::string::npos ; begin = _template.find("${", begin))
    {
        const auto end = _template.find('}', begin);

        if(end == std::string::npos)
        {
            break;
        }

        ids.push_back(_template.substr(begin + 2, end - begin - 2));
        begin = end;
    }

    return ids;
}

//------------------------------------------------------------------------------

/// The configured display duration, falling back to its consumer's default for its type when left out.
static std::optional<std::chrono::milliseconds> configured_duration(const sight::service::config_t& _element)
{
    const auto duration = _element.get_optional<std::int64_t>("<xmlattr>.duration");

    if(!duration.has_value())
    {
        return std::nullopt;
    }

    SIGHT_ERROR_IF("A negative 'duration' attribute is read as 0, i.e. no timeout.", *duration < 0);

    return std::chrono::milliseconds(std::max<std::int64_t>(*duration, 0));
}

//------------------------------------------------------------------------------

/// The configured icon. An absent attribute selects the notification type's default; an empty one disables it.
static std::optional<std::filesystem::path> configured_icon(const sight::service::config_t& _element)
{
    if(const auto icon = _element.get_optional<std::string>("<xmlattr>.icon"); icon.has_value())
    {
        return std::filesystem::path(*icon);
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

notification_composer::notification_composer() :
    has_notifications(has_signals::signals())
{
    new_slot(slots::ADD_NOTIFICATION, &notification_composer::add_notification, this);
    new_slot(slots::CANCEL, &notification_composer::cancel, this);
    new_slot(slots::NEXT, &notification_composer::next, this);
    new_slot(slots::RESTART, &notification_composer::restart, this);
    new_slot(slots::SHOW_CURRENT, &notification_composer::show_current, this);
}

//------------------------------------------------------------------------------

void notification_composer::configuring()
{
    const std::scoped_lock lock(m_mutex);

    m_contributions.clear();
    m_received.clear();
    m_merged.clear();
    m_notifications.clear();
    m_order.clear();
    m_current = 0;

    for(const auto& [tag, element] : this->get_config())
    {
        // The service attributes comes as a child of their own, which names no notification type.
        if(tag == "<xmlattr>")
        {
            continue;
        }

        const auto text          = element.get<std::string>("message", {});
        const auto contributions = element.equal_range("notification");
        const bool is_merged     = contributions.first != contributions.second || !placeholders(text).empty();

        const auto kind     = kind_for(tag);
        const auto duration = configured_duration(element);
        const auto icon     = configured_icon(element);

        if(!kind.has_value())
        {
            SIGHT_ERROR("Unknown element <" << tag << "> in the configuration, it will be ignored.");
            continue;
        }

        if(!is_merged)
        {
            // A regular notification, emitted as is whenever its show slot is called.
            const auto id = element.get_optional<std::string>("<xmlattr>.id");

            SIGHT_ERROR_IF("A <" << tag << "> element needs an 'id' attribute, it will be ignored.", !id.has_value());

            if(id.has_value())
            {
                this->new_notification(
                    *id,
                    *kind,
                    duration,
                    icon,
                    text,
                    element.get<bool>("<xmlattr>.show_on_start", false)
                );
            }

            continue;
        }

        // A merged notification, whose message holds the placeholders its contributions fill.
        m_merged.push_back(
            {.m_kind = *kind, .m_duration = duration, .m_icon = icon, .m_template = text, .m_ids = placeholders(text)
            });

        SIGHT_ERROR_IF(
            "The merged <" << tag << "> has no ${id} placeholder, it will never be displayed.",
            m_merged.back().m_ids.empty()
        );

        for(auto it = contributions.first ; it != contributions.second ; ++it)
        {
            const auto id = it->second.get_optional<std::string>("<xmlattr>.id");

            SIGHT_ERROR_IF(
                "A <notification> element needs an 'id' attribute, it will be ignored.",
                !id.has_value()
            );

            if(id.has_value())
            {
                this->new_contribution(*id, it->second.get<std::string>("message", {}));
            }
        }
    }
}

//------------------------------------------------------------------------------

void notification_composer::new_contribution(const std::string& _id, const std::string& _text)
{
    this->new_slot(
        slots::SHOW_PREFIX + _id,
        [weak_this = this->weak_from_this(), _id, _text]
        {
            if(const auto self = std::dynamic_pointer_cast<notification_composer>(weak_this.lock()); self)
            {
                self->set_contribution(_id, _text);
            }
        })->set_worker(this->worker());

    this->new_slot(
        slots::CANCEL_PREFIX + _id,
        [weak_this = this->weak_from_this(), _id]
        {
            if(const auto self = std::dynamic_pointer_cast<notification_composer>(weak_this.lock()); self)
            {
                self->cancel_contribution(_id);
            }
        })->set_worker(this->worker());
}

//------------------------------------------------------------------------------

void notification_composer::new_notification(
    const std::string& _id,
    notification_kind _kind,
    const std::optional<std::chrono::milliseconds>& _duration,
    const std::optional<std::filesystem::path>& _icon,
    const std::string& _text,
    bool _show_on_start
)
{
    m_notifications[_id] = {
        .m_kind          = _kind, .m_duration = _duration, .m_icon = _icon, .m_text = _text,
        .m_show_on_start = _show_on_start
    };
    m_order.push_back(_id);

    this->new_slot(
        slots::SHOW_PREFIX + _id,
        [weak_this = this->weak_from_this(), _id]
        {
            if(const auto self = std::dynamic_pointer_cast<notification_composer>(weak_this.lock()); self)
            {
                self->show_notification(_id);
            }
        })->set_worker(this->worker());

    this->new_slot(
        slots::CANCEL_PREFIX + _id,
        [weak_this = this->weak_from_this(), _id]
        {
            if(const auto self = std::dynamic_pointer_cast<notification_composer>(weak_this.lock()); self)
            {
                self->cancel_notification(_id);
            }
        })->set_worker(this->worker());
}

//------------------------------------------------------------------------------

void notification_composer::show_notification(const std::string& _id)
{
    const std::scoped_lock lock(m_mutex);

    const auto& it = m_notifications.find(_id);

    if(it == m_notifications.cend())
    {
        return;
    }

    // Showing a notification by name moves the cursor to it, so that a later next() follows from there.
    if(const auto& position = std::ranges::find(m_order, _id); position != m_order.end())
    {
        m_current = static_cast<std::size_t>(std::distance(m_order.begin(), position));
    }

    // Replace the one already displayed for this id, rather than stacking a second copy.
    this->cancel_notification(_id);

    it->second.m_notification = build_notification(
        it->second.m_kind,
        it->second.m_text,
        it->second.m_duration,
        it->second.m_icon
    );
    this->emit_notification_created(it->second.m_notification);
}

//------------------------------------------------------------------------------

void notification_composer::next()
{
    const std::scoped_lock lock(m_mutex);

    this->report_deprecated_sequence();

    ++m_current;
    this->show_current();
}

//------------------------------------------------------------------------------

void notification_composer::restart()
{
    const std::scoped_lock lock(m_mutex);

    this->report_deprecated_sequence();

    m_current = 0;
    this->show_current();
}

//------------------------------------------------------------------------------

void notification_composer::show_current()
{
    const std::scoped_lock lock(m_mutex);

    this->report_deprecated_sequence();

    if(m_current < m_order.size())
    {
        this->show_notification(m_order[m_current]);
    }
}

//------------------------------------------------------------------------------

void notification_composer::report_deprecated_sequence()
{
    const std::scoped_lock lock(m_mutex);

    // Reported once: these slots are wired to signals that fire repeatedly, a footswitch typically.
    FW_DEPRECATED_MSG_IF(
        "The 'next', 'restart' and 'show_current' slots of '" + this->get_id()
        + "' walk an invisible cursor. Address a notification by its own 'show_<id>' slot instead.",
        "27.0",
        !m_sequence_reported
    );

    m_sequence_reported = true;
}

//------------------------------------------------------------------------------

void notification_composer::cancel_notification(const std::string& _id)
{
    const std::scoped_lock lock(m_mutex);

    if(const auto& it = m_notifications.find(_id); it != m_notifications.cend() && it->second.m_notification)
    {
        const auto notification = it->second.m_notification;
        it->second.m_notification = nullptr;
        notification->cancel();
    }
}

//------------------------------------------------------------------------------

void notification_composer::starting()
{
    const std::scoped_lock lock(m_mutex);

    // m_order, and not m_notifications, which a std::map keeps sorted by id: the notifications must be shown
    // in the order they are declared in. The last one shown is the one the consumer displays on top, and the
    // one show_notification() leaves the sequence cursor on, so both would otherwise depend on how the ids
    // happen to sort.
    for(const auto& id : m_order)
    {
        if(const auto& it = m_notifications.find(id); it != m_notifications.cend() && it->second.m_show_on_start)
        {
            this->show_notification(id);
        }
    }
}

//------------------------------------------------------------------------------

void notification_composer::updating()
{
}

//------------------------------------------------------------------------------

void notification_composer::stopping()
{
    this->cancel();
}

//------------------------------------------------------------------------------

void notification_composer::add_notification(sight::core::notification::base::sptr _notification)
{
    const auto message = std::dynamic_pointer_cast<sight::core::notification::message>(_notification);

    if(!message)
    {
        return;
    }

    const auto& id = message->id();

    if(id.empty())
    {
        return;
    }

    const std::scoped_lock lock(m_mutex);

    const bool known = std::ranges::any_of(
        m_merged,
        [&id](const auto& _merged)
        {
            return std::ranges::find(_merged.m_ids, id) != _merged.m_ids.cend();
        });

    SIGHT_WARN_IF(
        "Received a notification with the id '" << id << "', which no merged notification refers to. Ignored.",
        !known
    );

    if(!known)
    {
        return;
    }

    // Keep it, so that its text changes and its cancellation are reflected on the merged notification.
    m_received[id] = message;
    this->watch(message, /*_own=*/ true, this->weak_from_this());

    this->set_contribution(id, message->text());
}

//------------------------------------------------------------------------------

void notification_composer::cancel()
{
    const std::scoped_lock lock(m_mutex);

    // Copy the ids first: the cancel functions erase from the maps as we go.
    std::vector<std::string> ids;
    ids.reserve(m_contributions.size() + m_notifications.size());

    for(const auto& id : m_contributions | std::views::keys)
    {
        ids.push_back(id);
    }

    for(const auto& id : ids)
    {
        this->cancel_contribution(id);
    }

    ids.clear();

    for(const auto& id : m_notifications | std::views::keys)
    {
        ids.push_back(id);
    }

    for(const auto& id : ids)
    {
        this->cancel_notification(id);
    }
}

//------------------------------------------------------------------------------

void notification_composer::set_contribution(const std::string& _id, const std::string& _text)
{
    const std::scoped_lock lock(m_mutex);

    m_contributions[_id] = _text;

    for(auto& merged : m_merged)
    {
        if(std::ranges::find(merged.m_ids, _id) != merged.m_ids.cend())
        {
            this->refresh(merged);
        }
    }
}

//------------------------------------------------------------------------------

void notification_composer::cancel_contribution(const std::string& _id)
{
    const std::scoped_lock lock(m_mutex);

    m_contributions.erase(_id);

    if(const auto& received = m_received.find(_id); received != m_received.cend())
    {
        const auto notification = received->second;
        m_received.erase(received);
        this->forget(notification);
        notification->cancel();
    }

    for(auto& merged : m_merged)
    {
        if(std::ranges::find(merged.m_ids, _id) != merged.m_ids.cend())
        {
            this->refresh(merged);
        }
    }
}

//------------------------------------------------------------------------------

void notification_composer::refresh(merged& _merged)
{
    const std::scoped_lock lock(m_mutex);

    const bool has_contribution = std::ranges::any_of(
        _merged.m_ids,
        [this](const auto& _id)
        {
            return m_contributions.contains(_id);
        });

    if(!has_contribution)
    {
        // The last contribution is gone: the merged notification has nothing left to show.
        if(_merged.m_notification)
        {
            const auto notification = _merged.m_notification;
            _merged.m_notification = nullptr;
            notification->cancel();
        }

        return;
    }

    const auto text = this->substitute(_merged.m_template);

    if(_merged.m_notification)
    {
        // Update in place rather than destroying and recreating the notification: set_text() already
        // reaches its consumer, which displays it without being told about it a second time.
        _merged.m_notification->set_text(text);
        return;
    }

    _merged.m_notification = build_notification(_merged.m_kind, text, _merged.m_duration, _merged.m_icon);

    // Ending the merged notification, by canceling it or by acknowledging it (which finishes it), clears its
    // contributions, so that the next one starts a new merged notification.
    _merged.m_notification->add_state_hook(
        [weak_this = this->weak_from_this(), ids = _merged.m_ids](sight::core::notification::base::state_t _state)
        {
            if(_state != sight::core::notification::base::canceled
               && _state != sight::core::notification::base::finished)
            {
                return;
            }

            if(const auto self = std::dynamic_pointer_cast<notification_composer>(weak_this.lock()); self)
            {
                const std::scoped_lock lock(self->m_mutex);

                for(const auto& id : ids)
                {
                    self->cancel_contribution(id);
                }
            }
        });

    this->emit_notification_created(_merged.m_notification);
}

//------------------------------------------------------------------------------

std::string notification_composer::substitute(const std::string& _template) const
{
    const std::scoped_lock lock(m_mutex);

    std::string text = _template;

    for(std::size_t begin = text.find("${") ; begin != std::string::npos ; begin = text.find("${", begin))
    {
        const auto end = text.find('}', begin);

        if(end == std::string::npos)
        {
            break;
        }

        const auto id           = text.substr(begin + 2, end - begin - 2);
        const auto contribution = m_contributions.find(id);

        // A placeholder without a contribution is substituted with nothing.
        const std::string& value = contribution == m_contributions.cend() ? std::string {} : contribution->second;

        text.replace(begin, end - begin + 1, value);
        begin += value.size();
    }

    return text;
}

//------------------------------------------------------------------------------

void notification_composer::on_notification_finished(const sight::core::notification::base::wptr& _notification)
{
    const std::scoped_lock lock(m_mutex);

    const auto notification = _notification.lock();

    // The contributing notification was canceled or finished by its emitter, drop its contribution. It may
    // already be expired, in which case the entry is found by elimination.
    const auto received = std::ranges::find_if(
        m_received,
        [&notification](const auto& _entry)
        {
            return notification == nullptr ? _entry.second->is_finished() : _entry.second == notification;
        });

    if(received != m_received.cend())
    {
        // Copy the id before erasing: the iterator does not survive it.
        const auto id = received->first;
        m_received.erase(received);
        this->cancel_contribution(id);
    }
}

//------------------------------------------------------------------------------

void notification_composer::on_notification_changed(const sight::core::notification::message::wptr& _notification)
{
    const std::scoped_lock lock(m_mutex);

    const auto notification = _notification.lock();

    if(!notification)
    {
        return;
    }

    // The emitter updated its own text: reflect it in the merged notifications referring to it.
    this->set_contribution(notification->id(), notification->text());
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui

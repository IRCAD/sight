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

#pragma once

#include <core/notification/has_notifications.hpp>
#include <core/notification/notifications_store.hpp>
#include <service/controller.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace sight::module::ui
{

/// The four notification kinds this service can build, matching the XML tags <instruction>, <information>,
/// <warning> and <error>.
enum class notification_kind : std::uint8_t
{
    instruction,
    information,
    warning,
    error
};

/**
 * @brief Composes the notifications of an application, declared in XML, merging several contributions into a
 * single notification when needed.
 *
 * When several services or shared configurations independently emit the same kind of notification (an
 * "out-of-range" warning, typically), they overwrite each other in the notification zone, which flickers, and
 * a shared "clear" slot hides a message that is still relevant. This service composes them instead: each
 * contribution fills a `${id}` placeholder of a merged notification, which is created on the first
 * contribution and updated in place afterwards, rather than being destroyed and recreated.
 *
 * A contribution comes either from a service emitting a notification carrying a matching id (see
 * sight::core::notification::has_notifications::configure_notifications()), or from one of the slots this
 * service creates from its own configuration.
 *
 * Placeholders without a contribution are left empty. Once the last contribution of a merged notification is
 * canceled, the merged notification is canceled. Conversely, canceling the merged notification, e.g. by
 * acknowledging it in the notification zone, cancels all of its contributions.
 *
 * @section XML XML configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::notification_composer">
        <warning>
            <notification id="left_sensor_out_of_range">
                <message><![CDATA[Left sensor <u>out of range</u>]]></message>
            </notification>
            <notification id="right_sensor_out_of_range">
                <message><![CDATA[Right sensor <u>out of range</u>]]></message>
            </notification>
            <message>
                <![CDATA[
                Some sensors may return imprecise data:
                ${left_sensor_out_of_range}
                ${right_sensor_out_of_range}
                ]]>
            </message>
        </warning>

        <instruction id="tracking_started" duration="5000">
            <message>Move the sensors into range</message>
        </instruction>
    </service>
   @endcode
 *
 * - \b instruction, \b information, \b warning, \b error: a notification of the matching type. Holding
 *   `<notification>` children or `${id}` placeholders, it is a merged notification, displayed as soon as one
 *   of its contributions arrives. Otherwise it is a regular notification, emitted as is on its show slot.
 *   - \b id: required for a regular notification, the two slots are deduced from it.
 *   - \b show_on_start (optional, default false): emit it as soon as the service starts, rather than waiting
 *     for its show slot.
 *   - \b duration (optional): display duration in milliseconds, 0 meaning no timeout. Defaults to its
 *     consumer's duration for its type. A merged notification that timed out is displayed again, with its
 *     delay restarted, by its next contribution.
 *   - \b icon (optional): icon path relative to a module, as "<module id>/<resource>". Left out, the
 *     notification type's default icon is used; left empty, no icon is displayed at all.
 *   - \b message: the text, or the template holding the `${id}` placeholders.
 * - \b notification: a contribution triggered by a slot, rather than by a service's notification.
 *   - \b id: the placeholder it fills.
 *   - \b message: the text substituted to that placeholder.
 *
 * @section Signals Signals
 * - \b notification_created: emitted with the merged notifications, and with the regular ones. Connect it to
 *   the notification zone. Note that the contributions themselves are consumed here and never forwarded.
 *   A merged notification is emitted once, on creation: the later contributions update its text, which its
 *   consumer follows on its own, so it is not emitted again.
 *
 * @section Slots Slots
 * - \b add_notification: take a notification into account, as a contribution to every merged notification
 *   referencing its id. Notifications without a known id are ignored.
 * - \b cancel: cancel every notification of this service, contributions included.
 * - \b next, \b restart and \b show_current: @deprecated walk the regular notifications as a sequence, in
 *   declaration order. Showing one by name moves the cursor to it, so both ways of driving the sequence stay
 *   consistent. Only there to port night's workflow_guide, prefer addressing a notification by its id.
 * - \b show_<id> and \b cancel_<id>, deduced from the id of each configured notification and contribution.
 */
class notification_composer final : public sight::service::controller,
                                    public sight::core::notification::has_notifications,
                                    public sight::core::notification::notifications_store
{
public:

    SIGHT_DECLARE_SERVICE(notification_composer, sight::service::controller);

    notification_composer();
    ~notification_composer() noexcept final = default;

    struct slots final
    {
        static inline const slot_key_t ADD_NOTIFICATION = "add_notification";
        static inline const slot_key_t CANCEL           = "cancel";
        /// @deprecated The sequence exists to port night's workflow_guide, address notifications by id instead.
        static inline const slot_key_t NEXT         = "next";
        static inline const slot_key_t RESTART      = "restart";
        static inline const slot_key_t SHOW_CURRENT = "show_current";

        /// Prefixes of the two slots deduced from the id of a configured notification.
        static inline const slot_key_t SHOW_PREFIX   = "show_";
        static inline const slot_key_t CANCEL_PREFIX = "cancel_";
    };

    /// SLOT: take a notification into account as a contribution, @see add_notification
    void add_notification(sight::core::notification::base::sptr _notification);

    /// SLOT: cancel every notification of this service, contributions included.
    void cancel();

    /**
     * @brief SLOT: show the notification following the current one, in declaration order.
     *
     * @deprecated Only there so that the configurations of night's workflow_guide could be ported as they
     * were. A step reached this way depends on an invisible cursor, which the XML does not show: prefer
     * addressing a notification by its own show slot. @see show_current
     */
    void next();

    /// SLOT: go back to the first declared notification and show it. @deprecated @see next
    void restart();

    /**
     * @brief SLOT: show the current notification again.
     *
     * The regular notifications form a sequence, in declaration order, walked by next() and restart(). Showing
     * one through its own show slot moves the cursor to it, so that the two ways of driving the sequence, by
     * step and by name, stay consistent. Past the last one, nothing is shown.
     *
     * @deprecated @see next
     */
    void show_current();

protected:

    /// Parse the contributions, the merged notifications and the regular ones, creating their slots.
    void configuring() final;

    /// Emit the notifications configured with show_on_start
    void starting() final;

    /// Does nothing
    void updating() final;

    /// Cancel everything
    void stopping() final;

    /// notifications_store: a contributing notification was canceled, drop its contribution.
    void on_notification_finished(const sight::core::notification::base::wptr& _notification) final;

    /// notifications_store: a contributing notification changed, refresh the merged text.
    void on_notification_changed(const sight::core::notification::message::wptr& _notification) final;

private:

    /// A notification whose message is built from the contributions its placeholders refer to.
    struct merged final
    {
        // Every field has a default member initializer, so that a designated initializer list can name only
        // the relevant ones without warning.

        /// Concrete notification type to build.
        notification_kind m_kind {notification_kind::instruction};

        /// Display duration of the notification built for this configuration, @see message::params::duration.
        std::optional<std::chrono::milliseconds> m_duration {};

        /// Icon of the notification built for this configuration, @see message::params::icon.
        std::optional<std::filesystem::path> m_icon {};

        /// Message of the configuration, with the `${id}` placeholders still in place.
        std::string m_template {};

        /// Ids of the placeholders found in m_template.
        std::vector<std::string> m_ids {};

        /// The notification currently displayed, null while no contribution has been received.
        sight::core::notification::message::sptr m_notification {nullptr};
    };

    /// Create the show_<id> and cancel_<id> slots of a contribution to a merged notification.
    void new_contribution(const std::string& _id, const std::string& _text);

    /// Create the show_<id> and cancel_<id> slots of a regular notification.
    void new_notification(
        const std::string& _id,
        notification_kind _kind,
        const std::optional<std::chrono::milliseconds>& _duration,
        const std::optional<std::filesystem::path>& _icon,
        const std::string& _text,
        bool _show_on_start
    );

    /// Emit the regular notification of the given id, replacing the one already displayed for it.
    void show_notification(const std::string& _id);

    /// Report, once, that the sequence slots this service was asked for are deprecated.
    void report_deprecated_sequence();

    /// Cancel the regular notification of the given id, if it is currently displayed.
    void cancel_notification(const std::string& _id);

    /// Set or replace the contribution of the given id, then refresh the merged notifications using it.
    void set_contribution(const std::string& _id, const std::string& _text);

    /// Drop the contribution of the given id, then refresh the merged notifications using it.
    void cancel_contribution(const std::string& _id);

    /// Create, update or cancel the merged notification, out of the current contributions.
    void refresh(merged& _merged);

    /// Substitute every `${id}` placeholder of the template with its contribution, empty when there is none.
    [[nodiscard]] std::string substitute(const std::string& _template) const;

    /// Cancel the notifications contributing to the given merged one, called when the user cancels it.
    void cancel_contributions(const merged& _merged);

    /// Contribution texts, by id. Only the ids present here are substituted.
    std::map<std::string, std::string> m_contributions;

    /// A regular notification, and how to build it.
    struct declaration final
    {
        /// Concrete notification type to build.
        notification_kind m_kind {notification_kind::instruction};

        /// Display duration of the notification built for it, @see message::params::duration.
        std::optional<std::chrono::milliseconds> m_duration {};

        /// Icon of the notification built for it, @see message::params::icon.
        std::optional<std::filesystem::path> m_icon {};

        std::string m_text {};
        bool m_show_on_start {false};

        /// The notification currently displayed for it, if any.
        sight::core::notification::message::sptr m_notification {nullptr};
    };

    /// Regular notifications, by id.
    std::map<std::string, declaration> m_notifications;

    /// Ids of the regular notifications, in declaration order: the sequence next() and restart() walk.
    std::vector<std::string> m_order;

    /// Index in m_order of the notification next() will move away from. @see show_current
    std::size_t m_current {0};

    /// Whether the deprecation of the sequence slots was already reported, to report it only once.
    bool m_sequence_reported {false};

    /// Notifications received through add_notification(), by id, kept to cancel them back.
    std::map<std::string, sight::core::notification::message::sptr> m_received;

    /// The merged notifications, in configuration order.
    std::vector<merged> m_merged;

    /// Guards the contributions and the merged notifications, written from the service's slots and read back
    /// from the notifications_store callbacks, which run on the default worker.
    mutable std::recursive_mutex m_mutex;
};

} // namespace sight::module::ui

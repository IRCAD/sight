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

#include <core/notification/base.hpp>
#include <core/notification/instruction.hpp>
#include <core/notification/message.hpp>
#include <core/notification/monitor.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>
#include <ui/__/editor.hpp>
#include <ui/qt/widget/notifications_store.hpp>
#include <ui/qt/widget/progress_bar.hpp>

#include <QLabel>
#include <QMediaPlayer>
#include <QPointer>
#include <QStackedWidget>
#include <QTimer>

#include <memory>
#include <string_view>

namespace sight::module::ui::qt
{

class acknowledger;

/**
 * @brief A simple notification/message zone service implemented as a sight::ui::editor
 *
 * Notifications are dispatched by their dynamic type:
 * - sight::core::notification::instruction: lowest priority, only one displayed at a time (a new instruction
 *   replaces the text of the one currently displayed in place; an empty text closes it), no timeout.
 * - sight::core::notification::information: low priority, stacks with itself, default timeout (configurable).
 * - sight::core::notification::monitor (observer/aggregator): progress display, same behavior as before.
 * - sight::core::notification::warning: normal priority, stacks with itself, default timeout (configurable).
 * - sight::core::notification::error: highest priority, stacks with itself, no timeout, must be acknowledged.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::notification_zone" >
       <properties information_duration="5000" warning_duration="5000" maximal_height="100"
 * show_progress_title="true" show_progress_cancel="true" progress_pulse="false" progress_expand="true"
 * progress_svg_path="icons/progress.svg" progress_svg_size="16" />
   </service>
   @endcode
 *
 * @subsection Signals Signals
 * - \b error: emitted when an error notification is displayed.
 * - \b error_acknowledged: emitted when an error notification is acknowledged.
 * - \b progress: emitted when a progress notification is displayed.
 * - \b progress_acknowledged: emitted when a progress notification is acknowledged.
 *
 * @subsection Slots Slots
 * - \b clear: clear all notifications.
 * - \b add_notification: display a notification (sight::core::notification::base::sptr), dispatched by dynamic
 *   type. This is the preferred entry point.
 * - \b add_monitor: @deprecated display a job's progress and result as notifications. Prefer add_notification.
 *
 * @subsection Properties Properties:
 * - \b m_information_duration [sight::data::integer]: duration in milliseconds to display information
 *   notifications.
 * - \b m_warning_duration [sight::data::integer]: duration in milliseconds to display warning notifications.
 * - \b m_maximal_height [sight::data::integer]: maximal height of the notification zone in pixels.
 * - \b m_show_progress_title [sight::data::boolean]: if true, show the title of progress notifications.
 * - \b m_show_progress_cancel [sight::data::boolean]: if true, show the cancel button of progress notifications.
 * - \b m_progress_pulse [sight::data::boolean]: if true, the progress bar is in pulse mode.
 * - \b m_progress_expand [sight::data::boolean]: if true, the progress bar expands to fill the width of the
 * notification zone.
 * - \b m_progress_svg_path [sight::data::string]: path to the SVG icon used in progress notifications.
 * - \b m_progress_svg_size [sight::data::integer]: size of the SVG icon used in progress notifications.
 */
class notification_zone final : public sight::ui::editor,
                                public sight::ui::qt::widget::notifications_store
{
public:

    SIGHT_DECLARE_SERVICE(notification_zone, sight::ui::editor);

    notification_zone();
    ~notification_zone() noexcept final = default;

    struct signals final
    {
        inline static const signal_key_t ERROR                 = "error";
        inline static const signal_key_t ERROR_ACKNOWLEDGED    = "error_acknowledged";
        inline static const signal_key_t PROGRESS              = "progress";
        inline static const signal_key_t PROGRESS_ACKNOWLEDGED = "progress_acknowledged";
        using notification_t = sight::core::com::signal<void (sight::core::notification::base::sptr)>;
    };

    struct slots final
    {
        static inline const slot_key_t CLEAR            = "clear";
        static inline const slot_key_t ADD_NOTIFICATION = "add_notification";

        /// @deprecated Use ADD_NOTIFICATION instead.
        static inline const slot_key_t ADD_MONITOR = "add_monitor";
    };

    /// SLOT: clear all notifications
    void clear();

    /// SLOT: display a notification, dispatched by its dynamic type. This is the preferred entry point.
    void add_notification(sight::core::notification::base::sptr _notification);

    /**
     * @brief SLOT: display a job's progress and result as notifications
     * @deprecated Kept for backward compatibility. Prefer add_notification().
     */
    void add_monitor(sight::core::notification::base::sptr _notification);

protected:

    /// Does nothing
    void configuring() final;

    /// Build UI
    void starting() final;

    /// Does nothing
    void updating() final;

    /// Destroy UI
    void stopping() final;

    /// Does nothing
    void swapping(std::string_view _key) final;

protected:

    /// notifications_store: close the page of a message that reached the canceled/finished state.
    void on_notification_finished(const sight::core::notification::base::wptr& _notification) final;

    /// notifications_store: refresh the label of a message page whose content was updated.
    void on_notification_changed(const sight::core::notification::message::wptr& _notification) final;

private:

    friend class sight::module::ui::qt::acknowledger;

    /// Qt widgets
    QPointer<QStackedWidget> m_stack {nullptr};

    struct page final
    {
        QPointer<QWidget> m_widget {nullptr};
        QPointer<QLabel> m_label {nullptr};
        QPointer<QTimer> m_timer {nullptr};

        /// Set (owning) for message/instruction pages only. Monitors must stay unowned or they can never
        /// reach the finished state (has_monitors::observe() relies on the caller's sptr being dropped).
        sight::core::notification::base::sptr m_notification;

        /// Weak identity for monitor pages (progress_bar already tracks them without owning them).
        sight::core::notification::base::wptr m_notification_weak;

        std::shared_ptr<sight::ui::qt::widget::progress_bar> m_progress {nullptr};
    };

    std::map<QWidget*, page> m_pages;

    std::unique_ptr<QMediaPlayer> m_sound;

    /// Recompute and apply top-most page (message, type property, visibility) and emit signal
    void refresh_top();

    /// Create a new page for the given notification
    notification_zone::page& create_page(const sight::core::notification::base::sptr& _notification);

    /// Remove a page: takes it off the stack, stops/deletes its timer, forgets its notification, erases the
    /// map entry, emits ERROR_ACKNOWLEDGED/PROGRESS_ACKNOWLEDGED when applicable, then refreshes the top page.
    void close_page(std::map<QWidget*, page>::iterator _it);

    /// Dispatch a monitor notification: same behavior as the deprecated add_monitor() slot.
    void handle_monitor(const sight::core::notification::monitor::sptr& _job);

    /// Dispatch an instruction notification: singleton, replace-in-place, empty text closes it.
    void handle_instruction(const sight::core::notification::instruction::sptr& _instruction);

    /// Dispatch an information/warning/error notification: stacking, timed (unless error), sound.
    void handle_message(const sight::core::notification::message::sptr& _message);

    /// Resolve the display duration (in milliseconds) for a message, 0 meaning no timeout.
    [[nodiscard]] int resolve_duration_ms(const sight::core::notification::message::sptr& _message) const;

    /// Play the sound associated with the given notification's type.
    void play_sound(const sight::core::notification::base::sptr& _notification) const;

    sight::data::property<sight::data::integer> m_information_duration {this, "information_duration", 3000};
    sight::data::property<sight::data::integer> m_warning_duration {this, "warning_duration", 3000};
    sight::data::property<sight::data::integer> m_maximal_height {this, "maximal_height", 100};

    sight::data::property<sight::data::boolean> m_show_progress_title {this, "show_progress_title", true};
    sight::data::property<sight::data::boolean> m_show_progress_cancel {this, "show_progress_cancel", true};
    sight::data::property<sight::data::boolean> m_progress_pulse {this, "progress_pulse", false};
    sight::data::property<sight::data::boolean> m_progress_expand {this, "progress_expand", false};
    sight::data::property<sight::data::string> m_progress_svg_path {this, "progress_svg_path", std::string()};
    sight::data::property<sight::data::integer> m_progress_svg_size {this, "progress_svg_size", 10};
};

} // namespace sight::module::ui::qt

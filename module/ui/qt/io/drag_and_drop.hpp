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

#include <core/com/slot.hpp>
#include <core/notification/base.hpp>
#include <core/notification/has_monitors.hpp>
#include <core/thread/worker.hpp>

#include <data/series_set.hpp>

#include <service/controller.hpp>

#include <QEvent>
#include <QObject>
#include <QPointer>
#include <QWidget>

#include <mutex>

namespace sight::module::ui::qt::io
{

/**
 * @brief Enables loading files and folders by drag and drop on a Qt widget.
 *
 * @section Signals Signals
 * - \b notification_created(core::notification::base::sptr): forwards progress
 *   notifications emitted by the reader selected for a dropped path.
 *
 * @section XML XML configuration
 * @code{.xml}
 * <service uid="..." type="sight::module::ui::qt::io::drag_and_drop">
 *     <config wid="..." />
 *     <inout key="data_image" uid="..." />
 * </service>
 * @endcode
 *
 * @subsection Configuration Configuration
 * - \b wid (mandatory, string): WID of the Qt widget on which drag and drop is
 *   enabled.
 *
 * @subsection InOut In-Out
 * - \b data_image (mandatory, sight::data::series_set): series set populated
 *   from dropped files or folders.
 */

class drag_and_drop final : public QObject,
                            public service::controller,
                            public core::notification::has_monitors
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(drag_and_drop, sight::service::controller);

    struct slots
    {
        using forward_notification_t = core::com::slot<void (core::notification::base::sptr)>;

        static inline const std::string FORWARD_NOTIFICATION = "forward_notification";
    };

    /// Initializes the slot and signals.
    drag_and_drop() noexcept;

    /// Cleans ressources.
    ~drag_and_drop() noexcept final = default;

    bool eventFilter(QObject* _obj, QEvent* _event) final;

protected:

    /// Configures the editor.
    void configuring() final;

    /// Creates layouts.
    void starting() final;

    /// Update layouts.
    void updating() final;

    /// Disconnects connections.
    void stopping() final;

private:

    /// Forwards notifications created by the selected reader.
    void forward_notification(core::notification::base::sptr _notification);

    sight::sptr<slots::forward_notification_t> m_slot_forward_notification;
    core::thread::worker::sptr m_read_worker;
    QPointer<QWidget> m_widget;
    std::string m_wid;
    sight::data::ptr<sight::data::series_set, data::access::inout> m_data {this, "data_image"};
    std::mutex m_mutex;
};

} // namespace sight::module::ui::qt::io

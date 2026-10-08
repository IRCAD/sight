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

#include <data/string.hpp>

#include <service/controller.hpp>

#include <QEvent>
#include <QObject>
#include <QPointer>
#include <QWidget>

#include <deque>
#include <filesystem>
#include <mutex>
#include <vector>

namespace sight::module::ui::qt::io
{

/**
 * @brief Enables loading files and folders by drag and drop on a Qt widget.
 *
 * @section XML XML configuration
 * @code{.xml}
 * <service uid="..." type="sight::module::ui::qt::io::drag_and_drop">
 *     <inout key="path" uid="input_path" />
 *     <config wid="..." />
 * </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b path [sight::data::string]: receives each exact local path from a drop.
 *
 * @subsection Configuration Configuration
 * - \b wid (mandatory, string): WID of the Qt widget on which drag and drop is
 *   enabled.
 *
 */

class drag_and_drop : public QObject,
                      public service::controller
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(drag_and_drop, sight::service::controller);

    struct slots
    {
        static inline const slot_key_t NEXT_PATH = "next_path";
    };

    /// Initializes the slot and signals.
    drag_and_drop() noexcept;

    /// Cleans ressources.
    ~drag_and_drop() noexcept override = default;

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

    /**
     * @brief Sends the next path from the pending paths queue to the connected slot.
     */
    void send_next_path();

    data::ptr<data::string, data::access::inout> m_path {this, "path"};
    QPointer<QWidget> m_widget;
    std::string m_wid;
    std::deque<std::filesystem::path> m_pending_paths;
    bool m_path_in_progress {false};
};

} // namespace sight::module::ui::qt::io

/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/ui/debug/config.hpp"

#include <core/com/connection.hpp>
#include <core/memory/buffer_manager.hpp>
#include <core/tools/failed.hpp>

#include <ui/__/editor.hpp>

#include <QFutureWatcher>
#include <QPointer>
#include <QPushButton>
#include <QTableView>
#include <QTableWidget>

#include <vector>

class QTimer;

namespace sight::core::com
{

template<typename F>
class Slot;

} // namespace sight::core::com

namespace sight::module::ui::debug
{

/**
 * @brief   Editor to dump or restore selected buffer.
 */
class MODULE_UI_DEBUG_CLASS_API DumpEditor : public QObject,
                                             public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(DumpEditor, sight::ui::editor);

    /// Constructor. Does nothing.
    MODULE_UI_DEBUG_API DumpEditor() noexcept;

    /// Destructor. Does nothing.
    MODULE_UI_DEBUG_API ~DumpEditor() noexcept override;

protected:

    /// Install the layout and call updating() method
    void starting() override;

    /// Stop the layout.
    void stopping() override;

    /// Update the choice selection
    void updating() override;

    /// Calls classic action methods to configure
    void configuring() override;

    /// Overrides. Does nothing.
    void info(std::ostream& _sstream) override;

    /// Start m_updateTimer, call on buffManager signal emit ( see m_refreshSignal )
    void onUpdate();

protected Q_SLOTS:

    /// This method is called when an item is pressed.
    void changeStatus(int /*index*/);

    /// Slot called when user click on button m_refresh, call updating() method
    void onRefreshButton();

    void on_buffer_info();

private:

    typedef core::com::slot<void ()> UpdateSlotType;

    QFutureWatcher<core::memory::buffer_manager::buffer_info_map_t> m_watcher;

    // Managed buffers
    std::vector<const void* const*> m_objectsUID;

    /// Widget to print some information on managed buffer by system
    QTableWidget* m_list {};

    /// Button to force refresh
    QPushButton* m_refresh {};

    /// Editor to manage dump policy
    QTableView* m_policyEditor {};

    /// Editor to show few memory information
    QTableView* m_infoEditor {};

    SPTR(UpdateSlotType) m_updateSlot;

    /// Manage connection between buffManager updated signal and onUpdate method
    core::com::connection m_connection;

    /// Timer use to call each 300 ms onRefreshButton() slot
    QPointer<QTimer> m_updateTimer;
};

} // namespace sight::module::ui::debug

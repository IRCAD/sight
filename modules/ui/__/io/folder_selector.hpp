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

#include "modules/ui/__/config.hpp"

#include <ui/__/action.hpp>

#include <filesystem>

namespace sight::module::ui::io
{

/**
 * @brief   Raise a pop-up dialog to select a directory, then send it via a signal.
 *
 * @section Signals Signals
 * - \b folderSelector(std::filesystem::path) : Emitted when the directory is chosen.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::io::folder_selector" >
            <dialogTitle>Select a folder</dialogTitle>
        </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b dialogTitle (optional): title of the dialog.
 */
class MODULE_UI_CLASS_API folder_selector : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(folder_selector, sight::ui::action);

    /**
     * @name Signals API
     * @{
     */
    typedef core::com::signal<void (std::filesystem::path)> folder_selected_signal_t;
    static const core::com::signals::key_t FOLDER_SELECTED_SIG;
    /// @}

    MODULE_UI_API folder_selector() noexcept;
    MODULE_UI_API ~folder_selector() noexcept override;

protected:

    /// Do nothing
    MODULE_UI_API void configuring() override;

    /// Do nothing.
    MODULE_UI_API void starting() override;

    /// Show the dialog and send the signal with the chosen directory
    MODULE_UI_API void updating() override;

    /// Do nothing.
    MODULE_UI_API void stopping() override;

private:

    /// Optional title for the dialog
    std::string m_dialogTitle;
};

} // namespace sight::module::ui::io

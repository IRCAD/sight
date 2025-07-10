/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
class folder_selector : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(folder_selector, sight::ui::action);

    /**
     * @name Signals API
     * @{
     */
    using folder_selected_signal_t = core::com::signal<void (std::filesystem::path)>;
    static const core::com::signals::key_t FOLDER_SELECTED_SIG;
    /// @}

    folder_selector() noexcept;
    ~folder_selector() noexcept override;

protected:

    /// Do nothing
    void configuring() override;

    /// Do nothing.
    void starting() override;

    /// Show the dialog and send the signal with the chosen directory
    void updating() override;

    /// Do nothing.
    void stopping() override;

private:

    /// Optional title for the dialog
    std::string m_dialog_title;
};

} // namespace sight::module::ui::io

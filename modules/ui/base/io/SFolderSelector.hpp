/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/base/config.hpp"

#include <ui/base/IAction.hpp>

#include <filesystem>

namespace sight::module::ui::base
{

namespace io
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
        <service type="sight::module::ui::base::io::SFolderSelector" >
            <dialogTitle>Select a folder</dialogTitle>
        </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b dialogTitle (optional): title of the dialog.
 */
class MODULE_UI_BASE_CLASS_API SFolderSelector : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SFolderSelector, sight::ui::base::IAction);

    /**
     * @name Signals API
     * @{
     */
    typedef core::com::Signal<void (std::filesystem::path)> FolderSelectedSignalType;
    static const core::com::Signals::SignalKeyType s_FOLDER_SELECTED_SIG;
    /// @}

    MODULE_UI_BASE_API SFolderSelector() noexcept;
    MODULE_UI_BASE_API virtual ~SFolderSelector() noexcept;

protected:

    /// Do nothing
    MODULE_UI_BASE_API void configuring() override;

    /// Do nothing.
    MODULE_UI_BASE_API void starting() override;

    /// Show the dialog and send the signal with the chosen directory
    MODULE_UI_BASE_API void updating() override;

    /// Do nothing.
    MODULE_UI_BASE_API void stopping() override;

private:

    /// Optional title for the dialog
    std::string m_dialogTitle;
};

} // namespace io

} // namespace sight::module::ui::base

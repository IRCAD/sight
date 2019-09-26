/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "uiIO/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

namespace uiIO
{
namespace action
{

/**
 * @brief   Raise a pop-up dialog to select a directory, then send it via a signal.
 *
 * @section Signals Signals
 * - \b folderSelector(::boost::filesystem::path) : Emitted when the directory is chosen.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::uiIO::action::SFolderSelector" >
            <dialogTitle>Select a folder</dialogTitle>
        </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b dialogTitle (optional): title of the dialog.
 */
class UIIO_CLASS_API SFolderSelector : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceMacro(SFolderSelector,  ::fwGui::IActionSrv);

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( ::boost::filesystem::path ) > FolderSelectedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_FOLDER_SELECTED_SIG;
    /// @}

    UIIO_API SFolderSelector() noexcept;
    UIIO_API virtual ~SFolderSelector() noexcept;

protected:

    /// Do nothing
    UIIO_API void configuring() override;

    /// Do nothing.
    UIIO_API void starting() override;

    /// Show the dialog and send the signal with the chosen directory
    UIIO_API void updating() override;

    /// Do nothing.
    UIIO_API void stopping() override;

private:

    /// Optional title for the dialog
    std::string m_dialogTitle;
};

} // namespace action
} // namespace uiIO

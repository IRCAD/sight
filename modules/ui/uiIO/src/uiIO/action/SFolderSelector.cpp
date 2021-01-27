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

#include "uiIO/action/SFolderSelector.hpp"

#include <core/com/Signal.hxx>

#include <data/location/Folder.hpp>

#include <gui/dialog/LocationDialog.hpp>

#include <services/macros.hpp>

namespace uiIO
{
namespace action
{

fwServicesRegisterMacro( ::sight::gui::IActionSrv, ::uiIO::action::SFolderSelector, ::sight::data::Object )

const core::com::Signals::SignalKeyType SFolderSelector::s_FOLDER_SELECTED_SIG = "folderSelected";

//------------------------------------------------------------------------------

SFolderSelector::SFolderSelector( ) noexcept
{
    newSignal< FolderSelectedSignalType >( s_FOLDER_SELECTED_SIG );
}

//------------------------------------------------------------------------------

SFolderSelector::~SFolderSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SFolderSelector::configuring()
{
    const auto& config = this->getConfigTree();
    m_dialogTitle = config.get("dialogTitle", "Select a folder");
}

//------------------------------------------------------------------------------

void SFolderSelector::starting()
{
}

//------------------------------------------------------------------------------

void SFolderSelector::updating( )
{
    static std::filesystem::path _sDefaultPath("");
    gui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_dialogTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(gui::dialog::ILocationDialog::READ);
    dialogFile.setType(gui::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result;
    result = data::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        auto sig = this->signal<FolderSelectedSignalType>(s_FOLDER_SELECTED_SIG);
        sig->asyncEmit(result->getFolder());
    }
}

//------------------------------------------------------------------------------

void SFolderSelector::stopping()
{
}

//------------------------------------------------------------------------------
//
} // namespace action
} // namespace uiIO

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiIO/action/SFolderSelector.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/location/Folder.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>

namespace uiIO
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiIO::action::SFolderSelector, ::fwData::Object );

const ::fwCom::Signals::SignalKeyType SFolderSelector::s_FOLDER_SELECTED_SIG = "folderSelected";

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
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_dialogTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
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

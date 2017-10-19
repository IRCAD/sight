/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "monitor/action/SDumpAll.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/BufferInfo.hpp>

#include <fwServices/macros.hpp>

namespace monitor
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::monitor::action::SDumpAll, ::fwData::Object );

//------------------------------------------------------------------------------

SDumpAll::SDumpAll( ) noexcept
{
}

//------------------------------------------------------------------------------

SDumpAll::~SDumpAll() noexcept
{
}

//------------------------------------------------------------------------------

void SDumpAll::updating( )
{
    size_t nbBuffDumped = 0;
    ::fwMemory::BufferManager::BufferInfoMapType buffInfoMap;
    ::fwMemory::BufferManager::sptr buffManager = ::fwMemory::BufferManager::getDefault();
    if(buffManager)
    {
        buffInfoMap = buffManager->getBufferInfos().get();
    }
    for(::fwMemory::BufferManager::BufferInfoMapType::value_type elt :  buffInfoMap)
    {
        ::fwMemory::BufferInfo dumpBuffInfo = elt.second;
        bool loaded = dumpBuffInfo.loaded;
        bool isLock = dumpBuffInfo.lockCount() > 0;
        if(loaded && !isLock)
        {
            bool dumped = buffManager->dumpBuffer(elt.first).get();
            if(dumped)
            {
                ++nbBuffDumped;
            }
        }
    }
    std::stringstream stream;
    stream << nbBuffDumped << " buffer dumped (" << nbBuffDumped <<"/"<<buffInfoMap.size()<<").";
    ::fwGui::dialog::MessageDialog::showMessageDialog(
        "Dump all",
        stream.str(),
        ::fwGui::dialog::IMessageDialog::INFO);
}

//------------------------------------------------------------------------------

void SDumpAll::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SDumpAll::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------
void SDumpAll::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace monitor

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/BufferInfo.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "monitor/action/SDumpAll.hpp"

namespace monitor
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::monitor::action::SDumpAll , ::fwData::Object ) ;

//------------------------------------------------------------------------------

SDumpAll::SDumpAll( ) throw()
{}

//------------------------------------------------------------------------------

SDumpAll::~SDumpAll() throw()
{}

//------------------------------------------------------------------------------

void SDumpAll::updating( ) throw(::fwTools::Failed)
{
    size_t nbBuffDumped = 0;
    ::fwMemory::BufferManager::BufferInfoMapType buffInfoMap;
    ::fwMemory::BufferManager::sptr buffManager = ::fwMemory::BufferManager::getCurrent();
    if(buffManager)
    {
        buffInfoMap = buffManager->getBufferInfos();
    }
    BOOST_FOREACH(::fwMemory::BufferManager::BufferInfoMapType::value_type elt, buffInfoMap)
    {
        ::fwMemory::BufferInfo dumpBuffInfo = elt.second;
        bool loaded = dumpBuffInfo.loaded;
        bool isLock = dumpBuffInfo.lockCount() > 0;
        if(loaded && !isLock)
        {
            bool dumped = buffManager->dumpBuffer(elt.first);
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

void SDumpAll::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SDumpAll::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------
void SDumpAll::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace monitor

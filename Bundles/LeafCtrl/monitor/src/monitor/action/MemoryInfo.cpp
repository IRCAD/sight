/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include <fwServices/macros.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/tools/MemoryMonitorTools.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "monitor/action/MemoryInfo.hpp"

namespace monitor
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::monitor::action::MemoryInfo , ::fwData::Object ) ;

//------------------------------------------------------------------------------

MemoryInfo::MemoryInfo( ) throw()
{}

//------------------------------------------------------------------------------

MemoryInfo::~MemoryInfo() throw()
{}

//------------------------------------------------------------------------------


void MemoryInfo::updating( ) throw(::fwTools::Failed)
{
    // Memory information
    ::boost::uint64_t mo = 1024*1024;
    ::boost::uint64_t totalSystemMemory     = ::fwMemory::tools::MemoryMonitorTools::getTotalSystemMemory();
    ::boost::uint64_t freeSystemMemory      = ::fwMemory::tools::MemoryMonitorTools::getFreeSystemMemory();
    ::boost::uint64_t usedProcessMemory     = ::fwMemory::tools::MemoryMonitorTools::getUsedProcessMemory();
    ::boost::uint64_t estimateFreeMem       = ::fwMemory::tools::MemoryMonitorTools::estimateFreeMem();

    ::fwMemory::BufferManager::SizeType managedBufferSize = 0;
    ::fwMemory::BufferManager::SizeType dumpedBufferSize = 0;
    ::fwMemory::BufferManager::sptr manager;
    manager = ::fwMemory::BufferManager::dynamicCast( ::fwTools::IBufferManager::getCurrent() );

    if( manager )
    {
        managedBufferSize = manager->getManagedBufferSize();
        dumpedBufferSize  = manager->getDumpedBufferSize();
    }
    std::stringstream stream;
    stream << "Total system memory = "  << totalSystemMemory/mo << " Mo" << std::endl;
    stream << "Free system memory  = "   << freeSystemMemory/mo  << " Mo" << std::endl;
    stream << "Used process memory = "  << usedProcessMemory/mo << " Mo" << std::endl;
    stream << "Estimed Free memory = "  << estimateFreeMem/mo << " Mo" << std::endl;
    stream << "ManagedBuffer size  = "  << managedBufferSize/mo << " Mo" << std::endl;
    stream << "DumpedBuffer size   = "  << dumpedBufferSize/mo << " Mo" << std::endl;

    // Information message box
    ::fwGui::dialog::MessageDialog::showMessageDialog(
            "MemoryInfo",
            stream.str(),
            ::fwGui::dialog::IMessageDialog::INFO);
}

//------------------------------------------------------------------------------

void MemoryInfo::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void MemoryInfo::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void MemoryInfo::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace basicVisuCtrl

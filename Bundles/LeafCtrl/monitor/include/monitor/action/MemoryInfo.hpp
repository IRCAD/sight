/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef MONITOR_MEMORYINFO_H_
#define MONITOR_MEMORYINFO_H_

#include <fwGui/IActionSrv.hpp>

#include "monitor/config.hpp"

namespace monitor
{
namespace action
{

/// Implements an action to show in a message box few memory information
class MONITOR_CLASS_API MemoryInfo : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (MemoryInfo)( ::fwGui::IActionSrv ) ) ;

    /// Does nothing
    MONITOR_API MemoryInfo() throw() ;

    /// Does nothing
    MONITOR_API virtual ~MemoryInfo() throw() ;

protected :

    /// Calls classic IAction methods to configure
    void configuring() throw (::fwTools::Failed);

    /// Calls classic IAction methods to start
    void starting() throw (::fwTools::Failed);

    /// Shows in a message box few memory information
    void updating() throw (::fwTools::Failed);

    /// Calls classic IAction methods to stop
    void stopping() throw (::fwTools::Failed);

};

} // namespace action
} // namespace monitor

#endif /*MONITOR_MEMORYINFO_H_*/

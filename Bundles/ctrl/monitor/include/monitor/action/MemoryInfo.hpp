/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITOR_ACTION_MEMORYINFO_HPP__
#define __MONITOR_ACTION_MEMORYINFO_HPP__

#include "monitor/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace monitor
{
namespace action
{

/// Implements an action to show in a message box few memory information
class MONITOR_CLASS_API MemoryInfo : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (MemoryInfo)( ::fwGui::IActionSrv ) );

    /// Does nothing
    MONITOR_API MemoryInfo() noexcept;

    /// Does nothing
    MONITOR_API virtual ~MemoryInfo() noexcept;

protected:

    /// Calls classic IAction methods to configure
    void configuring() override;

    /// Calls classic IAction methods to start
    void starting() override;

    /// Shows in a message box few memory information
    void updating() override;

    /// Calls classic IAction methods to stop
    void stopping() override;

};

} // namespace action
} // namespace monitor

#endif // __MONITOR_ACTION_MEMORYINFO_HPP__


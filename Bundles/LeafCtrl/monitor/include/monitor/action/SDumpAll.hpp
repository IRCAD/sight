/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITOR_ACTION_SDUMPALL_HPP__
#define __MONITOR_ACTION_SDUMPALL_HPP__

#include "monitor/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace monitor
{
namespace action
{

/// Implements an action that dump all buffer (if possible)
class MONITOR_CLASS_API SDumpAll : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SDumpAll)(::fwGui::IActionSrv) );

    /// Does nothing
    MONITOR_API SDumpAll() noexcept;

    /// Does nothing
    MONITOR_API virtual ~SDumpAll() noexcept;

protected:

    /// Calls classic IAction methods to configure
    void configuring() override;

    /// Calls classic IAction methods to start
    void starting() override;

    /// Dump all unlocked array/image etc
    void updating() override;

    /// Calls classic IAction methods to stop
    void stopping() override;

};

} // namespace action
} // namespace monitor

#endif //__MONITOR_ACTION_SDUMPALL_HPP__


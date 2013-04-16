/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MONITOR_UI_ACTION_SDUMPALL_HPP_
#define _MONITOR_UI_ACTION_SDUMPALL_HPP_


#include <fwGui/IActionSrv.hpp>

#include "monitor/config.hpp"

namespace monitor
{
namespace action
{


/// Implements an action that dump all buffer (if possible)
class MONITOR_CLASS_API SDumpAll : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SDumpAll)(::fwGui::IActionSrv) ) ;

    /// Does nothing
    MONITOR_API SDumpAll() throw() ;

    /// Does nothing
    MONITOR_API virtual ~SDumpAll() throw() ;

protected :

    /// Calls classic IAction methods to configure
    void configuring() throw (::fwTools::Failed);

    /// Calls classic IAction methods to start
    void starting() throw (::fwTools::Failed);

    /// Dump all unlocked array/image etc
    void updating() throw (::fwTools::Failed);

    /// Calls classic IAction methods to stop
    void stopping() throw (::fwTools::Failed);

};


} // namespace action
} // namespace monitor


#endif //_MONITOR_UI_ACTION_SDUMPALL_HPP_

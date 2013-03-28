/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MONITOR_ACTION_MEMORYCONSUMPTION_HPP_
#define _MONITOR_ACTION_MEMORYCONSUMPTION_HPP_

#include <fwGui/IActionSrv.hpp>

#include "monitor/config.hpp"

namespace monitor
{
namespace action
{

/// Increase or decrease the memory consumption by storing a new image, use to experiment dump process
class MONITOR_CLASS_API MemoryConsumption : public  ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (MemoryConsumption)(  ::fwGui::IActionSrv ) ) ;

    /// Does nothing
    MONITOR_API MemoryConsumption() throw() ;

    /// Does nothing
    MONITOR_API virtual ~MemoryConsumption() throw() ;

protected :

    /**
     * @brief Implements configuring method derived from IService. .
     *
     * Sample of declaration configuration :
     *
     * @verbatim
         <service uid="myIncMemConsum"  type="::fwGui::IActionSrv" implementation="::monitor::action::MemoryConsumption" autoComChannel="no" >
            <config mode="increase" value="256" />
        </service>
     @endverbatim
     * With:
     * @li mode : increase or decrease.
     * @li value : optional, consuming memory size in Mo (in "increase" mode).
     */
    void configuring() throw (::fwTools::Failed);

    /// Calls classic IAction methods to start
    void starting() throw (::fwTools::Failed);

    /// Increase or decrease memory with size defined during configuration
    void updating() throw (::fwTools::Failed);

    /// Calls classic IAction methods to stop
    void stopping() throw (::fwTools::Failed);

private:

    /// Intern method to create a new array
    void pushNewArray(size_t memorySizeInBytes);

    /// To manage allocation or free memory ( data are saved in a static member )
    bool m_isIncreaseMode;

    /// Memory allocated in "increase" mode
    size_t m_memorySizeInBytes;

};


} // namespace action
} // namespace monitor


#endif //_MONITOR_ACTION_COMPONENTSTREE_HPP_

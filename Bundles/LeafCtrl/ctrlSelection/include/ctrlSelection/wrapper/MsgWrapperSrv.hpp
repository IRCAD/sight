/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_MANAGER_MSGWRAPPERSRV_HPP_
#define _CTRLSELECTION_MANAGER_MSGWRAPPERSRV_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IWrapperSrv.hpp"

namespace ctrlSelection
{

namespace wrapper
{

/**
 * @class  MsgWrapperSrv.
 * @brief  Convert generic message by specific message
 * @author IRCAD (Research and Development Team).

 * @date   2007-2009.
 */
class CTRLSELECTION_CLASS_API MsgWrapperSrv : public ::ctrlSelection::IWrapperSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (MsgWrapperSrv)(::ctrlSelection::IWrapperSrv) ) ;

//    /// Constructor.  Do nothing.
    CTRLSELECTION_API MsgWrapperSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~MsgWrapperSrv() throw();

protected:

    /// Implements starting method derived from IService. Do nothing.
   CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed ) {};

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed ) {};

    /**
     * Convert message with event by a specific message with another event and type.
     *
     * @verbatim
     <wrapper onEvent="UPDATED_OBJECT" toEvent="NEW_IMAGE" toMsgType="::fwComEd::ImageMsg" />
     @endverbatim
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed ) {};

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed ) {};

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream ) {};

    /**
     * @brief Convert the Object message with "UPDATED_OBJECT" event sent on Graph by GraphMsg with "NEW_GRAPH" event
     */
    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    // < onEvent, toEvent, msgType >
    typedef ::boost::tuple< std::string, std::string, std::string > EventType;
    typedef std::vector < EventType > ManagedEventsType;
    ManagedEventsType m_managedEvents;
};

} // wrapper
} // ctrlSelection

#endif // _CTRLSELECTION_MANAGER_MSGWRAPPERSRV_HPP_

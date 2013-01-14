/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_MSGFORWARDERSRV_HPP_
#define _CTRLSELECTION_MSGFORWARDERSRV_HPP_

#include <boost/tuple/tuple.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/IController.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IWrapperSrv.hpp"

namespace ctrlSelection
{

namespace wrapper
{
/**
 * @class  MsgForwarderSrv.
 * @brief  This service forwards an event from specific object to object specified with key in in composite.
 * @author IRCAD (Research and Development Team).

 * @date   2011.
 */
class CTRLSELECTION_CLASS_API MsgForwarderSrv : public ::ctrlSelection::IWrapperSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (MsgForwarderSrv)(::ctrlSelection::IWrapperSrv) ) ;

    CTRLSELECTION_API MsgForwarderSrv() throw() ;

    CTRLSELECTION_API virtual ~MsgForwarderSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements swapping method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void swapping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configures the service.
     *
     * @verbatim
       <service uid="forwarderMsg" impl="::ctrlSelection::wrapper::MsgForwarderSrv" type="::ctrlSelection::IWrapperSrv" autoConnect="no">
           <forward compositeKey="frontImage" onEvent="SLICE_INDEX" fromUID="*" msgType="::fwComEd::ImageMsg" />
           <forward compositeKey="frontImage" onEvent="CHANGE_SLICE_TYPE" fromUID="*" msgType="::fwComEd::ImageMsg" />
       </service>
       @endverbatim
     * With :
     * - \b compositeKey : key of the object in the composite,
     * - \b onEvent : event of the message to receive,
     * - \b fromUID : uid of the object which sent the message. If "*" the message can be received from any object,
     * - \b msgType : type of the message to receive.
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Does nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private :

    typedef ::boost::tuple< std::string, std::string, std::string, std::string > EventType;
    typedef std::vector < EventType > ManagedEventsType;

    /// List of the managed event
    ManagedEventsType m_managedEvents;
};

} // wrapper
} // ctrlSelection

#endif // _CTRLSELECTION_MSGFORWARDERSRV_HPP_

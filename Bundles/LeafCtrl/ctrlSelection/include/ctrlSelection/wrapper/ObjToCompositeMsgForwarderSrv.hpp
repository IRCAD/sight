/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_COMPOSITEMSGFORWARDERSRV_HPP_
#define _CTRLSELECTION_COMPOSITEMSGFORWARDERSRV_HPP_

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
 * @class  ObjToCompositeMsgForwarderSrv.
 * @brief  This service forward an event from all object in composite to the composite .
 * @author IRCAD (Research and Development Team).

 * @date   2011.
 */
class CTRLSELECTION_CLASS_API ObjToCompositeMsgForwarderSrv : public ::ctrlSelection::IWrapperSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ObjToCompositeMsgForwarderSrv)(::ctrlSelection::IWrapperSrv) ) ;

protected:

    /// Constructor.  Do nothing.
    CTRLSELECTION_API ObjToCompositeMsgForwarderSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~ObjToCompositeMsgForwarderSrv() throw() ;

    /// Implements starting method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements swapping method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void swapping()  throw ( ::fwTools::Failed );

    /// Implements configuring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private :

    // < fromKey, onEvent, msgType >
    typedef ::boost::tuple< std::string, std::string, std::string > EventType;
    typedef std::vector < EventType > ManagedEventsType;

    /// Map to register objects comChannels
    typedef std::map< std::string, ::fwServices::IService::wptr > ObjComChannelMap;
    ObjComChannelMap m_objComChannel;
    /// List of the managed event
    ManagedEventsType m_managedEvents;
};

} // wrapper
} // ctrlSelection

#endif // _CTRLSELECTION_COMPOSITEMSGFORWARDERSRV_HPP_

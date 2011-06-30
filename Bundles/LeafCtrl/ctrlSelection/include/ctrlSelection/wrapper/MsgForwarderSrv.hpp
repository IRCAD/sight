/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
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
 * @brief  This service forward an event from specific object to object specified with key in in composite.
 * @author IRCAD (Research and Development Team).

 * @date   2011.
 */
class CTRLSELECTION_CLASS_API MsgForwarderSrv : public ::ctrlSelection::IWrapperSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (MsgForwarderSrv)(::ctrlSelection::IWrapperSrv) ) ;

protected:

    /// Constructor.  Do nothing.
    CTRLSELECTION_API MsgForwarderSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~MsgForwarderSrv() throw() ;

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

    typedef ::boost::tuple< std::string, std::string, std::string, std::string > EventType;
    typedef std::vector < EventType > ManagedEventsType;

    /// List of the managed event
    ManagedEventsType m_managedEvents;
};

} // wrapper
} // ctrlSelection

#endif // _CTRLSELECTION_MSGFORWARDERSRV_HPP_

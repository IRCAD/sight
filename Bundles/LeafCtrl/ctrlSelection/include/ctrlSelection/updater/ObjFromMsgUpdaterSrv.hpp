/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_OBJFROMMSGUPDATERSRV_HPP_
#define _CTRLSELECTION_OBJFROMMSGUPDATERSRV_HPP_

#include <boost/tuple/tuple.hpp>

#include <fwData/Acquisition.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class   ObjFromMsgUpdaterSrv : Update Object from message
 * @author IRCAD (Research and Development Team).

 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API ObjFromMsgUpdaterSrv : public ::ctrlSelection::IUpdaterSrv
{

public :

    enum ActionType {
        ADD,
        SWAP,
        REMOVE,
        ADD_OR_SWAP
    };

    fwCoreServiceClassDefinitionsMacro ( (ObjFromMsgUpdaterSrv)(::ctrlSelection::IUpdaterSrv::Baseclass) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API ObjFromMsgUpdaterSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~ObjFromMsgUpdaterSrv() throw() ;

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements configuring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private :

    void updateComposite( ::fwData::Object::sptr _obj, std::string _compositeKey, ActionType _action );

    typedef std::vector < ::boost::tuple< std::string, std::string, std::string, ActionType > > ManagedEvents;

    ManagedEvents m_managedEvents;
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_OBJFROMMSGUPDATERSRV_HPP_

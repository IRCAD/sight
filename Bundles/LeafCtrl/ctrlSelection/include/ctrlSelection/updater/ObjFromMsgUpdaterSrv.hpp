/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_OBJFROMMSGUPDATERSRV_HPP_
#define _CTRLSELECTION_OBJFROMMSGUPDATERSRV_HPP_

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class  ObjFromMsgUpdaterSrv : Update Object from message
 * @brief  Update the composite with the object which send the message.
 * @author IRCAD (Research and Development Team).

 * @date    2009-2010.
 */
class CTRLSELECTION_CLASS_API ObjFromMsgUpdaterSrv : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ObjFromMsgUpdaterSrv)(::ctrlSelection::IUpdaterSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API ObjFromMsgUpdaterSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~ObjFromMsgUpdaterSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the services : declare the event to react.
     * @verbatim
         <service uid="myUpdater" implementation="::ctrlSelection::updater::ObjFromMsgUpdaterSrv" type="::ctrlSelection::IUpdaterSrv" autoComChannel="no">
            <update compositeKey="myImage" onEvent="NEW_IMAGE_SELECTED" fromUID="myImage" actionType="ADD_OR_SWAP"/>
            <update compositeKey="myObject" onEvent="NEW_OBJECT" fromUID="myObject" actionType="ADD_OR_SWAP"/>
            <update compositeKey="myObject" onEvent="REMOVE_OBJECT" fromUID="myObject" actionType="REMOVE"/>
        </service>
      @endverbatim
     * @see IUpdaterSrv::configureManagedEvents(::fwRuntime::ConfigurationElement::sptr configuration);
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_OBJFROMMSGUPDATERSRV_HPP_

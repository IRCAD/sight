/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_RESECFROMPDBUPDATERSRV_HPP_
#define _CTRLSELECTION_RESECFROMPDBUPDATERSRV_HPP_

#include <fwData/PlaneList.hpp>
#include <fwData/PatientDB.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class   PlaneListFromPDBUpdaterSrv.
 * @brief  Specific updater to update PlaneList from a patientDB.
 * @author IRCAD (Research and Development Team).

 * @date    2009-2010.
 */
class CTRLSELECTION_CLASS_API PlaneListFromPDBUpdaterSrv : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (PlaneListFromPDBUpdaterSrv)(::ctrlSelection::IUpdaterSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API PlaneListFromPDBUpdaterSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~PlaneListFromPDBUpdaterSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the services : declare the event to react.
     * @verbatim
         <service uid="myUpdater" implementation="::ctrlSelection::updater::PlaneListFromPDBUpdaterSrv" type="::ctrlSelection::IUpdaterSrv" autoComChannel="no">
            <update compositeKey="myPlaneList" onEvent="NEW_IMAGE_SELECTED" fromUID="myPatientDB" actionType="ADD_OR_SWAP"/>
            <update compositeKey="myPlaneList" onEvent="CLEAR_PATIENT" fromUID="myPatientDB" actionType="REMOVE"/>
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

private:

    ::fwData::PlaneList::sptr getPlaneList( ::fwData::PatientDB::sptr patientDB, ::fwServices::ObjectMsg::csptr _msg );
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_RESECFROMPDBUPDATERSRV_HPP_

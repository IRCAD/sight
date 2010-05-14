/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IMAGEFROMPDBSELECTORUPDATERSRV_HPP_
#define _CTRLSELECTION_IMAGEFROMPDBSELECTORUPDATERSRV_HPP_

#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class  ImageFromPDBSelectorUpdaterSrv.
 * @brief  Specific updater to update image from a patientDB.
 * @author IRCAD (Research and Development Team).

 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API ImageFromPDBSelectorUpdaterSrv : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ImageFromPDBSelectorUpdaterSrv)(::ctrlSelection::IUpdaterSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API ImageFromPDBSelectorUpdaterSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~ImageFromPDBSelectorUpdaterSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the services : declare the event to react.
     * @verbatim
         <service uid="myUpdaterImage" implementation="::ctrlSelection::updater::ImageFromPDBSelectorUpdaterSrv" type="::ctrlSelection::IUpdaterSrv" autoComChannel="no">
            <update compositeKey="myImage" onEvent="NEW_IMAGE_SELECTED" fromUID="myPatientDB" actionType="ADD_OR_SWAP"/>
            <update compositeKey="myImage" onEvent="CLEAR_PATIENT" fromUID="myPatientDB" actionType="REMOVE"/>
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

    ::fwData::Image::sptr getImage(::fwData::PatientDB::sptr patientDB, ::fwServices::ObjectMsg::csptr _msg);
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_IMAGEFROMPDBSELECTORUPDATERSRV_HPP_

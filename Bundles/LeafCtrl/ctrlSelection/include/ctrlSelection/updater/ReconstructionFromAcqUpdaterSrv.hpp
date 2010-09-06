/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_RECONSTRUCTIONFROMACQUPDATERSRV_HPP_
#define _CTRLSELECTION_RECONSTRUCTIONFROMACQUPDATERSRV_HPP_

#include <fwData/Reconstruction.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class   ReconstructionFromAcqUpdaterSrv.
 * @brief  Specific updater to update PlaneList from acquisition.
 * @author IRCAD (Research and Development Team).

 * @date    2009-2010.
 */
class CTRLSELECTION_CLASS_API ReconstructionFromAcqUpdaterSrv : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ReconstructionFromAcqUpdaterSrv)(::ctrlSelection::IUpdaterSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API ReconstructionFromAcqUpdaterSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~ReconstructionFromAcqUpdaterSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the services : declare the event to react.
     * @verbatim
         <service uid="myUpdater" implementation="::ctrlSelection::updater::ReconstructionFromAcqUpdaterSrv" type="::ctrlSelection::IUpdaterSrv" autoComChannel="no">
            <update compositeKey="myReconstruction" onEvent="NEW_RECONSTRUCTION" fromUID="myAcquisition" actionType="ADD_OR_SWAP"/>
            <update compositeKey="myReconstruction" onEvent="NEW_RECONSTRUCTION" fromUID="*" actionType="ADD_OR_SWAP"/>
            <update compositeKey="myReconstruction" onEvent="CLEAR" fromUID="myAcquisition" actionType="REMOVE"/>
        </service>
      @endverbatim
     * The '*' value for "fromUID" mean that the message could be received from every objects
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

    ::fwData::Reconstruction::sptr getReconstruction(::fwServices::ObjectMsg::csptr _msg);
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_RECONSTRUCTIONFROMACQUPDATERSRV_HPP_

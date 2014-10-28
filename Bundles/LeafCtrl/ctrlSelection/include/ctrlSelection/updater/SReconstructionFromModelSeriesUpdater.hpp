/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_RECONSTRUCTIONFROMMODELSERIESUPDATERSRV_HPP__
#define __CTRLSELECTION_RECONSTRUCTIONFROMMODELSERIESUPDATERSRV_HPP__

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace fwData
{
    class Reconstruction;
}

namespace ctrlSelection
{

namespace updater
{

/**
 * @class  SReconstructionFromModelSeriesUpdater

 * @date   2013
 */
class CTRLSELECTION_CLASS_API SReconstructionFromModelSeriesUpdater : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SReconstructionFromModelSeriesUpdater)(::ctrlSelection::IUpdaterSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SReconstructionFromModelSeriesUpdater() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SReconstructionFromModelSeriesUpdater() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the services : declare the event to react.
     * @verbatim
         <service uid="myUpdater" impl="::ctrlSelection::updater::SReconstructionFromModelSeriesUpdater" type="::ctrlSelection::IUpdaterSrv" autoConnect="no">
            <update compositeKey="reconstructionKey" onEvent="NEW_RECONSTRUCTION" fromUID="modelSeriesUid" actionType="ADD_OR_SWAP"/>
            <update compositeKey="reconstructionKey" onEvent="NEW_RECONSTRUCTION" fromUID="*" actionType="ADD_OR_SWAP"/>
            <update compositeKey="reconstructionKey" onEvent="CLEAR" fromUID="modelSeries" actionType="REMOVE"/>
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

    CTRLSELECTION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private:

    /**
     * @brief Retrieves the reconstruction object attached to the given message.
     * @note The message must be of class ::fwComEd::ModelSeriesMsg.
     */
    SPTR(::fwData::Reconstruction) getReconstruction(::fwServices::ObjectMsg::csptr _msg);
};

} // updater
} // ctrlSelection

#endif // __CTRLSELECTION_RECONSTRUCTIONFROMMODELSERIESUPDATERSRV_HPP__

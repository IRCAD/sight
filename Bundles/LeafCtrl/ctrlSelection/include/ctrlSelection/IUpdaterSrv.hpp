/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IUPDATERSRV_HPP_
#define _CTRLSELECTION_IUPDATERSRV_HPP_

#include <boost/tuple/tuple.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class   IUpdaterSrv.
 * @brief   An updater is attached on a composite. It is use to add, swap and remove object in the composite when it receive specific message (declared in configuration)
 * and send corresponding message message. It work mainly with a IManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API IUpdaterSrv : public ::fwServices::IService
{

public:

    /// Action to do on object in the composite
    enum ActionType {
        ADD,
        SWAP,
        REMOVE,
        DO_NOTHING,
        REMOVE_IF_PRESENT,
        ADD_OR_SWAP
    };

    fwCoreServiceClassDefinitionsMacro ( (IUpdaterSrv)(::fwServices::IService) ) ;

    ///@brief IUpdaterSrv constructor. Do nothing.
    CTRLSELECTION_API IUpdaterSrv();

    ///@brief IUpdaterSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IUpdaterSrv();

protected:

    /**
     * @brief Update the object in the composite
     * @param composite Composite which will contain the object
     * @param _obj object to add in the composite
     * @param _compositeKey key of the object in the composite
     * @param _action action to do with the object
     * @note the _obj can be null if the action is REMOVE, REMOVE_IF_PRESENT or DO_NOTHING
     */
    void updateComposite( ::fwData::Composite::sptr composite, ::fwData::Object::sptr _obj, std::string _compositeKey, ActionType _action );

    /**
     * @brief Configure the event to manage and its corresponding action
     * @param configuration configuration of the service
     *
     * Sample of declaration configuration :
     *
     * @verbatim
         <update compositeKey="myImage" onEvent="NEW_PATIENT" fromUID="myPatientDB" actionType="ADD_OR_SWAP" />
         <update compositeKey="myImage" onEvent="CLEAR_PATIENT" fromUID="myPatientDB" actionType="REMOVE" />
       @endverbatim
     * Here, when the updater receive the a message with the NEW_PATIENT event from the object "myPatientDB" the object myImage is added or swapped in the composite.
     * when the updater receive the a message with the CLEAR_PATIENT event from the object "myPatientDB" the object myImage is removed from the composite
     *
     * The actionType can be : ADD, ADD_OR_SWAP, SWAP, REMOVE, REMOVE_IF_PRESENT and DO_NOTHING
     */
    void configureManagedEvents(::fwRuntime::ConfigurationElement::sptr configuration);

    typedef std::vector < ::boost::tuple< std::string, std::string, std::string, ActionType > > ManagedEvents;

    /// List of the managed event
    ManagedEvents m_managedEvents;
};

}

#endif // _CTRLSELECTION_IUPDATERSRV_HPP_

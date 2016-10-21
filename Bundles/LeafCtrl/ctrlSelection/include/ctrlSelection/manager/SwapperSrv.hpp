/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_MANAGER_SWAPPERSRV_HPP__
#define __CTRLSELECTION_MANAGER_SWAPPERSRV_HPP__

#include "ctrlSelection/IManagerSrv.hpp"
#include "ctrlSelection/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>


namespace ctrlSelection
{
namespace manager
{

/**
 * @brief  This services is a manager which starts, stops or swaps services on object contained in a composite when
 * it receive specific message (mainly sent by updater).
 */
class CTRLSELECTION_CLASS_API SwapperSrv : public ::ctrlSelection::IManagerSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SwapperSrv)(::ctrlSelection::IManagerSrv) );

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SwapperSrv() throw();

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SwapperSrv() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_ADD_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_CHANGE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_REMOVE_OBJECTS_SLOT
     */
    CTRLSELECTION_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /// Implements starting method derived from IService. Starts the managed services if their objects are in the composite.
    /// If the mode is "dummy", starts the the managed services on dummy objects if their objects are not in the composite.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Stops and erases all the managed services.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Implements configuring method derived from IService. .
     *
     * Sample of declaration configuration for a simple swapper service
     *
     * @code{.xml}
        <service uid="myManager" impl="::ctrlSelection::manager::SwapperSrv" type="::ctrlSelection::IManagerSrv" autoConnect="yes" >
            <mode type="dummy" />
            <config>
                <object id="myImage" type="::fwData::Image" >
                    <service uid="myMedicalImageConverter" impl="::ctrlSelection::MedicalImageSrv" type="::fwServices::IController"  autoConnect="no" />
                    <service uid="myServices" impl="..." type="..." autoConnect="yes" />

                    <connect>
                        <signal>key</signal><!-- Signal of object "myImage" -->
                        <slot>uid/key</slot>
                    </connect>

                </object>
                <object id="myAcquisition" type="::fwData::Acquisition" >
                    <service uid="myServices2" impl="..." type="..." autoConnect="yes" worker="myThread" />

                    <proxy channel="...">
                        <signal>...</signal><!-- Signal of object "myAcquisition" -->
                        <signal>.../...</signal>
                        <slot>.../...</slot>
                        <slot>.../...</slot>
                    </proxy>
                </object>
            </config>
        </service>
       @endcode
     * With:
     * @li \b mode : must be "stop", "dummy" or "startAndUpdate".
     *     - The mode "stop", used by default, starts the services when their attached object is added in the compsite
     *       and stop and unregister the services when the object is deleted.
     *     - The mode "dummy" doesn't stop the services when its attached object is deleted but swap it on a dummy
     *       object.
     *     - The mode "startAndUpdate" start and update the services when its attached object is added in the composite.
     * @li the objects and services tags are defined as same as the configuration of objects and services.
     * @li \b autoConnect: optional (default value = false), if true allows to listen signals from the associated object.
     * @li \b worker: optional, allows to manage the service in another thread.
     * @li \b connect : not mandatory, connects signal to slot
     *   - \b signal : mandatory, must be signal holder UID, followed by '/', followed by signal name. To use the
     *        object signal, you don't have to write object uid, only the signal name.
     *   - \b slot : mandatory, must be slot holder UID, followed by '/', followed by slot name
     * @li \b proxy : not mandatory, connects the signals/slots in a proxy
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream );

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string ObjectIdType;


    class SubService
    {
    public:

        SubService() : m_hasAutoConnection(false)
        {
        }

        ~SubService()
        {
        }

        SPTR (::fwServices::IService) getService()
        {
            return m_service.lock();
        }

        ::fwData::Object::sptr m_dummy;
        ConfigurationType m_config;
        WPTR(::fwServices::IService) m_service;
        ::fwCom::helper::SigSlotConnection m_connections;
        bool m_hasAutoConnection;
    };

    typedef std::vector< SPTR(SubService) > SubServicesVecType;
    typedef std::map< ObjectIdType, SubServicesVecType > SubServicesMapType;

    /// Slot: add objects
    void addObjects(::fwData::Composite::ContainerType objects);

    /// Slot: change objects
    void changeObjects(::fwData::Composite::ContainerType newObjects, ::fwData::Composite::ContainerType oldObjects);

    /// Slot: remove objects
    void removeObjects(::fwData::Composite::ContainerType objects);

    void initOnDummyObject( std::string objectId );
    void addObject( const std::string& objectId, ::fwData::Object::sptr object );
    void changeObject(const std::string& objectId, ::fwData::Object::sptr object);
    void removeObject( const std::string& objectId );

    ::fwServices::IService::sptr add( ::fwData::Object::sptr obj, ::fwRuntime::ConfigurationElement::sptr _elt );

private:

    std::string m_mode;
    bool m_dummyStopMode;
    ConfigurationType m_managerConfiguration;
    SubServicesMapType m_objectsSubServices;

};

} // manager
} // ctrlSelection

#endif // __CTRLSELECTION_MANAGER_SWAPPERSRV_HPP__

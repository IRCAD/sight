/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_MANAGER_SWAPPERSRV_HPP__
#define __CTRLSELECTION_MANAGER_SWAPPERSRV_HPP__

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IManagerSrv.hpp"

namespace ctrlSelection
{
namespace manager
{

/**
 * @class  SwapperSrv
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
     * @verbatim
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
       @endverbatim
     * With:
     * @li \b mode : must be "stop" or "dummy". The dummy mode doesn't stop the services when its attached object is
     * deleted but swap it on a dummy object.
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
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    /// Reacts on specifics event (ADDED_KEYS, REMOVED_KEYS and CHANGED_KEYS) and start, stop or swap the managed services
    /// on the objects defined in the message or dummy objects
    CTRLSELECTION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );


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
        ::fwServices::helper::SigSlotConnection::sptr m_connections;
        bool m_hasAutoConnection;
    };

    typedef std::vector< SPTR(SubService) > SubServicesVecType;
    typedef std::map< ObjectIdType, SubServicesVecType > SubServicesMapType;

    void initOnDummyObject( std::string objectId );
    void addObjects( ::fwData::Composite::sptr _composite );
    void addObject( const std::string &objectId, ::fwData::Object::sptr object );
    void swapObjects( ::fwData::Composite::sptr _composite );
    void swapObject(const std::string &objectId, ::fwData::Object::sptr object);
    void removeObjects( ::fwData::Composite::sptr _composite );
    void removeObject( const std::string &objectId );

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

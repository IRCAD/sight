/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_MANAGER_SWAPPER_HPP_
#define _CTRLSELECTION_MANAGER_SWAPPER_HPP_

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
 * @class  SwapperSrv.
 * @brief  This services is a manager which starts, stops or swaps services on object contained in a composite when
 * it receive specific message (mainly sent by updater).
 * @author IRCAD (Research and Development Team).

 * @date   2007-2009.
 */
class CTRLSELECTION_CLASS_API SwapperSrv : public ::ctrlSelection::IManagerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SwapperSrv)(::ctrlSelection::IManagerSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SwapperSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SwapperSrv() throw() ;

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
        <service uid="myManager" implementation="::ctrlSelection::manager::SwapperSrv" type="::ctrlSelection::IManagerSrv" autoComChannel="yes" >
            <mode type="dummy" />
            <config>
                <object id="myImage" type="::fwData::Image" >
                    <service uid="myMedicalImageConverter" impl="::ctrlSelection::MedicalImageSrv" type="::fwServices::IController"  autoConnect="no" />
                    <service uid="myServices" impl="..." type="..." autoConnect="yes" />

                    <connect>
                        <signal>key</signal>
                        <slot>uid/key</slot>
                    </connect>

                </object>
                <object id="myAcquisition" type="::fwData::Acquisition" >
                    <service uid="myServices2" impl="..." type="..." autoConnect="yes" />

                    <proxy channel="...">
                        <signal>...</signal>
                        <signal>.../...</signal>
                        <slot>.../...</slot>
                        <slot>.../...</slot>
                    </proxy>
                </object>
            </config>
        </service>
    @endverbatim
    * With:
    * @li mode : must be "stop" or "dummy". The dummy mode doesn't stop the services when its attached object is deleted but swap it on a dummy object.
    * @li the objects and services tags are defined as same as the configuration of objects and services.
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
        {}

        ~SubService()
        {}

        SPTR (::fwServices::IService) getService()
                    { return m_service.lock(); }

        ::fwData::Object::sptr m_dummy;
        ConfigurationType m_config;
        WPTR(::fwServices::IService) m_service;
        ::fwServices::helper::SigSlotConnection::sptr m_connections;
        bool m_hasAutoConnection;
    };

    typedef std::vector< SPTR(SubService) > SubServicesVecType;
    typedef std::map< ObjectIdType, SubServicesVecType > SubServicesMapType ;

    /// Used to register proxy connection in order to properly disconnect it.
    struct ProxyConnections
    {
        typedef std::string UIDType;
        typedef std::string KeyType;
        typedef std::pair<UIDType, KeyType> ProxyEltType;
        typedef std::vector<ProxyEltType> ProxyEltVectType;

        std::string m_channel;
        ProxyEltVectType m_slots;
        ProxyEltVectType m_signals;

        ProxyConnections(const std::string& channel) : m_channel(channel)
        {}

        ~ProxyConnections()
        {}

        void addSlotConnection(UIDType uid, KeyType key)
        {
            m_slots.push_back(std::make_pair(uid, key));
        }
        void addSignalConnection(UIDType uid, KeyType key)
        {
            m_signals.push_back(std::make_pair(uid, key));
        }
    };
    typedef std::vector<ProxyConnections> ProxyConnectionsVectType;
    typedef std::map< ObjectIdType, ProxyConnectionsVectType > ProxyConnectionsMapType;
    /// Proxy connection information map : used to properly disconnect proxies
    ProxyConnectionsMapType m_proxyCtns;


    void initOnDummyObject( std::string objectId );
    void addObjects( ::fwData::Composite::sptr _composite );
    void addObject( const std::string objectId, ::fwData::Object::sptr object );
    void swapObjects( ::fwData::Composite::sptr _composite );
    void swapObject(const std::string objectId, ::fwData::Object::sptr object);
    void removeObjects( ::fwData::Composite::sptr _composite );
    void removeObject( const std::string objectId );

    /**
     * @brief Manage all connections define in config associated to object.
     * Call manageConnection()
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for this object
     */
    void manageConnections(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /**
     * @brief Manage a connection define in config associated to object.
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for a <connect> tag associated this object
     */
    void manageConnection(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /// Disconnect all registred connection for objectId.
    void removeConnections(const std::string &objectId);

    /**
     * @brief Manage all proxies connections define in config associated to object
     * Call manageProxy()
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for this object
     */
    void manageProxies(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /**
     * @brief Manage proxy connections define in config associated to object
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for a <proxy> tag associated this object
     */
    void manageProxy(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /// Disconnect all proxies associated to objectId;
    void disconnectProxies(const std::string &objectId);

    ::fwServices::IService::sptr add( ::fwData::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt );

    typedef std::map< ObjectIdType, ::fwServices::helper::SigSlotConnection::sptr > ObjectConnectionsMapType;

    /// Register connection associated to an object. Connections are connected/disconnected when the object is added/removed.
    ObjectConnectionsMapType m_objectConnections;

private:

    std::string m_mode;
    bool m_dummyStopMode;
    ConfigurationType m_managerConfiguration;
    SubServicesMapType m_objectsSubServices;

};

} // manager
} // ctrlSelection

#endif // _CTRLSELECTION_MANAGER_SWAPPER_HPP_

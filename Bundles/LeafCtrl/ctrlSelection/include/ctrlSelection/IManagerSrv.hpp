/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IMANAGERSRV_HPP_
#define _CTRLSELECTION_IMANAGERSRV_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class   IManagerSrv
 * @brief   Base class for manager.
 * A manager starts, stops or swaps services on object contained in a composite when it receives specific message.
 * 
 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API IManagerSrv : public ::fwServices::IService
{

public:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string ObjectIdType;

    fwCoreServiceClassDefinitionsMacro ( (IManagerSrv)(::fwServices::IService) ) ;

    ///@brief IManagerSrv constructor. Do nothing.
    CTRLSELECTION_API IManagerSrv();

    ///@brief IManagerSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IManagerSrv();

protected:

    /**
     * @brief Swaps the service from associated object to another object. Stops and starts this service.
     */
    CTRLSELECTION_API virtual void swapping() throw ( ::fwTools::Failed );

    typedef std::map< ObjectIdType, ::fwServices::helper::SigSlotConnection::sptr > ObjectConnectionsMapType;



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

    /**
     * @brief Manages all connections define in config associated to object.
     * Call manageConnection()
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for this object
     */
    void manageConnections(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /**
     * @brief Manages a connection define in config associated to object.
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for a \<connect\> tag associated this object
     */
    void manageConnection(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /// Disconnects all registred connection for objectId.
    void removeConnections(const std::string &objectId);

    /**
     * @brief Manages all proxies connections define in config associated to object
     * Call manageProxy()
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for this object
     */
    void manageProxies(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /**
     * @brief Manages proxy connections define in config associated to object
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for a \<proxy\> tag associated this object
     */
    void manageProxy(const std::string &objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /// Disconnects all proxies associated to objectId;
    void disconnectProxies(const std::string &objectId);

    /// Registers connection associated to an object. Connections are connected/disconnected when the object is added/removed.
    ObjectConnectionsMapType m_objectConnections;

    /// Proxy connection information map : used to properly disconnect proxies
    ProxyConnectionsMapType m_proxyCtns;


};

}

#endif // _CTRLSELECTION_IMANAGERSRV_HPP_

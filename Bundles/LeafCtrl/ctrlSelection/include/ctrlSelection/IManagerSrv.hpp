/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_IMANAGERSRV_HPP__
#define __CTRLSELECTION_IMANAGERSRV_HPP__

#include "ctrlSelection/config.hpp"

#include <fwServices/IService.hpp>
#include <fwServices/helper/Config.hpp>

namespace ctrlSelection
{

/**
 * @brief   Base class for manager.
 * A manager starts, stops or swaps services on object contained in a composite when it receives specific message.
 */
class CTRLSELECTION_CLASS_API IManagerSrv : public ::fwServices::IService
{

public:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string ObjectIdType;

    fwCoreServiceClassDefinitionsMacro ( (IManagerSrv)(::fwServices::IService) );

    ///@brief IManagerSrv constructor. Do nothing.
    CTRLSELECTION_API IManagerSrv();

    ///@brief IManagerSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IManagerSrv();

protected:

    /**
     * @brief Swaps the service from associated object to another object. Stops and starts this service.
     */
    CTRLSELECTION_API virtual void swapping();

    typedef std::map< ObjectIdType, ::fwCom::helper::SigSlotConnection > ObjectConnectionsMapType;

    /**
     * @brief Manages all connections define in config associated to object.
     * Call manageConnection()
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for this object
     */
    void manageConnections(const std::string& objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /**
     * @brief Manages a connection define in config associated to object.
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for a \<connect\> tag associated this object
     */
    void manageConnection(const std::string& objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /// Disconnects all registred connection for objectId.
    void removeConnections(const std::string& objectId);

    /**
     * @brief Manages all proxies connections define in config associated to object
     * Call manageProxy()
     *
     * @param objectId Id of the object
     * @param object Object associated with the id
     * @param config configuration for this object
     */
    void manageProxies(const std::string& objectId, ::fwData::Object::sptr object, ConfigurationType config);

    /// Disconnects all proxies associated to objectId;
    void disconnectProxies(const std::string& objectId);

    /// Registers connection associated to an object. Connections are connected/disconnected when the object is added/removed.
    ObjectConnectionsMapType m_objectConnections;

    /// Proxy connection information map : used to properly disconnect proxies
    ::fwServices::helper::Config::ProxyConnectionsMapType m_proxyCtns;


};

}

#endif // __CTRLSELECTION_IMANAGERSRV_HPP__

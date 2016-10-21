/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_APPCONFIGMANAGER_HPP__
#define __FWSERVICES_APPCONFIGMANAGER_HPP__

#include "fwServices/IAppConfigManager.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/config.hpp"
#include "fwServices/registry/AppConfig.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <boost/tuple/tuple.hpp>

#include <string>
#include <vector>

namespace fwData
{
class Composite;
}

namespace fwServices
{
/**
 * @brief   This class provides an API to manage config template.
 */
class FWSERVICES_CLASS_API AppConfigManager : public ::fwServices::IAppConfigManager
{
protected:
    typedef std::vector< ::fwServices::IService::wptr > ServiceContainer;
    typedef ::boost::tuple< std::string, bool > ConfigAttribute;

public:

    fwCoreClassDefinitionsWithFactoryMacro((AppConfigManager)(::fwServices::IAppConfigManager),
                                           (()),
                                           std::make_shared< AppConfigManager >);

    /// Constructor. Do nothing.
    FWSERVICES_API AppConfigManager();

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~AppConfigManager();

    /**
     * @name Overrides
     * @{
     */
    FWSERVICES_API virtual void setConfig(const std::string& configId,
                                          const FieldAdaptorType& replaceFields = FieldAdaptorType());
    FWSERVICES_API virtual void setConfig(const std::string& configId, const ::fwData::Composite::csptr& replaceFields);
    FWSERVICES_API virtual ::fwData::Object::sptr getConfigRoot() const;
    FWSERVICES_API virtual void launch();
    FWSERVICES_API virtual void stopAndDestroy();
    FWSERVICES_API virtual void create();
    FWSERVICES_API virtual void start();
    FWSERVICES_API virtual void update();
    FWSERVICES_API virtual void stop();
    FWSERVICES_API virtual void destroy();
    ///@}

    /**
     * @brief Starts the bundle associated to the config
     * @note  Does nothing if the bundle is already started or if the config id is not specified (ie. if config is set
     *        with setConfig(::fwRuntime::ConfigurationElement::csptr cfgElem) ).
     */
    FWSERVICES_API virtual void startBundle();

protected:
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
        {
        }

        ~ProxyConnections()
        {
        }

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

    /// Root of the config
    ::fwData::Object::sptr m_configuredObject;

    ::fwServices::IXMLParser::sptr m_objectParser;
    ProxyConnectionsVectType m_vectProxyCtns;

    std::string m_configId;

    ServiceContainer m_createdSrv;
    ServiceContainer m_startedSrv;
    ::fwCom::helper::SigSlotConnection m_connections;

    FWSERVICES_API ::fwData::Object::sptr getNewObject(
        ConfigAttribute type,
        const std::string& uid,
        ConfigAttribute id = ConfigAttribute("", false));

    FWSERVICES_API ::fwData::Object::sptr getNewObject(
        ConfigAttribute type,
        ConfigAttribute uid = ConfigAttribute("", false),
        ConfigAttribute id = ConfigAttribute("", false));

    FWSERVICES_API ::fwData::Object::sptr getRefObject(
        ConfigAttribute type,
        const std::string& uid,
        ConfigAttribute id = ConfigAttribute("", false));

    FWSERVICES_API ::fwServices::IService::sptr getNewService(
        ConfigAttribute type,
        ConfigAttribute uid,
        ConfigAttribute implType);

    FWSERVICES_API void autoSigSlotConnection(
        ::fwData::Object::sptr obj,
        ::fwServices::IService::sptr srv,
        ConfigAttribute priority);

    FWSERVICES_API void startConnections();
    FWSERVICES_API void stopConnections();

    FWSERVICES_API void stopStartedServices();
    FWSERVICES_API void destroyCreatedServices();

    FWSERVICES_API void processStartItems();
    FWSERVICES_API void processUpdateItems();

    FWSERVICES_API virtual ::fwData::Object::sptr createObject();
    FWSERVICES_API virtual void createServices();
    FWSERVICES_API virtual void createServices(::fwRuntime::ConfigurationElement::csptr cfgElem);

    FWSERVICES_API virtual void bindService(::fwRuntime::ConfigurationElement::csptr srvElem);
    FWSERVICES_API virtual void createConnections();
    FWSERVICES_API virtual void createProxy(::fwRuntime::ConfigurationElement::csptr config);
    FWSERVICES_API virtual void destroyProxies();
};

} // namespace fwServices

#endif // __FWSERVICES_APPCONFIGMANAGER_HPP__

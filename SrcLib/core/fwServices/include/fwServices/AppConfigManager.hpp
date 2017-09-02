/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_APPCONFIGMANAGER_HPP__
#define __FWSERVICES_APPCONFIGMANAGER_HPP__

#include "fwServices/config.hpp"
#include "fwServices/helper/Config.hpp"
#include "fwServices/helper/ProxyConnections.hpp"
#include "fwServices/IAppConfigManager.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/registry/AppConfig.hpp"

#include <fwCom/HasSlots.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>
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
class FWSERVICES_CLASS_API AppConfigManager :  public ::fwServices::IAppConfigManager,
                                               public ::fwCom::HasSlots
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((AppConfigManager)(::fwServices::IAppConfigManager),
                                           (()),
                                           std::make_shared< AppConfigManager >);
    fwCoreAllowSharedFromThis();

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

    /// Set it to true if we are testing the class
    FWSERVICES_API void setIsUnitTest(bool isUnitTest);

private:

    typedef ::std::pair< std::string, bool > ConfigAttribute;
    typedef ::fwServices::helper::ProxyConnections ProxyConnections;
    typedef ::fwServices::IService::Config ServiceConfig;

    ::fwData::Object::sptr findObject(const std::string& uid, const std::string& errMsgTail) const;

    ::fwData::Object::sptr getNewObject(ConfigAttribute type, const std::string& uid) const;

    ::fwData::Object::sptr getNewObject(ConfigAttribute type,
                                        ConfigAttribute uid = ConfigAttribute("", false)) const;

    ::fwData::Object::sptr getRefObject(ConfigAttribute type, const std::string& uid) const;

    ::fwServices::IService::sptr getNewService(const std::string& uid, const std::string& implType) const;

    void stopStartedServices();
    void destroyCreatedServices();

    void processStartItems();
    void processUpdateItems();

    /// Parse objects section and create objects
    void createObjects(::fwRuntime::ConfigurationElement::csptr cfgElem);

    /// Parse services and create all the services that can be instantiated
    void createServices(::fwRuntime::ConfigurationElement::csptr cfgElem);

    /// Create a single service from its configuration
    ::fwServices::IService::sptr createService(const ServiceConfig& srvConfig);

    /// Parse connection sections and creates them
    void createConnections();

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    std::string msgHead() const;

    /// Slot: add objects
    void addObjects(::fwData::Object::sptr obj, const std::string& id);

    /// Slot: remove objects
    void removeObjects(::fwData::Object::sptr obj, const std::string& id);

    /// Connect signal and slots
    void connectProxy(const std::string& channel, const ProxyConnections& connectCfg);

    void destroyProxy(const std::string& channel, const ProxyConnections& proxyCfg, const std::string& key = "",
                      fwData::Object::csptr hintObj = nullptr);
    void destroyProxies();

    /// Given a list of UIDs or WIDs, get a friendly printable message
    static std::string getUIDListAsString(const std::vector<std::string>& uidList);

    typedef std::pair< ::fwData::Object::sptr, ::fwServices::IXMLParser::sptr> CreatedObjectType;
    /// Map containing the object and its XML parser
    std::unordered_map<std::string, CreatedObjectType> m_createdObjects;

    struct DeferredObjectType
    {
        std::vector< ServiceConfig > m_servicesCfg;
        std::unordered_map< std::string, ProxyConnections > m_proxyCnt;
        /// Copy of the object pointer necessary to access signals/slots when destroying proxy
        ::fwData::Object::sptr m_object;
    };

    /// Map indexed by the object uid, containing all the service configurations that depend on this object
    std::unordered_map<std::string, DeferredObjectType > m_deferredObjects;

    /// All the identifiers of the deferred services
    std::unordered_set<std::string> m_deferredServices;

    /// All proxies of created objects, ordered by channel name
    std::unordered_map<std::string, ProxyConnections> m_createdObjectsProxies;

    struct ServiceProxyType
    {
        std::unordered_map< std::string, ProxyConnections > m_proxyCnt;
    };
    std::unordered_map< std::string, ServiceProxyType > m_servicesProxies;

    /// Identifier of this configuration
    std::string m_configId;

    typedef std::vector< ::fwServices::IService::wptr > ServiceContainer;

    /// List of services created in this configuration
    ServiceContainer m_createdSrv;

    /// List of services started in this configuration
    ServiceContainer m_startedSrv;

    /// Start ordered list of deferred services
    std::vector<std::string> m_deferredStartSrv;

    /// Update ordered list of deferred services
    std::vector<std::string> m_deferredUpdateSrv;

    /// While we need to maintain old and new services behavior, we need a dummy data for new services
    /// that don't work on any data.
    /// TODO: Remove with V1
    ::fwData::Composite::sptr m_tmpRootObject;

    /// Counter used to generate a unique proxy name
    unsigned int m_proxyID;

    ::fwCom::Connection m_addObjectConnection;
    ::fwCom::Connection m_removeObjectConnection;

    /// Hack to know if we are doing a unit test. We skip some code in this case to be able to launch a configuration
    bool m_isUnitTest;
};

// ------------------------------------------------------------------------

inline std::string AppConfigManager::msgHead() const
{
    return "[" + m_configId + "] ";
}

} // namespace fwServices

#endif // __FWSERVICES_APPCONFIGMANAGER_HPP__


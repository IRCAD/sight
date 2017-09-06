/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_SERVICEFACTORY_HPP__
#define __FWSERVICES_REGISTRY_SERVICEFACTORY_HPP__

#include "fwServices/config.hpp"

#include <fwCore/mt/types.hpp>

#include <fwRuntime/Bundle.hpp>

#include <fwTools/Object.hpp>

#include <boost/unordered_map.hpp>

#include <unordered_map>

namespace fwServices
{

class IService;

namespace registry
{

/**
 * The ServiceFactory is a registrar of all the services. It allows to retrieve the informations necessary to
 * instantiate the services.
 */
class FWSERVICES_CLASS_API ServiceFactory : public ::fwCore::BaseObject
{

public:

    typedef std::string KeyType;
    typedef std::vector<KeyType> KeyVectorType;
    typedef std::pair<std::string, std::string> StringPair;

    /// We keep boost here because std implementation does not support a pair of std::string as key
    typedef ::boost::unordered_map< StringPair, bool > SupportMapType;
    typedef std::function< SPTR(::fwServices::IService)() > FactoryType;

    fwCoreClassDefinitionsWithFactoryMacro( (ServiceFactory)(::fwCore::BaseObject), (()), new ServiceFactory);

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static ServiceFactory::sptr getDefault();

    /// Parse bundle information to retrieve service declaration
    FWSERVICES_API void parseBundleInformation( );

    FWSERVICES_API void addServiceFactory( FactoryType _factory,
                                           const std::string& simpl,
                                           const std::string& stype);

    FWSERVICES_API void addObjectFactory( const std::string& simpl,
                                          const std::string& oimpl);

    /// Create a service from a factory type
    FWSERVICES_API SPTR(IService) create( const std::string& _srvImpl ) const;

    /// Create a service from a factory type and verify that it is an inherited type of _srvType
    FWSERVICES_API SPTR(IService) create( const std::string& _srvType, const std::string& _srvImpl ) const;

    FWSERVICES_API void clearFactory();

    /// return a vector of service implementation
    FWSERVICES_API std::vector< std::string > getImplementationIdFromObjectAndType(const std::string& object,
                                                                                   const std::string& type) const;

    /// return the default service implementation for an object
    FWSERVICES_API std::string getDefaultImplementationIdFromObjectAndType( const std::string& object,
                                                                            const std::string& type ) const;

    /// return the associated object implementation.
    FWSERVICES_API std::string getObjectImplementation(const std::string& srvImpl) const;

    /// return the service description.
    FWSERVICES_API std::string getServiceDescription(const std::string& srvImpl) const;

    /// Check if the service with given object and implementation is valid
    FWSERVICES_API bool checkServiceValidity(const std::string& object, const std::string& srvImpl) const;

    /**
     * @brief Check whether an object (object) supports service of type srvType
     * @return true if service type supported
     */
    FWSERVICES_API bool support(const std::string& object, const std::string& srvType);

    /**
     * @brief Check whether an object (object) supports service of type srvType and implementation srvImpl
     * @return true if service type supported
     */
    FWSERVICES_API bool support(const std::string& object, const std::string& srvType,
                                const std::string& srvImpl) const;

    /**
     * @brief returns the registered factory keys.
     */
    FWSERVICES_API virtual KeyVectorType getFactoryKeys() const;

private:

    struct ServiceInfo
    {
        std::string serviceType;
        std::string objectImpl;
        std::string desc;
        std::shared_ptr< ::fwRuntime::Bundle > bundle;
        FactoryType factory;
    };
    typedef std::unordered_map< KeyType, ServiceInfo > SrvRegContainer;

    /**
     * @brief print services informations
     * @warning not thread-safe
     */
    void printInfoMap( const SrvRegContainer& src ) const;

    /**
     * @brief Trace services not declared in plugin.xml
     * @warning not thread-safe
     */
    void checkServicesNotDeclaredInPluginXml() const;

    /// Container of service information
    SrvRegContainer m_srvImplTosrvInfo;
    SupportMapType m_supportMap;

    mutable ::fwCore::mt::ReadWriteMutex m_srvImplTosrvInfoMutex;
    mutable ::fwCore::mt::ReadWriteMutex m_supportMapMutex;
};

} // namespace registry
} // namespace fwServices

#endif // __FWSERVICES_REGISTRY_SERVICEFACTORY_HPP__


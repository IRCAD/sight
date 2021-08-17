/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "service/config.hpp"

#include <core/mt/types.hpp>
#include <core/runtime/Module.hpp>
#include <core/tools/Object.hpp>

#include <boost/unordered_map.hpp>

#include <unordered_map>

namespace sight::service
{

class IService;

namespace extension
{

/**
 * The Factory is a registry of all the services. It allows to retrieve the informations necessary to
 * instantiate the services.
 */
class SERVICE_CLASS_API Factory : public core::BaseObject
{
public:

    typedef std::string KeyType;
    typedef std::vector<KeyType> KeyVectorType;
    typedef std::pair<std::string, std::string> StringPair;

    /// We keep boost here because std implementation does not support a pair of std::string as key
    typedef ::boost::unordered_map<StringPair, bool> SupportMapType;
    typedef std::function<SPTR(service::IService)()> FactoryType;

    SIGHT_DECLARE_CLASS(Factory, core::BaseObject, new Factory);

    /// Return the unique Instance, create it if required at first access
    SERVICE_API static Factory::sptr getDefault();

    /// Parse module information to retrieve service declaration
    SERVICE_API void parseBundleInformation();

    SERVICE_API void addServiceFactory(
        FactoryType _factory,
        const std::string& simpl,
        const std::string& stype
    );

    SERVICE_API void addObjectFactory(
        const std::string& simpl,
        const std::string& oimpl
    );

    /// Create a service from a factory type
    SERVICE_API SPTR(IService) create(const std::string& _srvImpl) const;

    SERVICE_API void clearFactory();

    /// return a vector of service implementation
    SERVICE_API std::vector<std::string> getImplementationIdFromObjectAndType(
        const std::string& _object,
        const std::string& _type
    ) const;

    /// return the default service implementation for an object
    SERVICE_API std::string getDefaultImplementationIdFromObjectAndType(
        const std::string& _object,
        const std::string& _type
    ) const;

    /// return the objects registered for a given service.
    SERVICE_API const std::vector<std::string>& getServiceObjects(const std::string& _srvImpl) const;

    /// return the service description.
    SERVICE_API std::string getServiceDescription(const std::string& _srvImpl) const;

    /// return the service capabilities.
    SERVICE_API std::string getServiceTags(const std::string& _srvImpl) const;

    /// Check if the service with given object and implementation is valid
    SERVICE_API bool checkServiceValidity(const std::string& _object, const std::string& _srvImpl) const;

    /**
     * @brief Check whether an object (object) supports service of type srvType
     * @return true if service type supported
     */
    SERVICE_API bool support(const std::string& _object, const std::string& _srvType);

    /**
     * @brief Check whether an object (object) supports service of type srvType and implementation srvImpl
     * @return true if service type supported
     */
    SERVICE_API bool support(
        const std::string& _object,
        const std::string& _srvType,
        const std::string& _srvImpl
    ) const;

    /**
     * @brief returns the registered factory keys.
     */
    SERVICE_API virtual KeyVectorType getFactoryKeys() const;

private:

    struct ServiceInfo
    {
        std::string serviceType;
        std::vector<std::string> objectImpl;
        std::string desc;
        std::string tags;
        std::shared_ptr<core::runtime::Module> module;
        FactoryType factory;
        bool objectsSetFromModule {false}; // True if the objects implementation are set from the module information
    };
    typedef std::unordered_map<KeyType, ServiceInfo> SrvRegContainer;

    /**
     * @brief print services informations
     * @warning not thread-safe
     */
    void printInfoMap(const SrvRegContainer& src) const;

    /**
     * @brief Trace services not declared in plugin.xml
     * @warning not thread-safe
     */
    void checkServicesNotDeclaredInPluginXml() const;

    /// Container of service information
    SrvRegContainer m_srvImplTosrvInfo;
    SupportMapType m_supportMap;

    mutable core::mt::ReadWriteMutex m_srvImplTosrvInfoMutex;
    mutable core::mt::ReadWriteMutex m_supportMapMutex;
};

} // namespace extension

} // namespace sight::service

/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "service/extension/Factory.hpp"

#include "service/IService.hpp"

#include <core/LazyInstantiator.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/runtime/Runtime.hpp>

#include <data/Exception.hpp>

#include <functional>
#include <vector>

namespace sight::service::extension
{

//-----------------------------------------------------------------------------

Factory::sptr Factory::getDefault()
{
    return core::LazyInstantiator<Factory>::getInstance();
}

//-----------------------------------------------------------------------------

void Factory::parseBundleInformation()
{
    SrvRegContainer moduleInfoMap;

    using ConfigurationType = core::runtime::ConfigurationElement::sptr;
    using ExtensionType     = std::shared_ptr<core::runtime::Extension>;

    std::vector<ExtensionType> extElements;
    extElements = core::runtime::getAllExtensionsForPoint("sight::service::extension::Factory");
    for(const ExtensionType& extElt : extElements)
    {
        std::vector<ConfigurationType> cfgEltVec = extElt->getElements();
        std::string type;
        std::string service;
        std::vector<std::string> objects;
        std::string desc;
        std::string tags;

        for(const ConfigurationType& cfgElt : cfgEltVec)
        {
            std::string elt = cfgElt->getName();
            if(elt == "type")
            {
                type = core::runtime::filterID(cfgElt->getValue());
            }
            else if(elt == "service")
            {
                service = core::runtime::filterID(cfgElt->getValue());
            }
            else if(elt == "object")
            {
                objects.push_back(core::runtime::filterID(cfgElt->getValue()));
            }
            else if(elt == "desc")
            {
                desc = cfgElt->getValue();
            }
            else if(elt == "tags")
            {
                tags = cfgElt->getValue();
            }
            else
            {
                SIGHT_FATAL("Unknown element !");
            }
        }

        SIGHT_ASSERT("Missing type element.", !type.empty());
        SIGHT_ASSERT("Missing service element.", !service.empty());

        ServiceInfo info;
        info.serviceType          = type;
        info.objectsSetFromModule = !objects.empty();
        info.objectImpl           = std::move(objects);
        info.desc                 = desc;
        info.tags                 = tags;
        info.module               = cfgEltVec[0]->getModule();
        SIGHT_ASSERT("Module not found.", info.module);

        moduleInfoMap.emplace(std::make_pair(service, info));
    }

    sight::service::extension::Factory::printInfoMap(moduleInfoMap);

    core::mt::ReadToWriteLock lock(m_srvImplToSrvInfoMutex);
    // Merge data info
    for(const SrvRegContainer::value_type& module : moduleInfoMap)
    {
        auto iter = m_srvImplToSrvInfo.find(module.first);

        if(iter != m_srvImplToSrvInfo.end())
        {
            SIGHT_DEBUG(
                "We already have informations about this service  (from register macro) ( " << module.first
                << " )."
            );

            ServiceInfo& info             = iter->second;
            const ServiceInfo& infoModule = module.second;

            SIGHT_ASSERT("Try to add a module, but this module already exists.", !info.module);
            SIGHT_ASSERT(
                "Try to add a module, but this srv is already registered and doesn't have the same srv type.",
                infoModule.serviceType == info.serviceType
            );
            SIGHT_ASSERT(
                "Try to add a module, but the service '"
                << module.first << "' is already registered and does not have the same objects.",
                infoModule.objectImpl.empty() || infoModule.objectImpl == info.objectImpl
            );

            info.module               = infoModule.module;
            info.desc                 = infoModule.desc;
            info.objectsSetFromModule = infoModule.objectsSetFromModule;
        }
        else
        {
            core::mt::UpgradeToWriteLock upgrade(lock);
            m_srvImplToSrvInfo.emplace(std::make_pair(module.first, module.second));
        }
    }

    sight::service::extension::Factory::printInfoMap(m_srvImplToSrvInfo);
    this->checkServicesNotDeclaredInPluginXml();
}

//-----------------------------------------------------------------------------

IService::sptr Factory::create(const std::string& _srvImpl) const
{
    const std::string srvImpl = core::runtime::filterID(_srvImpl);
    IService::sptr service;

    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);

    SIGHT_ASSERT(
        "The service called '" << srvImpl << "' does not exist in the Factory ",
        iter != m_srvImplToSrvInfo.end()
    );

    const ServiceInfo& info = iter->second;

    SIGHT_DEBUG("SR creates a new service ( classname = " << srvImpl << " )");

    if(info.factory)
    {
        service = info.factory();
    }
    else
    {
        SIGHT_ASSERT(
            "A module must declare the factory named"
            << srvImpl
            << ", the service declaration might be missing (or misspelled) in a module plugin.",
            info.module
        );
        SIGHT_ASSERT(
            "The module '" + info.module->getIdentifier() + "' is already loaded and the factory '"
            + srvImpl + "' is still missing. The service declaration might be missing (or misspelled)"
                        "in a .cpp file.",
            !info.module->isStarted()
        );

        lock.unlock(); // module->start() may trigger calls to addFactory
        info.module->start();
        lock.lock();

        SIGHT_THROW_EXCEPTION_IF(
            data::Exception(
                "After loading the module " + info.module->getIdentifier() + " , factory " + srvImpl
                + " is still missing. The service declaration might be missing (or misspelled) "
                  "in a cpp file."
            ),
            !info.factory
        );

        service = info.factory();
    }

    // Setup worker here, this is a better place than the constructor
    // because here, the service slots are also set up
    // This allows to setup
    service->setWorker(core::thread::getDefaultWorker());

    return service;
}

//------------------------------------------------------------------------------

void Factory::addServiceFactory(
    FactoryType _factory,
    const std::string& _srvImpl,
    const std::string& _srvType
)
{
    const std::string srvImpl = core::runtime::filterID(_srvImpl);
    const std::string srvType = core::runtime::filterID(_srvType);

    SIGHT_DEBUG(std::string("New service registering : srvImpl =") + srvImpl + " srvType=" + srvType);

    core::mt::ReadToWriteLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);

    if(iter != m_srvImplToSrvInfo.end())
    {
        SIGHT_DEBUG("We already have informations about this service ( " + srvImpl + " ).");
        ServiceInfo& info = iter->second;
        SIGHT_ASSERT(
            "Try to add factory, but this srv ( " << srvImpl << " ) already has a registered factory.",
            !info.factory
        );
        SIGHT_ASSERT(
            "Try to add factory, but this srv ( "
            << srvImpl << " ) is already registered and doesn't have the same srv type. ( "
            << srvType << " != " << info.serviceType << " )",
            srvType == info.serviceType
        );

        core::mt::UpgradeToWriteLock upgrade(lock);
        info.factory = _factory;
    }
    else
    {
        SIGHT_DEBUG("Add new service factory in registry ( " + srvImpl + " ).");
        core::mt::UpgradeToWriteLock upgrade(lock);
        ServiceInfo info;
        info.serviceType = srvType;
        info.factory     = _factory;
        m_srvImplToSrvInfo.emplace(std::make_pair(srvImpl, info));
    }
}

//-----------------------------------------------------------------------------

void Factory::addObjectFactory(const std::string& _srvImpl, const std::string& _oimpl)
{
    const std::string srvImpl = core::runtime::filterID(_srvImpl);
    const std::string oImpl   = core::runtime::filterID(_oimpl);

    SIGHT_DEBUG(std::string("New object oImpl=") + oImpl + "registering to service: srvImpl =" + srvImpl);
    SIGHT_ASSERT("Empty oImpl", !oImpl.empty());

    core::mt::ReadToWriteLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);

    SIGHT_ASSERT(
        "Try to associate an object to a service factory, but this srv is not yet registered.",
        iter != m_srvImplToSrvInfo.end()
    );

    if(iter != m_srvImplToSrvInfo.end())
    {
        ServiceInfo& info = iter->second;

        // Either the module does not contain objects informations or this service does not belong to a module
        if(info.objectsSetFromModule)
        {
#ifdef _DEBUG
            const auto itFind = std::find(info.objectImpl.begin(), info.objectImpl.end(), oImpl);
#endif
            SIGHT_ASSERT(
                "Try to add factory, but the service '" + srvImpl + "' is already registered and does not have the "
                                                                    "same objects.",
                info.objectImpl.empty() || itFind != info.objectImpl.end()
            );
        }
        else
        {
            core::mt::UpgradeToWriteLock upgrade(lock);
            info.objectImpl.push_back(oImpl);
        }
    }
}

//-----------------------------------------------------------------------------

void Factory::printInfoMap(const SrvRegContainer& src)
{
    // not thread-safe

    //Print information
    for(const SrvRegContainer::value_type& srvReg : src)
    {
        SIGHT_DEBUG(" Service name = " << srvReg.first);
        SIGHT_DEBUG("  - type   = " << srvReg.second.serviceType);

#if SIGHT_DEBUG_ENABLED
        std::size_t objNum = 0;
        for(const auto& objImpl : srvReg.second.objectImpl)
        {
            SIGHT_DEBUG("  - object " << objNum++ << " = " << objImpl)
        }
#endif

        SIGHT_DEBUG_IF("  - module = " << srvReg.second.module->getIdentifier(), srvReg.second.module);
        SIGHT_DEBUG_IF("  - module = ( no module registered )", !srvReg.second.module);

        SIGHT_DEBUG_IF(
            "  - name after creation = "
            << srvReg.second.factory()->getClassname(),
            srvReg.second.factory
        );
        SIGHT_DEBUG_IF("  - name after creation = ( no factory registered )", !srvReg.second.factory);
    }
}

//-----------------------------------------------------------------------------

void Factory::checkServicesNotDeclaredInPluginXml() const
{
    // not thread-safe
    //Print information
    for(const SrvRegContainer::value_type& srvReg : m_srvImplToSrvInfo)
    {
        if(!srvReg.second.module)
        {
            SIGHT_WARN("Service " << srvReg.first << " is not declared/found in a plugin.xml.");
        }
    }
}

//-----------------------------------------------------------------------------

void Factory::clearFactory()
{
    core::mt::WriteLock lock(m_srvImplToSrvInfoMutex);
    m_srvImplToSrvInfo.clear();
}

//-----------------------------------------------------------------------------

std::vector<std::string> Factory::getImplementationIdFromObjectAndType(
    const std::string& _object,
    const std::string& _type
) const
{
    const std::string object = core::runtime::filterID(_object);
    const std::string type   = core::runtime::filterID(_type);

    std::vector<std::string> serviceImpl;

    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    for(const SrvRegContainer::value_type& srv : m_srvImplToSrvInfo)
    {
        const ServiceInfo& srvInfo = srv.second;
        for(const auto& oimpl : srvInfo.objectImpl)
        {
            if(srvInfo.serviceType == type
               && (oimpl == object || oimpl == "sight::data::Object"))
            {
                serviceImpl.push_back(srv.first);
                break;
            }
        }
    }

    return serviceImpl;
}

//-----------------------------------------------------------------------------

std::string Factory::getDefaultImplementationIdFromObjectAndType(
    const std::string& _object,
    const std::string& _type
) const
{
    const std::string object = core::runtime::filterID(_object);
    const std::string type   = core::runtime::filterID(_type);
    SIGHT_ASSERT("This case is not managed ", object != "sight::data::Object");

    std::string serviceImpl;
#ifdef _DEBUG
    bool genericImplIsFound = false;
#endif
    bool specificImplIsFound = false;

    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    for(const SrvRegContainer::value_type& srv : m_srvImplToSrvInfo)
    {
        const ServiceInfo& srvInfo = srv.second;
        if(srvInfo.serviceType == type)
        {
            for(const auto& oimpl : srvInfo.objectImpl)
            {
                if(oimpl == object)
                {
                    SIGHT_ASSERT(
                        "Method has already found a specific ("
                        << serviceImpl << " != " << srv.first
                        << ") service for the object " << oimpl << ".",
                        !specificImplIsFound
                    );

                    specificImplIsFound = true;
                    serviceImpl         = srv.first;
                    break;
                }

                if(oimpl == "sight::data::Object")
                {
                    SIGHT_ASSERT(
                        "Method has already found a generic service for the object ("
                        << oimpl << ").",
                        !genericImplIsFound
                    );
#ifdef _DEBUG
                    genericImplIsFound = true;
#endif
                    if(!specificImplIsFound)
                    {
                        serviceImpl = srv.first;
                        break;
                    }
                }
            }
        }
    }

    SIGHT_ASSERT("A default implementation is not found for this type of service " << type, !serviceImpl.empty());

    return serviceImpl;
}

//-----------------------------------------------------------------------------

const std::vector<std::string>& Factory::getServiceObjects(const std::string& _srvImpl) const
{
    const std::string srvImpl = core::runtime::filterID(_srvImpl);
    std::string objImpl;
    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);
    SIGHT_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplToSrvInfo.end());
    return iter->second.objectImpl;
}

//-----------------------------------------------------------------------------

std::string Factory::getServiceDescription(const std::string& _srvImpl) const
{
    const std::string srvImpl = core::runtime::filterID(_srvImpl);
    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);
    SIGHT_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplToSrvInfo.end());
    return iter->second.desc;
}

//-----------------------------------------------------------------------------

std::string Factory::getServiceTags(const std::string& _srvImpl) const
{
    const std::string srvImpl = core::runtime::filterID(_srvImpl);
    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);
    SIGHT_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplToSrvInfo.end());
    return iter->second.tags;
}

//-----------------------------------------------------------------------------

bool Factory::checkServiceValidity(const std::string& _object, const std::string& _srvImpl) const
{
    const std::string object  = core::runtime::filterID(_object);
    const std::string srvImpl = core::runtime::filterID(_srvImpl);
    bool isValid              = true;
    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);
    isValid &= (iter != m_srvImplToSrvInfo.end());
    if(isValid)
    {
        const ServiceInfo& srvInfo = iter->second;

        isValid = false;
        for(const auto& oimpl : srvInfo.objectImpl)
        {
            if(oimpl == "sight::data::Object" || oimpl == object)
            {
                isValid = true;
                break;
            }
        }
    }

    return isValid;
}

//-----------------------------------------------------------------------------

bool Factory::support(const std::string& _object, const std::string& _srvType, const std::string& _srvImpl) const
{
    const std::string object  = core::runtime::filterID(_object);
    const std::string srvType = core::runtime::filterID(_srvType);
    const std::string srvImpl = core::runtime::filterID(_srvImpl);

    bool isSupported = true;
    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    auto iter = m_srvImplToSrvInfo.find(srvImpl);
    isSupported &= (iter != m_srvImplToSrvInfo.end());
    if(isSupported)
    {
        const ServiceInfo& srvInfo = iter->second;

        isSupported = false;
        if(srvInfo.serviceType == srvType)
        {
            for(const auto& oimpl : srvInfo.objectImpl)
            {
                if(oimpl == "sight::data::Object" || oimpl == object)
                {
                    isSupported = true;
                    break;
                }
            }
        }
    }

    return isSupported;
}

//-----------------------------------------------------------------------------

bool Factory::support(const std::string& _object, const std::string& _srvType)
{
    const std::string object  = core::runtime::filterID(_object);
    const std::string srvType = core::runtime::filterID(_srvType);

    bool isSupported = false;
    SupportMapType::key_type key(object, srvType);
    core::mt::ReadToWriteLock supportMapLock(m_supportMapMutex);
    SupportMapType::const_iterator iter = m_supportMap.find(key);
    if(iter != m_supportMap.end())
    {
        isSupported = iter->second;
    }
    else
    {
        core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
        for(const SrvRegContainer::value_type& srv : m_srvImplToSrvInfo)
        {
            const ServiceInfo& srvInfo = srv.second;

            if(srvInfo.serviceType == srvType)
            {
                for(const auto& oimpl : srvInfo.objectImpl)
                {
                    if(oimpl == "sight::data::Object" || oimpl == object)
                    {
                        isSupported = true;
                        break;
                    }
                }
            }
        }

        core::mt::UpgradeToWriteLock upgrade(supportMapLock);
        m_supportMap.insert(SupportMapType::value_type(key, isSupported));
    }

    return isSupported;
}

//-----------------------------------------------------------------------------

Factory::KeyVectorType Factory::getFactoryKeys() const
{
    core::mt::ReadLock lock(m_srvImplToSrvInfoMutex);
    KeyVectorType vectKeys;
    std::transform(
        m_srvImplToSrvInfo.begin(),
        m_srvImplToSrvInfo.end(),
        std::back_inserter(vectKeys),
        [](const auto& e){return e.first;});
    return vectKeys;
}

//-----------------------------------------------------------------------------

} // namespace sight::service::extension

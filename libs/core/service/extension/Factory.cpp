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

#include "service/extension/Factory.hpp"

#include "service/IService.hpp"
#include "core/thread/ActiveWorkers.hpp"

#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/runtime/Runtime.hpp>
#include <core/LazyInstantiator.hpp>

#include <data/Exception.hpp>

#include <functional>
#include <vector>

namespace sight::service
{
namespace extension
{

//-----------------------------------------------------------------------------

Factory::sptr Factory::getDefault()
{
    return core::LazyInstantiator< Factory >::getInstance();
}
//-----------------------------------------------------------------------------

void Factory::parseBundleInformation()
{
    SrvRegContainer moduleInfoMap;

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::shared_ptr< core::runtime::Extension > ExtensionType;

    std::vector< ExtensionType >  extElements;
    extElements = core::runtime::getAllExtensionsForPoint("::sight::service::extension::Factory");
    for(ExtensionType extElt :  extElements)
    {
        std::vector< ConfigurationType > cfgEltVec = extElt->getElements();
        std::string type;
        std::string service;
        std::vector<std::string> objects;
        std::string desc;
        std::string tags;

        for(ConfigurationType cfgElt :  cfgEltVec)
        {
            std::string elt = cfgElt->getName();
            if(elt == "type")
            {
                type = cfgElt->getValue();
            }
            else if(elt == "service")
            {
                service = cfgElt->getValue();
            }
            else if(elt == "object")
            {
                objects.push_back(cfgElt->getValue());
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
                SLM_FATAL("Unknown element !");
            }
        }
        SLM_ASSERT("Missing type element.", !type.empty());
        SLM_ASSERT("Missing service element.", !service.empty());

        ServiceInfo info;
        info.serviceType          = type;
        info.objectsSetFromModule = !objects.empty();
        info.objectImpl           = std::move(objects);
        info.desc                 = desc;
        info.tags                 = tags;
        info.module               = cfgEltVec[0]->getModule();
        SLM_ASSERT("Module not find.", info.module );

        moduleInfoMap.emplace(std::make_pair(service, info));
    }

    this->printInfoMap( moduleInfoMap );

    core::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    // Merge data info
    for(SrvRegContainer::value_type module : moduleInfoMap)
    {
        SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( module.first );

        if ( iter != m_srvImplTosrvInfo.end() )
        {
            SLM_DEBUG(
                "We already have informations about this service  (from register macro) ( "<< module.first <<
                    " )." );

            ServiceInfo& info       = iter->second;
            ServiceInfo& infoModule = module.second;

            SLM_ASSERT("Try to add a module, but this module already exists.", !info.module );
            SLM_ASSERT("Try to add a module, but this srv is already registered and doesn't have the same srv type.",
                       infoModule.serviceType == info.serviceType );
            SLM_ASSERT("Try to add a module, but the service '"
                       << module.first << "' is already registered and does not have the same objects.",
                       infoModule.objectImpl.empty() || infoModule.objectImpl == info.objectImpl );

            info.module               = infoModule.module;
            info.desc                 = infoModule.desc;
            info.objectsSetFromModule = infoModule.objectsSetFromModule;
        }
        else
        {
            core::mt::UpgradeToWriteLock upgrade(lock);
            m_srvImplTosrvInfo.emplace(std::make_pair(module.first, module.second));
        }

    }

    this->printInfoMap( m_srvImplTosrvInfo );
    this->checkServicesNotDeclaredInPluginXml();
}

//-----------------------------------------------------------------------------

IService::sptr Factory::create( const std::string& _srvImpl ) const
{
    IService::sptr service;

    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( _srvImpl );

    SLM_ASSERT("The service called '" << _srvImpl << "' does not exist in the Factory ",
               iter != m_srvImplTosrvInfo.end() );

    const ServiceInfo& info = iter->second;

    SLM_DEBUG("SR creates a new service ( classname = " << _srvImpl << " )");

    if ( info.factory )
    {
        service = info.factory();
    }
    else
    {
        SLM_ASSERT( "A module must declare the factory named"
                    << _srvImpl
                    <<", the service declaration might be missing (or misspelled) in a module plugin.",
                    info.module );
        SLM_ASSERT( "The module '" + info.module->getIdentifier() + "' is already loaded and the factory '"
                    + _srvImpl + "' is still missing. The service declaration might be missing (or misspelled)"
                    "in a .cpp file.", !info.module->isStarted() );

        lock.unlock(); // module->start() may trigger calls to addFactory
        info.module->start();
        lock.lock();

        core::runtime::getCurrentProfile()->setup();

        FW_RAISE_EXCEPTION_IF(
            data::Exception( "After loading the module " + info.module->getIdentifier() + " , factory " + _srvImpl
                             + " is still missing. The service declaration might be missing (or misspelled) "
                             "in a cpp file."),
            !info.factory );

        service = info.factory();
    }

    // Setup worker here, this is a better place than the constructor
    // because here, the service slots are also set up
    // This allows to setup
    service->setWorker( core::thread::ActiveWorkers::getDefaultWorker() );

    return service;
}

//-----------------------------------------------------------------------------

IService::sptr Factory::create( const std::string& _srvType, const std::string& _srvImpl ) const
{
#ifdef _DEBUG
    {
        core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);

        SLM_ASSERT("The service called " << _srvImpl << " does not exist in the Factory.",
                   m_srvImplTosrvInfo.find( _srvImpl ) != m_srvImplTosrvInfo.end() );

        SLM_ASSERT(
            "Conflicting types were defined for this service, "
                << _srvType << " != " << m_srvImplTosrvInfo.find( _srvImpl )->second.serviceType,
                _srvType == m_srvImplTosrvInfo.find( _srvImpl )->second.serviceType);
    }
#else
    FwCoreNotUsedMacro(_srvType);
#endif //_DEBUG

    IService::sptr service = this->create(_srvImpl);
    return service;
}

//------------------------------------------------------------------------------

void Factory::addServiceFactory( FactoryType _factory,
                                        const std::string& simpl,
                                        const std::string& stype)
{
    SLM_DEBUG("New service registering : simpl =" + simpl + " stype=" + stype);

    core::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( simpl );

    if ( iter != m_srvImplTosrvInfo.end() )
    {
        SLM_DEBUG("We already have informations about this service ( " + simpl + " )." );
        ServiceInfo& info = iter->second;
        SLM_ASSERT("Try to add factory, but this srv ( " << simpl << " ) already has a registered factory.",
                   !info.factory );
        SLM_ASSERT("Try to add factory, but this srv ( "
                   << simpl << " ) is already registered and doesn't have the same srv type. ( "
                   << stype << " != " << info.serviceType <<" )",
                   stype == info.serviceType );

        core::mt::UpgradeToWriteLock upgrade(lock);
        info.factory = _factory;
    }
    else
    {
        SLM_DEBUG("Add new service factory in registry ( " + simpl + " )." );
        core::mt::UpgradeToWriteLock upgrade(lock);
        ServiceInfo info;
        info.serviceType = stype;
        info.factory     = _factory;
        m_srvImplTosrvInfo.emplace(std::make_pair(simpl, info));
    }
}

//-----------------------------------------------------------------------------

void Factory::addObjectFactory(const std::string& simpl, const std::string& oimpl)
{
    SLM_DEBUG("New object oimpl=" + oimpl + "registering to service: simpl =" + simpl);
    SLM_ASSERT("Empty oimpl", !oimpl.empty());

    core::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( simpl );

    SLM_ASSERT("Try to associate an object to a service factory, but this srv is not yet registered.",
               iter != m_srvImplTosrvInfo.end());

    if ( iter != m_srvImplTosrvInfo.end() )
    {
        ServiceInfo& info = iter->second;

        // Either the module does not contain objects informations or this service does not belong to a module
        if(info.objectsSetFromModule)
        {
#ifdef _DEBUG
            const auto itFind = std::find(info.objectImpl.begin(), info.objectImpl.end(), oimpl);
#endif
            SLM_ASSERT("Try to add factory, but the service '" + simpl + "' is already registered and does not have the "
                       "same objects.",
                       info.objectImpl.empty() || itFind != info.objectImpl.end());
        }
        else
        {
            core::mt::UpgradeToWriteLock upgrade(lock);
            info.objectImpl.push_back(oimpl);
        }
    }
}

//-----------------------------------------------------------------------------

void Factory::printInfoMap( const SrvRegContainer& src ) const
{
    // not thread-safe

    //Print information
    for(SrvRegContainer::value_type srvReg :  src)
    {
        SLM_DEBUG(" Service name = " << srvReg.first );
        SLM_DEBUG("  - type   = " << srvReg.second.serviceType );

#if SLM_DEBUG_ENABLED
        size_t objNum = 0;
        for(const auto& objImpl : srvReg.second.objectImpl)
        {
            SLM_DEBUG("  - object " << objNum++ << " = " << objImpl)
        }
#endif

        SLM_DEBUG_IF("  - module = " <<  srvReg.second.module->getIdentifier(), srvReg.second.module );
        SLM_DEBUG_IF("  - module = ( no module registered )", !srvReg.second.module );

        SLM_DEBUG_IF("  - name after creation = "
                     <<  srvReg.second.factory()->getClassname(), srvReg.second.factory );
        SLM_DEBUG_IF("  - name after creation = ( no factory registered )", !srvReg.second.factory );
    }
}

//-----------------------------------------------------------------------------

void Factory::checkServicesNotDeclaredInPluginXml() const
{
    // not thread-safe
    //Print information
    for(SrvRegContainer::value_type srvReg :  m_srvImplTosrvInfo)
    {
        if ( !srvReg.second.module )
        {
            SLM_WARN("Service " << srvReg.first << " is not declared/found in a plugin.xml." );
        }
    }
}

//-----------------------------------------------------------------------------

void Factory::clearFactory()
{
    core::mt::WriteLock lock(m_srvImplTosrvInfoMutex);
    m_srvImplTosrvInfo.clear();
}

//-----------------------------------------------------------------------------

std::vector< std::string > Factory::getImplementationIdFromObjectAndType( const std::string& object,
                                                                                 const std::string& type ) const
{
    std::vector< std::string > serviceImpl;

    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    for(SrvRegContainer::value_type srv :  m_srvImplTosrvInfo)
    {
        const ServiceInfo& srvInfo = srv.second;
        for(const auto& oimpl : srvInfo.objectImpl)
        {
            if( srvInfo.serviceType == type &&
                (oimpl == object || oimpl == "::sight::data::Object") )
            {
                serviceImpl.push_back(srv.first);
                break;
            }
        }
    }
    return serviceImpl;
}

//-----------------------------------------------------------------------------

std::string Factory::getDefaultImplementationIdFromObjectAndType( const std::string& object,
                                                                         const std::string& type ) const
{
    SLM_ASSERT("This case is not managed ", object != "::sight::data::Object" );

    std::string serviceImpl = "";
#ifdef _DEBUG
    bool genericImplIsFound = false;
#endif
    bool specificImplIsFound = false;

    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    for( SrvRegContainer::value_type srv :  m_srvImplTosrvInfo )
    {
        const ServiceInfo& srvInfo = srv.second;
        if ( srvInfo.serviceType == type )
        {
            for(const auto& oimpl : srvInfo.objectImpl)
            {
                if ( oimpl == object )
                {
                    SLM_ASSERT("Method has already found a specific ("
                               << serviceImpl <<" != " << srv.first
                               << ") service for the object " << oimpl << ".",
                               !specificImplIsFound );

                    specificImplIsFound = true;
                    serviceImpl         = srv.first;
                    break;
                }
                else if ( oimpl == "::sight::data::Object" )
                {
                    SLM_ASSERT("Method has already found a generic service for the object ("
                               << oimpl << ").",
                               !genericImplIsFound );
#ifdef _DEBUG
                    genericImplIsFound = true;
#endif
                    if ( !specificImplIsFound )
                    {
                        serviceImpl = srv.first;
                        break;
                    }
                }
            }
        }
    }

    SLM_ASSERT("A default implementation is not found for this type of service "<<type, !serviceImpl.empty() );

    return serviceImpl;
}

//-----------------------------------------------------------------------------

const std::vector<std::string>& Factory::getServiceObjects(const std::string& srvImpl) const
{
    std::string objImpl;
    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    return iter->second.objectImpl;
}

//-----------------------------------------------------------------------------

std::string Factory::getServiceDescription(const std::string& srvImpl) const
{
    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    return iter->second.desc;
}

//-----------------------------------------------------------------------------

std::string Factory::getServiceTags(const std::string& srvImpl) const
{
    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    return iter->second.tags;
}

//-----------------------------------------------------------------------------

bool Factory::checkServiceValidity(const std::string& objectClassName, const std::string& srvImpl) const
{
    bool isValid = true;
    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    isValid &= ( iter != m_srvImplTosrvInfo.end() );
    if (isValid)
    {
        const ServiceInfo& srvInfo = iter->second;

        isValid = false;
        for(const auto& oimpl : srvInfo.objectImpl)
        {
            if(oimpl == "::sight::data::Object" || oimpl == objectClassName)
            {
                isValid = true;
                break;
            }
        }
    }
    return isValid;
}

//-----------------------------------------------------------------------------

bool Factory::support(const std::string& object, const std::string& srvType, const std::string& srvImpl) const
{
    bool isSupported = true;
    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    isSupported &= ( iter != m_srvImplTosrvInfo.end());
    if (isSupported)
    {
        const ServiceInfo& srvInfo = iter->second;

        isSupported = false;
        if(srvInfo.serviceType == srvType)
        {
            for(const auto& oimpl : srvInfo.objectImpl)
            {
                if(oimpl == "::sight::data::Object" || oimpl == object)
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

bool Factory::support(const std::string& object, const std::string& srvType)
{
    bool isSupported = false;
    SupportMapType::key_type key(object, srvType);
    core::mt::ReadToWriteLock supportMapLock(m_supportMapMutex);
    SupportMapType::const_iterator iter = m_supportMap.find( key );
    if (iter != m_supportMap.end())
    {
        isSupported = iter->second;
    }
    else
    {
        core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
        for(SrvRegContainer::value_type srv :  m_srvImplTosrvInfo)
        {
            const ServiceInfo& srvInfo = srv.second;

            if(srvInfo.serviceType == srvType)
            {
                for(const auto& oimpl : srvInfo.objectImpl)
                {
                    if(oimpl == "::sight::data::Object" || oimpl == object)
                    {
                        isSupported = true;
                        break;
                    }
                }
            }
        }
        core::mt::UpgradeToWriteLock upgrade(supportMapLock);
        m_supportMap.insert( SupportMapType::value_type(key, isSupported) );
    }
    return isSupported;
}

//-----------------------------------------------------------------------------

Factory::KeyVectorType Factory::getFactoryKeys() const
{
    core::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    KeyVectorType vectKeys;
    std::transform( m_srvImplTosrvInfo.begin(), m_srvImplTosrvInfo.end(),
                    std::back_inserter(vectKeys),
                    std::bind(&SrvRegContainer::value_type::first, std::placeholders::_1) );
    return vectKeys;
}

//-----------------------------------------------------------------------------

} // namespace extension
} // namespace sight::service

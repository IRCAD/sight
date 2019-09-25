/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwServices/registry/ServiceFactory.hpp"

#include "fwServices/IService.hpp"
#include "fwServices/registry/ActiveWorkers.hpp"

#include <fwCore/util/LazyInstantiator.hpp>

#include <fwData/Exception.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/Runtime.hpp>

#include <functional>
#include <vector>

namespace fwServices
{
namespace registry
{

//-----------------------------------------------------------------------------

ServiceFactory::sptr ServiceFactory::getDefault()
{
    return ::fwCore::util::LazyInstantiator< ServiceFactory >::getInstance();
}
//-----------------------------------------------------------------------------

void ServiceFactory::parseBundleInformation()
{
    SrvRegContainer bundleInfoMap;

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::shared_ptr< ::fwRuntime::Extension > ExtensionType;

    std::vector< ExtensionType >  extElements;
    extElements = ::fwRuntime::getAllExtensionsForPoint("::fwServices::registry::ServiceFactory");
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
        info.objectsSetFromBundle = !objects.empty();
        info.objectImpl           = std::move(objects);
        info.desc                 = desc;
        info.tags                 = tags;
        info.bundle               = cfgEltVec[0]->getBundle();
        SLM_ASSERT("Bundle not find.", info.bundle );

        bundleInfoMap.emplace(std::make_pair(service, info));
    }

    this->printInfoMap( bundleInfoMap );

    ::fwCore::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    // Merge data info
    for(SrvRegContainer::value_type bundle : bundleInfoMap)
    {
        SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( bundle.first );

        if ( iter != m_srvImplTosrvInfo.end() )
        {
            OSLM_DEBUG(
                "We already have informations about this service  (from register macro) ( "<< bundle.first <<
                    " )." );

            ServiceInfo& info       = iter->second;
            ServiceInfo& infoBundle = bundle.second;

            SLM_ASSERT("Try to add bundle, but bundle exists.", !info.bundle );
            SLM_ASSERT("Try to add bundle, but this srv is already registered and doesn't have the same srv type.",
                       infoBundle.serviceType == info.serviceType );
            SLM_ASSERT("Try to add bundle, but the service '"
                       << bundle.first << "' is already registered and does not have the same objects.",
                       infoBundle.objectImpl.empty() || infoBundle.objectImpl == info.objectImpl );

            info.bundle               = infoBundle.bundle;
            info.desc                 = infoBundle.desc;
            info.objectsSetFromBundle = infoBundle.objectsSetFromBundle;
        }
        else
        {
            ::fwCore::mt::UpgradeToWriteLock upgrade(lock);
            m_srvImplTosrvInfo.emplace(std::make_pair(bundle.first, bundle.second));
        }

    }

    this->printInfoMap( m_srvImplTosrvInfo );
    this->checkServicesNotDeclaredInPluginXml();
}

//-----------------------------------------------------------------------------

IService::sptr ServiceFactory::create( const std::string& _srvImpl ) const
{
    IService::sptr service;

    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( _srvImpl );

    OSLM_ASSERT("The service called '" << _srvImpl << "' does not exist in the ServiceFactory ",
                iter != m_srvImplTosrvInfo.end() );

    const ServiceInfo& info = iter->second;

    OSLM_DEBUG("SR creates a new service ( classname = " << _srvImpl << " )");

    if ( info.factory )
    {
        service = info.factory();
    }
    else
    {
        OSLM_ASSERT( "A bundle must declare the factory named"
                     << _srvImpl
                     <<", the service declaration might be missing (or misspelled) in a bundle plugin.",
                     info.bundle );
        OSLM_ASSERT( "The bundle '" + info.bundle->getIdentifier() + "' is already loaded and the factory '"
                     + _srvImpl + "' is still missing. The service declaration might be missing (or misspelled)"
                     "in a .cpp file.", !info.bundle->isStarted() );

        lock.unlock(); // bundle->start() may trigger calls to addFactory
        info.bundle->start();
        lock.lock();

        ::fwRuntime::profile::getCurrentProfile()->setup();

        FW_RAISE_EXCEPTION_IF(
            ::fwData::Exception( "After loading the bundle " + info.bundle->getIdentifier() + " , factory " + _srvImpl
                                 + " is still missing. The service declaration might be missing (or misspelled) "
                                 "in a cpp file."),
            !info.factory );

        service = info.factory();
    }

    // Setup worker here, this is a better place than the constructor
    // because here, the service slots are also set up
    // This allows to setup
    service->setWorker( ::fwServices::registry::ActiveWorkers::getDefaultWorker() );

    return service;
}

//-----------------------------------------------------------------------------

IService::sptr ServiceFactory::create( const std::string& _srvType, const std::string& _srvImpl ) const
{
#ifdef _DEBUG
    {
        ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);

        OSLM_ASSERT("The service called " << _srvImpl << " does not exist in the ServiceFactory.",
                    m_srvImplTosrvInfo.find( _srvImpl ) != m_srvImplTosrvInfo.end() );

        OSLM_ASSERT(
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

void ServiceFactory::addServiceFactory( FactoryType _factory,
                                        const std::string& simpl,
                                        const std::string& stype)
{
    SLM_DEBUG("New service registering : simpl =" + simpl + " stype=" + stype);

    ::fwCore::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( simpl );

    if ( iter != m_srvImplTosrvInfo.end() )
    {
        SLM_DEBUG("We already have informations about this service ( " + simpl + " )." );
        ServiceInfo& info = iter->second;
        OSLM_ASSERT("Try to add factory, but this srv ( " << simpl << " ) already has a registered factory.",
                    !info.factory );
        OSLM_ASSERT("Try to add factory, but this srv ( "
                    << simpl << " ) is already registered and doesn't have the same srv type. ( "
                    << stype << " != " << info.serviceType <<" )",
                    stype == info.serviceType );

        ::fwCore::mt::UpgradeToWriteLock upgrade(lock);
        info.factory = _factory;
    }
    else
    {
        SLM_DEBUG("Add new service factory in registry ( " + simpl + " )." );
        ::fwCore::mt::UpgradeToWriteLock upgrade(lock);
        ServiceInfo info;
        info.serviceType = stype;
        info.factory     = _factory;
        m_srvImplTosrvInfo.emplace(std::make_pair(simpl, info));
    }
}

//-----------------------------------------------------------------------------

void ServiceFactory::addObjectFactory(const std::string& simpl, const std::string& oimpl)
{
    SLM_DEBUG("New object oimpl=" + oimpl + "registering to service: simpl =" + simpl);
    SLM_ASSERT("Empty oimpl", !oimpl.empty());

    ::fwCore::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( simpl );

    OSLM_ASSERT("Try to associate an object to a service factory, but this srv is not yet registered.",
                iter != m_srvImplTosrvInfo.end());

    if ( iter != m_srvImplTosrvInfo.end() )
    {
        ServiceInfo& info = iter->second;

        // Either the bundle does not contain objects informations or this service does not belong to a bundle
        if(info.objectsSetFromBundle)
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
            ::fwCore::mt::UpgradeToWriteLock upgrade(lock);
            info.objectImpl.push_back(oimpl);
        }
    }
}

//-----------------------------------------------------------------------------

void ServiceFactory::printInfoMap( const SrvRegContainer& src ) const
{
    // not thread-safe

    //Print information
    for(SrvRegContainer::value_type srvReg :  src)
    {
        OSLM_DEBUG(" Service name = " << srvReg.first );
        OSLM_DEBUG("  - type   = " << srvReg.second.serviceType );

#if SLM_DEBUG_ENABLED
        size_t objNum = 0;
        for(const auto& objImpl : srvReg.second.objectImpl)
        {
            OSLM_DEBUG("  - object " << objNum++ << " = " << objImpl)
        }
#endif

        OSLM_DEBUG_IF("  - bundle = " <<  srvReg.second.bundle->getIdentifier(), srvReg.second.bundle );
        OSLM_DEBUG_IF("  - bundle = ( no bundle registered )", !srvReg.second.bundle );

        OSLM_DEBUG_IF("  - name after creation = "
                      <<  srvReg.second.factory()->getClassname(), srvReg.second.factory );
        OSLM_DEBUG_IF("  - name after creation = ( no factory registered )", !srvReg.second.factory );
    }
}

//-----------------------------------------------------------------------------

void ServiceFactory::checkServicesNotDeclaredInPluginXml() const
{
    // not thread-safe
    //Print information
    for(SrvRegContainer::value_type srvReg :  m_srvImplTosrvInfo)
    {
        if ( !srvReg.second.bundle )
        {
            OSLM_WARN("Service " << srvReg.first << " is not declared/found in a plugin.xml." );
        }
    }
}

//-----------------------------------------------------------------------------

void ServiceFactory::clearFactory()
{
    ::fwCore::mt::WriteLock lock(m_srvImplTosrvInfoMutex);
    m_srvImplTosrvInfo.clear();
}

//-----------------------------------------------------------------------------

std::vector< std::string > ServiceFactory::getImplementationIdFromObjectAndType( const std::string& object,
                                                                                 const std::string& type ) const
{
    std::vector< std::string > serviceImpl;

    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    for(SrvRegContainer::value_type srv :  m_srvImplTosrvInfo)
    {
        const ServiceInfo& srvInfo = srv.second;
        for(const auto& oimpl : srvInfo.objectImpl)
        {
            if( srvInfo.serviceType == type &&
                (oimpl == object || oimpl == "::fwData::Object") )
            {
                serviceImpl.push_back(srv.first);
                break;
            }
        }
    }
    return serviceImpl;
}

//-----------------------------------------------------------------------------

std::string ServiceFactory::getDefaultImplementationIdFromObjectAndType( const std::string& object,
                                                                         const std::string& type ) const
{
    SLM_ASSERT("This case is not managed ", object != "::fwData::Object" );

    std::string serviceImpl  = "";
    bool genericImplIsFound  = false;
    bool specificImplIsFound = false;

    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    for( SrvRegContainer::value_type srv :  m_srvImplTosrvInfo )
    {
        const ServiceInfo& srvInfo = srv.second;
        if ( srvInfo.serviceType == type )
        {
            for(const auto& oimpl : srvInfo.objectImpl)
            {
                if ( oimpl == object )
                {
                    OSLM_ASSERT("Method has already found a specific ("
                                << serviceImpl <<" != " << srv.first
                                << ") service for the object " << oimpl << ".",
                                !specificImplIsFound );

                    specificImplIsFound = true;
                    serviceImpl         = srv.first;
                    break;
                }
                else if ( oimpl == "::fwData::Object" )
                {
                    OSLM_ASSERT("Method has already found a generic service for the object ("
                                << oimpl << ").",
                                !genericImplIsFound );

                    genericImplIsFound = true;
                    if ( !specificImplIsFound )
                    {
                        serviceImpl = srv.first;
                        break;
                    }
                }
            }
        }
    }

    OSLM_ASSERT("A default implementation is not found for this type of service "<<type, !serviceImpl.empty() );

    return serviceImpl;
}

//-----------------------------------------------------------------------------

const std::vector<std::string>& ServiceFactory::getServiceObjects(const std::string& srvImpl) const
{
    std::string objImpl;
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    return iter->second.objectImpl;
}

//-----------------------------------------------------------------------------

std::string ServiceFactory::getServiceDescription(const std::string& srvImpl) const
{
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    return iter->second.desc;
}

//-----------------------------------------------------------------------------

std::string ServiceFactory::getServiceTags(const std::string& srvImpl) const
{
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    return iter->second.tags;
}

//-----------------------------------------------------------------------------

bool ServiceFactory::checkServiceValidity(const std::string& objectClassName, const std::string& srvImpl) const
{
    bool isValid = true;
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    isValid &= ( iter != m_srvImplTosrvInfo.end() );
    if (isValid)
    {
        const ServiceInfo& srvInfo = iter->second;

        isValid = false;
        for(const auto& oimpl : srvInfo.objectImpl)
        {
            if(oimpl == "::fwData::Object" || oimpl == objectClassName)
            {
                isValid = true;
                break;
            }
        }
    }
    return isValid;
}

//-----------------------------------------------------------------------------

bool ServiceFactory::support(const std::string& object, const std::string& srvType, const std::string& srvImpl) const
{
    bool isSupported = true;
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
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
                if(oimpl == "::fwData::Object" || oimpl == object)
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

bool ServiceFactory::support(const std::string& object, const std::string& srvType)
{
    bool isSupported = false;
    SupportMapType::key_type key(object, srvType);
    ::fwCore::mt::ReadToWriteLock supportMapLock(m_supportMapMutex);
    SupportMapType::const_iterator iter = m_supportMap.find( key );
    if (iter != m_supportMap.end())
    {
        isSupported = iter->second;
    }
    else
    {
        ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
        for(SrvRegContainer::value_type srv :  m_srvImplTosrvInfo)
        {
            const ServiceInfo& srvInfo = srv.second;

            if(srvInfo.serviceType == srvType)
            {
                for(const auto& oimpl : srvInfo.objectImpl)
                {
                    if(oimpl == "::fwData::Object" || oimpl == object)
                    {
                        isSupported = true;
                        break;
                    }
                }
            }
        }
        ::fwCore::mt::UpgradeToWriteLock upgrade(supportMapLock);
        m_supportMap.insert( SupportMapType::value_type(key, isSupported) );
    }
    return isSupported;
}

//-----------------------------------------------------------------------------

ServiceFactory::KeyVectorType ServiceFactory::getFactoryKeys() const
{
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    KeyVectorType vectKeys;
    std::transform( m_srvImplTosrvInfo.begin(), m_srvImplTosrvInfo.end(),
                    std::back_inserter(vectKeys),
                    std::bind(&SrvRegContainer::value_type::first, std::placeholders::_1) );
    return vectKeys;
}

//-----------------------------------------------------------------------------

} // namespace registry
} // namespace fwServices

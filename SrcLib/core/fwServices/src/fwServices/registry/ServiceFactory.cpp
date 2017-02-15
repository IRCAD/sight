/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/registry/ServiceFactory.hpp"

#include "fwServices/IService.hpp"
#include "fwServices/registry/ActiveWorkers.hpp"

#include <fwCore/util/LazyInstantiator.hpp>

#include <fwData/Exception.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/profile/Profile.hpp>

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
        std::string object;
        std::string desc;

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
                object = cfgElt->getValue();
            }
            else if(elt == "desc")
            {
                desc = cfgElt->getValue();
            }
            else
            {
                SLM_FATAL("Unknown element !");
            }
        }
        SLM_ASSERT("Missing type element.", !type.empty());
        SLM_ASSERT("Missing service element.", !service.empty());

        ServiceInfo info;
        info.serviceType = type;
        info.objectImpl  = object;
        info.desc        = desc;
        info.bundle      = cfgEltVec[0]->getBundle();
        SLM_ASSERT("Bundle not find.", info.bundle );

        bundleInfoMap.emplace(std::make_pair(service, info));
    }

#if SPYLOG_LEVEL >= 5
    //Print information
    this->printInfoMap( bundleInfoMap );
#endif

    ::fwCore::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    // Merge data info
    for(SrvRegContainer::value_type bundle : bundleInfoMap)
    {
        SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( bundle.first );

        if ( iter != m_srvImplTosrvInfo.end() )
        {
            OSLM_DEBUG("Have already information (from register macro) about this service ( "<< bundle.first << " )." );

            ServiceInfo& info       = iter->second;
            ServiceInfo& infoBundle = bundle.second;

            SLM_ASSERT("Try to add bundle, but bundle exists.", !info.bundle );
            SLM_ASSERT("Try to add bundle, but this srv is already register and has not the same srv type.",
                       infoBundle.serviceType == info.serviceType );
            SLM_ASSERT("Try to add bundle, but this srv ("
                       << bundle.first << ")is already register and has not the same oimpl. ( "
                       << infoBundle.objectImpl <<" != "<< info.objectImpl <<" )",
                       infoBundle.objectImpl == info.objectImpl );

            info.bundle = infoBundle.bundle;
            info.desc   = infoBundle.desc;
        }
        else
        {
            ::fwCore::mt::UpgradeToWriteLock upgrade(lock);
            m_srvImplTosrvInfo.emplace(std::make_pair(bundle.first, bundle.second));
        }

    }

#if SPYLOG_LEVEL >= 5
    //Print information
    this->printInfoMap( m_srvImplTosrvInfo );
#endif
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
    // because here, the service slots are also setup
    // This allow to setup
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
            "Conflincting types were defined for this service, "
            << _srvType << " != " << m_srvImplTosrvInfo.find( _srvImpl )->second.serviceType,
            _srvType == m_srvImplTosrvInfo.find( _srvImpl )->second.serviceType);
    }
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
        SLM_DEBUG("Have already information about this service ( " + simpl + " )." );
        ServiceInfo& info = iter->second;
        OSLM_ASSERT("Try to add factory, but this srv ( " << simpl << " ) has already a registered factory.",
                    !info.factory );
        OSLM_ASSERT("Try to add factory, but this srv ( "
                    << simpl << " ) is already register and has not the same srv type. ( "
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

    ::fwCore::mt::ReadToWriteLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( simpl );

    OSLM_ASSERT("Try to associate an object to a service factory, but this srv is not yet registered.",
                iter != m_srvImplTosrvInfo.end());

    if ( iter != m_srvImplTosrvInfo.end() )
    {
        ServiceInfo& info = iter->second;
        SLM_ASSERT("Try to add factory, but this srv ( " + simpl + " ) is already registered "
                   "and has not the same oimpl. ( " + oimpl + " != " + info.objectImpl + ")",
                   info.objectImpl.empty() || oimpl == info.objectImpl );

        ::fwCore::mt::UpgradeToWriteLock upgrade(lock);
        info.objectImpl = oimpl;
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
        OSLM_DEBUG("  - object = " << srvReg.second.objectImpl);

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
        if(srvInfo.serviceType == type
           && (srvInfo.objectImpl == object || srvInfo.objectImpl == "::fwData::Object") )
        {
            serviceImpl.push_back(srv.first);
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
            if ( srvInfo.objectImpl == object )
            {
                OSLM_ASSERT("Method has already found a specific ("
                            << serviceImpl <<" != " << srv.first
                            << ") service for the object " << srvInfo.objectImpl << ".",
                            !specificImplIsFound );

                specificImplIsFound = true;
                serviceImpl         = srv.first;
            }
            else if ( srvInfo.objectImpl == "::fwData::Object" )
            {
                OSLM_ASSERT("Method has already found a generic service for the object ("
                            << srvInfo.objectImpl << ").",
                            !genericImplIsFound );

                genericImplIsFound = true;
                if ( !specificImplIsFound )
                {
                    serviceImpl = srv.first;
                }
            }
        }
    }

    OSLM_ASSERT("A default implementation is not found for this type of service "<<type, !serviceImpl.empty() );

    return serviceImpl;
}

//-----------------------------------------------------------------------------

std::string ServiceFactory::getObjectImplementation(const std::string& srvImpl) const
{
    std::string objImpl;
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    objImpl = iter->second.objectImpl;
    return objImpl;
}

//-----------------------------------------------------------------------------

std::string ServiceFactory::getServiceDescription(const std::string& srvImpl) const
{
    std::string srvDescription;
    ::fwCore::mt::ReadLock lock(m_srvImplTosrvInfoMutex);
    SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.find( srvImpl );
    SLM_ASSERT("The service " << srvImpl << " is not found.", iter != m_srvImplTosrvInfo.end());
    srvDescription = iter->second.desc;
    return srvDescription;
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
        isValid &= (srvInfo.objectImpl == "::fwData::Object" || srvInfo.objectImpl == objectClassName);
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
        isSupported &= (srvInfo.objectImpl == "::fwData::Object" || srvInfo.objectImpl == object);
        isSupported &= (srvInfo.serviceType == srvType);
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
            if(srvInfo.serviceType == srvType
               && (srvInfo.objectImpl == object || srvInfo.objectImpl == "::fwData::Object") )
            {
                isSupported = true;
                break;
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


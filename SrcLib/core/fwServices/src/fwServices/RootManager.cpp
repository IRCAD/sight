/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/GlobalEventManager.hpp"
#include "fwServices/registry/ServiceFactory.hpp"
#include "fwServices/registry/ServiceConfig.hpp"
#include "fwServices/registry/AppConfig.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"

#include "fwServices/RootManager.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

RootManager::sptr RootManager::getDefault()
{
    SLM_TRACE_FUNC();
    static RootManager::sptr m_instance = RootManager::New();
    return m_instance;
}

//-----------------------------------------------------------------------------

RootManager::~RootManager()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

RootManager::RootManager()
{
    SLM_TRACE_FUNC();
    m_rootObjectConfigurationName.first = false ;
    m_isRootInitialized = false ;
}

//------------------------------------------------------------------------------

void RootManager::setRootObjectConfigurationName(std::string name)
{
    SLM_ASSERT("Sorry, configuration name is an empty string.", ! name.empty() );
    getDefault()->m_rootObjectConfigurationName = std::pair< bool , std::string >( true , name ) ;
}

//------------------------------------------------------------------------------

void RootManager::setRootObjectConfigurationFile(std::string _rootObjectConfigurationFile)
{
    SLM_ASSERT("Sorry, configuration file is an empty string.", ! _rootObjectConfigurationFile.empty() );
    getDefault()->m_rootObjectConfigurationFile = std::pair< bool , std::string >( true , _rootObjectConfigurationFile ) ;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr RootManager::getRootObject()
{
    return getDefault()->m_ctm->getConfigRoot();
}

//------------------------------------------------------------------------------

void RootManager::initializeRootObject()
{

    SLM_ASSERT("Sorry, the OSR is already initialized.", ! getDefault()->m_isRootInitialized );
    SLM_ASSERT("Sorry, configuration name parameter is not initialized.", getDefault()->m_rootObjectConfigurationName.first );
    SLM_ASSERT("Sorry, configuration file parameter is not initialized.", getDefault()->m_rootObjectConfigurationFile.first );

    ::fwServices::registry::ServiceFactory::getDefault()->parseBundleInformation();

    // ToDo Correct this hack
    // Load another "pseudo" bundle
    ::boost::filesystem::path filePath ( getDefault()->m_rootObjectConfigurationFile.second );
    SPTR(::fwRuntime::Bundle) configBundle = ::fwRuntime::io::BundleDescriptorReader::createBundleFromXmlPlugin( filePath );
    ::fwRuntime::Runtime::getDefault()->addBundle( configBundle );
    configBundle->setEnable( true );

    // Validate bundle
    ::fwRuntime::Bundle::ExtensionConstIterator iter = configBundle->extensionsBegin();
    while( iter != configBundle->extensionsEnd() )
    {
        ::fwRuntime::Extension::Validity isValid = (*iter)->validate();
        OSLM_FATAL_IF("Sorry, extension " << (*iter)->getIdentifier() << " is not valid.", isValid == ::fwRuntime::Extension::Invalid );
        iter++;
    }

    // register service config
    ::fwServices::registry::ServiceConfig::getDefault()->parseBundleInformation();

    ::fwServices::registry::AppConfig::getDefault()->parseBundleInformation();

    getDefault()->m_ctm = AppConfigManager::New();
    ::fwRuntime::ConfigurationElement::csptr config = ::fwServices::registry::AppConfig::getDefault()->getStandardConfig( getDefault()->m_rootObjectConfigurationName.second );
    getDefault()->m_ctm->setConfig( ::fwRuntime::ConfigurationElement::constCast( config ) );
    getDefault()->m_ctm->launch();
    getDefault()->m_isRootInitialized = true;
}

//------------------------------------------------------------------------------

void RootManager::uninitializeRootObject()
{
    SLM_TRACE_FUNC();
    SLM_WARN_IF("(Hack) Sorry, the OSR must be initialized to uninitialize it. ToDo => transform in assert", ! getDefault()->m_isRootInitialized );
    if ( getDefault()->m_isRootInitialized )
    {
        // Setting initialization to false
        getDefault()->m_isRootInitialized = false ;

        assert( getDefault()->m_rootObjectConfigurationName.first ) ;
        // Stop services reported in m_rootObjectConfiguration before stopping everything

        //::fwServices::stopAndUnregister(getDefault()->m_rootObjectConfiguration) ;
        getDefault()->m_ctm->stopAndDestroy();

        OSLM_WARN_IF("Sorry, few services still exist before erasing root object ( cf debug following message )" << std::endl << ::fwServices::OSR::getRegistryInformation(),
                        ::fwServices::OSR::getDefault()->getOSContainer().size() != 1 || ::fwServices::OSR::getDefault()->getOSContainer().begin()->second.size() != 0 );

        ::fwServices::GlobalEventManager::getDefault()->clearMessages();

        SLM_TRACE("uninitializeRootObject : Reset the last shared_ptr on root object.");

        // Reset the root object: involve complete m_container clearing
        //getDefault()->m_rootObject.reset();
        //assert( getDefault()->m_rootObject.use_count() == 0 );

        ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
        SLM_TRACE("Stopping Profile");
        profile->stop();
        SLM_TRACE("Profile Stopped");

         // Clear all service configs
        ::fwServices::registry::ServiceConfig::getDefault()->clearRegistry();

        // Clear all app configuration
        ::fwServices::registry::AppConfig::getDefault()->clearRegistry();

        // Clear all service factories
        ::fwServices::registry::ServiceFactory::getDefault()->clearFactory();

        // Clear all factories before stop application.
        ::fwTools::ClassFactoryRegistry::getFactories().clear();
    }
}

//------------------------------------------------------------------------------

bool RootManager::isRootObjectInitialized()
{
    return getDefault()->m_isRootInitialized ;
}

//------------------------------------------------------------------------------

} // namespace fwServices


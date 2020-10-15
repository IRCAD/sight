/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwRuntime/detail/Module.hpp"

#include "fwRuntime/detail/EmptyPlugin.hpp"
#include "fwRuntime/detail/ExtensionPoint.hpp"
#include "fwRuntime/detail/profile/Initializer.hpp"
#include "fwRuntime/detail/profile/Profile.hpp"
#include "fwRuntime/detail/profile/Stopper.hpp"
#include "fwRuntime/detail/Runtime.hpp"
#include "fwRuntime/ExecutableFactory.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/utils/GenericExecutableFactory.hpp"

#include <fwCore/base.hpp>

#include <algorithm>
#include <cassert>
#include <exception>
#include <memory>

namespace fwRuntime
{

namespace detail
{

//------------------------------------------------------------------------------

SPTR( Module ) Module::m_loadingModule;

//------------------------------------------------------------------------------

SPTR( Module ) Module::getLoadingModule()
{
    return m_loadingModule;
}

//------------------------------------------------------------------------------

Module::Module( const std::filesystem::path& location,
                const std::string& id,
                const std::string& version,
                const std::string& c ) :
    m_resourcesLocation( location.lexically_normal() ),
    m_identifier( id ),
    m_version( version ),
    m_class( c )
{
    // Post-condition.
    SLM_ASSERT( "Invalid module location.",  m_resourcesLocation.is_absolute() == true );

    // Starting from Sight 13.0, the plugin.xml is now likely to be separated from the libraries in the build/install
    std::string strLocation           = location.string();
    std::filesystem::path strRCPrefix = MODULE_RC_PREFIX;
    const auto itModule               = strLocation.find(strRCPrefix.string());
    if(itModule != std::string::npos)
    {
        strLocation.replace(itModule, strRCPrefix.string().length(), std::string(MODULE_LIB_PREFIX));
    }

    // This may fail if the module does not contain any library, so we ignore the returned error
    m_libraryLocation = std::filesystem::weakly_canonical(std::filesystem::path(strLocation));
}

//------------------------------------------------------------------------------

void Module::addExecutableFactory( SPTR( ExecutableFactory )factory )
{
    m_executableFactories.insert( factory );
}

//------------------------------------------------------------------------------

Module::ExecutableFactoryConstIterator Module::executableFactoriesBegin() const
{
    return m_executableFactories.begin();
}

//------------------------------------------------------------------------------

Module::ExecutableFactoryConstIterator Module::executableFactoriesEnd() const
{
    return m_executableFactories.end();
}

//------------------------------------------------------------------------------

SPTR( ExecutableFactory ) Module::findExecutableFactory( const std::string& type ) const
{
    std::shared_ptr<ExecutableFactory> resExecutableFactory;
    for(const ExecutableFactoryContainer::value_type& factory : m_executableFactories)
    {
        if(factory->getType() == type)
        {
            resExecutableFactory = factory;
            break;
        }
    }
    return resExecutableFactory;
}

//------------------------------------------------------------------------------

void Module::addExtension( SPTR( Extension )extension )
{
    m_extensions.insert( extension );
}

//------------------------------------------------------------------------------

bool Module::hasExtension(const std::string& identifier) const
{
    bool hasExtension = false;
    for(const ExtensionContainer::value_type& extension : m_extensions)
    {
        if(extension->getIdentifier() == identifier)
        {
            hasExtension = true;
            break;
        }
    }
    return hasExtension;
}

//------------------------------------------------------------------------------

void Module::setEnableExtension(const std::string& identifier, const bool enable)
{
    for(const ExtensionContainer::value_type& extension : m_extensions)
    {
        if(extension->getIdentifier() == identifier)
        {
            extension->setEnable(enable);
            break; // The identifier is unique => can break the loop
        }
    }
}

//------------------------------------------------------------------------------

Module::ExtensionConstIterator Module::extensionsBegin() const
{
    return m_extensions.begin();
}

//------------------------------------------------------------------------------

Module::ExtensionConstIterator Module::extensionsEnd() const
{
    return m_extensions.end();
}
//------------------------------------------------------------------------------

void Module::addExtensionPoint( SPTR( ExtensionPoint )extensionPoint )
{
    m_extensionPoints.insert( extensionPoint );
}

//------------------------------------------------------------------------------

SPTR( ExtensionPoint ) Module::findExtensionPoint( const std::string& identifier ) const
{
    std::shared_ptr<ExtensionPoint> resExtensionPoint;
    for(const ExtensionPointContainer::value_type& extensionPoint :  m_extensionPoints)
    {
        if(extensionPoint->getIdentifier() == identifier && extensionPoint->isEnabled())
        {
            resExtensionPoint = extensionPoint;
            break;
        }
    }
    return resExtensionPoint;
}

//------------------------------------------------------------------------------

bool Module::hasExtensionPoint(const std::string& identifier) const
{
    bool hasExtensionPoint = false;
    for(const ExtensionPointContainer::value_type& extensionPoint :  m_extensionPoints)
    {
        if(extensionPoint->getIdentifier() == identifier)
        {
            hasExtensionPoint = true;
            break;
        }
    }
    return hasExtensionPoint;
}

//------------------------------------------------------------------------------

void Module::setEnableExtensionPoint(const std::string& identifier, const bool enable)
{
    for(const ExtensionPointContainer::value_type& extensionPoint :  m_extensionPoints)
    {
        if(extensionPoint->getIdentifier() == identifier)
        {
            extensionPoint->setEnable(enable);
            break;
        }
    }
}

//------------------------------------------------------------------------------

Module::ExtensionPointConstIterator Module::extensionPointsBegin() const
{
    return m_extensionPoints.begin();
}

//------------------------------------------------------------------------------

Module::ExtensionPointConstIterator Module::extensionPointsEnd() const
{
    return m_extensionPoints.end();
}

//------------------------------------------------------------------------------

void Module::addLibrary( SPTR(dl::Library)library )
{
    library->setSearchPath(this->getLibraryLocation());
    m_libraries.insert(library);
}

//------------------------------------------------------------------------------

void Module::addRequirement(const std::string& requirement)
{
    m_requirements.insert(requirement);
}

//------------------------------------------------------------------------------

const std::string Module::getClass() const
{
    return m_class;
}

//------------------------------------------------------------------------------

const std::string& Module::getIdentifier() const
{
    return m_identifier;
}

//------------------------------------------------------------------------------

const std::filesystem::path& Module::getLibraryLocation() const
{
    return m_libraryLocation;
}

//------------------------------------------------------------------------------

const std::filesystem::path& Module::getResourcesLocation() const
{
    return m_resourcesLocation;
}

//------------------------------------------------------------------------------

SPTR( IPlugin ) Module::getPlugin() const
{
    return m_plugin;
}

//------------------------------------------------------------------------------

const Version& Module::getVersion() const
{
    return m_version;
}

//------------------------------------------------------------------------------

void Module::loadLibraries()
{
    // Ensure the module is enabled.
    if( m_enabled == false )
    {
        throw RuntimeException( getModuleStr(m_identifier, m_version) + ": module is not enabled." );
    }

    // Pre-condition
    SLM_ASSERT("Module is already loaded", m_loadingModule == nullptr );

    SLM_TRACE( "Loading " + this->getIdentifier() + s_VERSION_DELIMITER + this->getVersion().string() + " library...");

    // References the current module as the loading module.
    m_loadingModule = shared_from_this();

    // Loads all libraries.
    for(const LibraryContainer::value_type& library : m_libraries)
    {
        if(library->isLoaded() == false)
        {
            try
            {
                library->load();
            }
            catch( std::exception& e )
            {
                std::string message;

                message += "Unable to load module ";
                message += library->getPath().string();
                message += ". ";
                message += e.what();

                SLM_ERROR(message);
                m_loadingModule.reset();

                throw RuntimeException( message );
            }
        }
    }

    // Unreferences the current module from the loading module.
    m_loadingModule.reset();

    // Post-condition
    assert( m_loadingModule == 0 );
    SLM_TRACE("Library " + getModuleStr(m_identifier, m_version) + " loaded");
}

//------------------------------------------------------------------------------

void Module::loadRequirements()
{
    try
    {
        Runtime& rntm = Runtime::get();
        RequirementContainer::const_iterator iter;
        for(const RequirementContainer::value_type& requirement : m_requirements)
        {
            auto module = rntm.findModule(requirement);

            // Ensure that a module has been retrieved.
            if( module == nullptr )
            {
                throw RuntimeException( requirement + ": required module not found." );
            }
            // Enable the required module if necessary.
            if( !module->isEnabled() )
            {
                std::dynamic_pointer_cast< detail::Module >(module)->setEnable(true);
            }
            // Starts the module (loads its libraries and requirements module).
            if ( !module->isStarted() )
            {
                module->start();
            }
        }
    }
    catch( const std::exception& e )
    {
        std::string message;

        message += "Module " + getModuleStr(m_identifier, m_version) + " was not able to load requirements. ";
        message += e.what();
        throw RuntimeException( message );
    }
}

//------------------------------------------------------------------------------

void Module::start()
{
    SLM_ASSERT("Module " + getModuleStr(m_identifier, m_version) + " already started.", !m_started );
    if( m_enabled == false )
    {
        throw RuntimeException( getModuleStr(m_identifier, m_version) + ": module is not enabled." );
    }

    if( m_plugin == nullptr )
    {
        loadRequirements();
        loadLibraries();
        try
        {
            startPlugin();
            SLM_TRACE(getModuleStr(m_identifier, m_version) + " Started");
        }
        catch( std::exception& e )
        {
            throw RuntimeException( getModuleStr(m_identifier, m_version) +
                                    ": start plugin error (after load requirement) :" + e.what() );
        }
    }
}

//------------------------------------------------------------------------------

void Module::startPlugin()
{
    SLM_ASSERT("Module " + getModuleStr(m_identifier, m_version) + " plugin is already started.",
               !m_started );
    // Retrieves the type of the plugin.
    const std::string pluginType( getClass() );

    // According to the presence of a class or not, build and empty
    // plugin or attempt to instantiate a user defined plugin.
    SPTR( IPlugin ) plugin;

    if( pluginType.empty() )
    {
        plugin = std::make_shared< EmptyPlugin > ();
    }
    else
    {
        Runtime* rntm( Runtime::getDefault() );
        SPTR( IExecutable ) executable( rntm->createExecutableInstance(pluginType) );

        plugin = std::dynamic_pointer_cast< IPlugin >( executable );
    }

    // Ensures that a plugin has been created.
    if( plugin == 0 )
    {
        throw RuntimeException( getModuleStr(m_identifier, m_version) + ": unable to create a plugin instance." );
    }

    if(::fwRuntime::getCurrentProfile())
    {
        SLM_TRACE("Starting " + getModuleStr(m_identifier, m_version) + " Module's plugin.");
        // Stores and start the plugin.
        try
        {
            auto prof = std::dynamic_pointer_cast< detail::profile::Profile>(::fwRuntime::getCurrentProfile());
            SLM_TRACE("Register stopper for " + getModuleStr(m_identifier, m_version) + " Module's plugin.");
            prof->add(std::make_shared<profile::Stopper>(this->getIdentifier(), this->getVersion()));

            m_plugin = plugin;
            m_plugin->start();

            prof->add(std::make_shared<profile::Initializer>(this->getIdentifier(), this->getVersion()));

            m_started = true;
        }
        catch( std::exception& e )
        {
            throw RuntimeException( getModuleStr(m_identifier, m_version) + ": start plugin error : " + e.what() );
        }
    }
}

//------------------------------------------------------------------------------

void Module::stop()
{
    SLM_ASSERT("Module "+ getModuleStr(m_identifier, m_version) + " not started.", m_started );
    SLM_ASSERT(getModuleStr(m_identifier, m_version) + " : m_plugin not an intance.", m_plugin != nullptr );
    SLM_ASSERT("Module " + getModuleStr(m_identifier, m_version) + " not uninitialized.", !m_initialized );

    SLM_TRACE("Stopping " + getModuleStr(m_identifier, m_version) + " Module's plugin.");
    try
    {
        m_plugin->stop();
        m_started = false;
        SLM_TRACE(getModuleStr(m_identifier, m_version) << " Stopped");
    }
    catch( std::exception& e )
    {
        throw RuntimeException( getModuleStr(m_identifier, m_version) + ": stop plugin error : " + e.what() );
    }

    detail::Runtime& runtime = detail::Runtime::get();
    runtime.unregisterModule(this->shared_from_this());

    //Unloads all libraries.
//    LibraryContainer::iterator curEntry;
//    LibraryContainer::iterator endEntry = m_libraries.end();
//    for(curEntry = m_libraries.begin(); curEntry != endEntry; ++curEntry)
//    {
//        std::shared_ptr<dl::Library> library(*curEntry);
//        if(library->isLoaded() == true )
//        {
//            library->unload();
//        }
//    }
}

//------------------------------------------------------------------------------
void Module::initialize()
{
    SLM_ASSERT("Module '" + getModuleStr(m_identifier, m_version) + "' not started.", m_started );
    SLM_ASSERT("Module '"+ getModuleStr(m_identifier, m_version) + "' already initialized.", !m_initialized );
    try
    {
        m_initialized = true;
        SLM_TRACE("Initializing " + getModuleStr(m_identifier, m_version) + " ...");
        m_plugin->initialize();
        SLM_TRACE("             " + getModuleStr(m_identifier, m_version) + " Initialized");
    }
    catch( std::exception& e )
    {
        throw RuntimeException( getModuleStr(m_identifier, m_version) + ": initialize plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

void Module::uninitialize()
{
    SLM_ASSERT("Module '"+ getModuleStr(m_identifier, m_version) + "' has not been started.",
               m_plugin != nullptr);
    SLM_ASSERT("Module '"+ getModuleStr(m_identifier, m_version) + "' not initialized.", m_initialized );
    try
    {
        SLM_TRACE("Uninitializing " + this->getIdentifier() + " ...");
        m_plugin->uninitialize();
        m_initialized = false;
        SLM_TRACE("               " + this->getIdentifier() + " Uninitialized");
    }
    catch( std::exception& e )
    {
        throw RuntimeException( getModuleStr(m_identifier, m_version) + ": initialize plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

bool Module::isEnabled() const
{
    return m_enabled;
}

//------------------------------------------------------------------------------

void Module::setEnable( const bool state )
{
    m_enabled = state;
}

//------------------------------------------------------------------------------

void Module::addParameter( const std::string& identifier, const std::string& value )
{
    m_parameters[identifier] = value;
}

//------------------------------------------------------------------------------

const std::string Module::getParameterValue( const std::string& identifier ) const
{
    ParameterContainer::const_iterator found = m_parameters.find(identifier);

    return (found != m_parameters.end()) ? found->second : std::string();
}

//------------------------------------------------------------------------------

bool Module::hasParameter( const std::string& identifier ) const
{
    return (m_parameters.find(identifier) != m_parameters.end());
}

//------------------------------------------------------------------------------

fwRuntime::Module::ExtensionContainer fwRuntime::detail::Module::getExtensions() const
{
    return m_extensions;
}

//------------------------------------------------------------------------------

std::string Module::getModuleStr(const std::string& identifier, const fwRuntime::Version& version)
{
    return identifier + s_VERSION_DELIMITER + version.string();
}

//------------------------------------------------------------------------------

void Module::operator= ( const Module& )
{
}

//------------------------------------------------------------------------------

} // namespace detail

} // namespace fwRuntime

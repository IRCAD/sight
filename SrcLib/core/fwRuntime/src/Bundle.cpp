/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/Bundle.hpp"

#include "fwRuntime/EmptyPlugin.hpp"
#include "fwRuntime/ExecutableFactory.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/io/BundleDescriptorReader.hpp"
#include "fwRuntime/profile/Initializer.hpp"
#include "fwRuntime/profile/Profile.hpp"
#include "fwRuntime/profile/Stopper.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/utils/GenericExecutableFactory.hpp"

#include <fwCore/base.hpp>

#include <algorithm>
#include <cassert>
#include <exception>
#include <memory>

namespace fwRuntime
{

//------------------------------------------------------------------------------

SPTR( Bundle ) Bundle::m_loadingBundle;

//------------------------------------------------------------------------------

SPTR( Bundle ) Bundle::getLoadingBundle()
{
    return m_loadingBundle;
}

//------------------------------------------------------------------------------

Bundle::Bundle( const ::boost::filesystem::path& location,
                const std::string& id,
                const std::string& version ) :
    Bundle(location, id, version, "")
{
}

//------------------------------------------------------------------------------

Bundle::Bundle( const ::boost::filesystem::path& location,
                const std::string& id,
                const std::string& version,
                const std::string& c ) :
    m_resourcesLocation( location.lexically_normal() ),
    m_identifier( id ),
    m_version( version ),
    m_class( c )
{
    // Post-condition.
    SLM_ASSERT( "Invalid bundle location.",  m_resourcesLocation.is_complete() == true );

    // Starting from FW4SPL 13.0, the plugin.xml is now likely to be separated from the libraries in the build/install
    std::string strLocation       = location.string();
    const std::string strRCPrefix = BUNDLE_RC_PREFIX;
    const auto itBundle           = strLocation.find(strRCPrefix);
    if(itBundle != std::string::npos)
    {
        strLocation.replace(itBundle, strRCPrefix.length(), std::string(BUNDLE_LIB_PREFIX));
    }
    m_libraryLocation = ::boost::filesystem::path(strLocation);
}

//------------------------------------------------------------------------------

void Bundle::addExecutableFactory( SPTR( ExecutableFactory )factory )
{
    m_executableFactories.insert( factory );
}

//------------------------------------------------------------------------------

Bundle::ExecutableFactoryConstIterator Bundle::executableFactoriesBegin() const
{
    return m_executableFactories.begin();
}

//------------------------------------------------------------------------------

Bundle::ExecutableFactoryConstIterator Bundle::executableFactoriesEnd() const
{
    return m_executableFactories.end();
}

//------------------------------------------------------------------------------

SPTR( ExecutableFactory ) Bundle::findExecutableFactory( const std::string & type ) const
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

void Bundle::addExtension( SPTR( Extension )extension )
{
    m_extensions.insert( extension );
}

//------------------------------------------------------------------------------

bool Bundle::hasExtension(const std::string& identifier) const
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

void Bundle::setEnableExtension(const std::string& identifier, const bool enable)
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

Bundle::ExtensionConstIterator Bundle::extensionsBegin() const
{
    return m_extensions.begin();
}

//------------------------------------------------------------------------------

Bundle::ExtensionConstIterator Bundle::extensionsEnd() const
{
    return m_extensions.end();
}
//------------------------------------------------------------------------------

void Bundle::addExtensionPoint( SPTR( ExtensionPoint )extensionPoint )
{
    m_extensionPoints.insert( extensionPoint );
}

//------------------------------------------------------------------------------

SPTR( ExtensionPoint ) Bundle::findExtensionPoint( const std::string & identifier ) const
{
    std::shared_ptr<ExtensionPoint> resExtensionPoint;
    for(const ExtensionPointContainer::value_type& extensionPoint :  m_extensionPoints)
    {
        if(extensionPoint->getIdentifier() == identifier && extensionPoint->isEnable())
        {
            resExtensionPoint = extensionPoint;
            break;
        }
    }
    return resExtensionPoint;
}

//------------------------------------------------------------------------------

bool Bundle::hasExtensionPoint(const std::string& identifier) const
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

void Bundle::setEnableExtensionPoint(const std::string& identifier, const bool enable)
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

Bundle::ExtensionPointConstIterator Bundle::extensionPointsBegin() const
{
    return m_extensionPoints.begin();
}

//------------------------------------------------------------------------------

Bundle::ExtensionPointConstIterator Bundle::extensionPointsEnd() const
{
    return m_extensionPoints.end();
}

//------------------------------------------------------------------------------

void Bundle::addLibrary( SPTR(dl::Library)library )
{
    library->setBundle(this);
    m_libraries.insert(library);
}

//------------------------------------------------------------------------------

Bundle::LibraryConstIterator Bundle::librariesBegin() const
{
    return m_libraries.begin();
}

//------------------------------------------------------------------------------

Bundle::LibraryConstIterator Bundle::librariesEnd() const
{
    return m_libraries.end();
}

//------------------------------------------------------------------------------

void Bundle::addRequirement(const std::string& requirement)
{
    m_requirements.insert(requirement);
}

//------------------------------------------------------------------------------

const std::string Bundle::getClass() const
{
    return m_class;
}

//------------------------------------------------------------------------------

const std::string& Bundle::getIdentifier() const
{
    return m_identifier;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path& Bundle::getLibraryLocation() const
{
    return m_libraryLocation;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path& Bundle::getResourcesLocation() const
{
    return m_resourcesLocation;
}

//------------------------------------------------------------------------------

SPTR( IPlugin ) Bundle::getPlugin() const
{
    return m_plugin;
}

//------------------------------------------------------------------------------

const Version& Bundle::getVersion() const
{
    return m_version;
}

//------------------------------------------------------------------------------

void Bundle::loadLibraries()
{
    // Ensure the bundle is enabled.
    if( m_enable == false )
    {
        throw RuntimeException( m_identifier + "_" + m_version.string() + ": bundle is not enabled." );
    }

    // Pre-condition
    SLM_ASSERT("Bundle is already loaded", m_loadingBundle == 0 );

    SLM_TRACE( "Loading " + this->getIdentifier() + "_" + this->getVersion().string() + " library...");

    // References the current bundle as the loading bundle.
    m_loadingBundle = shared_from_this();

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
                m_loadingBundle.reset();

                throw RuntimeException( message );
            }
        }
    }

    // Unreferences the current bundle from the loading bundle.
    m_loadingBundle.reset();

    // Post-condition
    assert( m_loadingBundle == 0 );
    SLM_TRACE("Library " + m_identifier + "_" + m_version.string() + " loaded");
}

//------------------------------------------------------------------------------

void Bundle::loadRequirements()
{
    try
    {
        Runtime* rntm(Runtime::getDefault());
        RequirementContainer::const_iterator iter;
        for(const RequirementContainer::value_type& requirement : m_requirements)
        {
            SPTR( Bundle ) bundle( rntm->findEnabledBundle(requirement) );

            // Ensure that a bundle has been retrieved.
            if( bundle == 0 )
            {
                throw RuntimeException( requirement + ": required bundle not found or not enable." );
            }
            // Starts the bundle (loads its libraries and requirements bundle).
            if ( !bundle->isStarted() )
            {
                bundle->start();
            }
        }
    }
    catch( const std::exception& e )
    {
        std::string message;

        message += "Bundle " + m_identifier + "_" + m_version.string() + " was not able to load requirements. ";
        message += e.what();
        throw RuntimeException( message );
    }
}

//------------------------------------------------------------------------------

void Bundle::start()
{
    SLM_ASSERT("Bundle " + m_identifier + "_" + m_version.string() + " already started.",
               !m_started );
    if( m_enable == false )
    {
        throw RuntimeException( m_identifier + "_" + m_version.string() + ": bundle is not enabled." );
    }

    if( m_plugin == 0 )
    {
        loadRequirements();
        loadLibraries();
        try
        {
            startPlugin();
            SLM_TRACE(m_identifier + "_" + m_version.string() + " Started");
        }
        catch( std::exception& e )
        {
            throw RuntimeException( m_identifier + "_" + m_version.string() +
                                    ": start plugin error (after load requirement) :" + e.what() );
        }
    }
}

//------------------------------------------------------------------------------

void Bundle::startPlugin()
{
    SLM_ASSERT("Bundle " + m_identifier + "_" + m_version.string() + " plugin is already started.",
               !m_started );
    // Retrieves the type of the plugin.
    const std::string pluginType( getClass() );

    // According to the presence of a class or not, build and empty
    // plugin or attempt to instantiate a user defined plugin.
    SPTR( IPlugin ) plugin;

    if( pluginType.empty() )
    {
        plugin = SPTR( IPlugin )( new EmptyPlugin() );
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
        throw RuntimeException( m_identifier + "_" + m_version.string() + ": unable to create a plugin instance." );
    }

    SLM_TRACE("Starting " + m_identifier + "_" + m_version.string() + " Bundle's plugin.");
    // Stores and start the plugin.
    try
    {
        SLM_TRACE("Register stopper for " + m_identifier + "_" + m_version.string() + " Bundle's plugin.");
        ::fwRuntime::profile::getCurrentProfile()->add(
            SPTR(profile::Stopper) (new profile::Stopper(this->getIdentifier(), this->getVersion())));
        m_plugin = plugin;
        m_plugin->start();
        ::fwRuntime::profile::getCurrentProfile()->add(
            SPTR(profile::Initializer) (new profile::Initializer(this->getIdentifier(), this->getVersion())) );
        m_started = true;
    }
    catch( std::exception& e )
    {
        throw RuntimeException( m_identifier + "_" + m_version.string() + ": start plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

void Bundle::stop()
{
    SLM_ASSERT("Bundle "+ m_identifier + "_" + m_version.string() + " not started.", m_started );
    SLM_ASSERT(m_identifier + "_" + m_version.string() + " : m_plugin not an intance.", m_plugin != 0 );
    SLM_ASSERT("Bundle " + m_identifier + "_" + m_version.string() + " not uninitialized.", !m_initialized );

    SLM_TRACE("Stopping " + m_identifier + "_" + m_version.string() + " Bundle's plugin.");
    try
    {
        m_plugin->stop();
        m_started = false;
        OSLM_TRACE(m_identifier + "_" + m_version.string() << " Stopped");
    }
    catch( std::exception& e )
    {
        throw RuntimeException( m_identifier + "_" + m_version.string() + ": stop plugin error : " + e.what() );
    }

    ::fwRuntime::Runtime::getDefault()->unregisterBundle(this->shared_from_this());

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
void Bundle::initialize()
{
    SLM_ASSERT("Bundle '" + m_identifier + "_" + m_version.string() + "' not started.", m_started );
    SLM_ASSERT("Bundle '"+ m_identifier + "_" + m_version.string() + "' already initialized.", !m_initialized );
    try
    {
        m_initialized = true;
        SLM_TRACE("Initializing " + m_identifier + "_" + m_version.string() + " ...");
        m_plugin->initialize();
        SLM_TRACE("             " + m_identifier + "_" + m_version.string() + " Initialized");
    }
    catch( std::exception& e )
    {
        throw RuntimeException( m_identifier + "_" + m_version.string() + ": initialize plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

void Bundle::uninitialize()
{
    SLM_ASSERT("Bundle '"+ m_identifier + "_" + m_version.string() + "' has not been started.", m_plugin );
    SLM_ASSERT("Bundle '"+ m_identifier + "_" + m_version.string() + "' not initialized.", m_initialized );
    try
    {
        SLM_TRACE("Uninitializing " + this->getIdentifier() + " ...");
        m_plugin->uninitialize();
        m_initialized = false;
        SLM_TRACE("               " + this->getIdentifier() + " Uninitialized");
    }
    catch( std::exception& e )
    {
        throw RuntimeException( m_identifier + "_" + m_version.string() + ": initialize plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

bool Bundle::isEnable() const
{
    return m_enable;
}

//------------------------------------------------------------------------------

void Bundle::setEnable( const bool state )
{
    m_enable = state;
}

//------------------------------------------------------------------------------

void Bundle::addParameter( const std::string& identifier, const std::string& value )
{
    m_parameters[identifier] = value;
}

//------------------------------------------------------------------------------

const std::string Bundle::getParameterValue( const std::string& identifier ) const
{
    ParameterContainer::const_iterator found = m_parameters.find(identifier);

    return (found != m_parameters.end()) ? found->second : std::string();
}

//------------------------------------------------------------------------------

bool Bundle::hasParameter( const std::string& identifier ) const
{
    return (m_parameters.find(identifier) != m_parameters.end());
}

//------------------------------------------------------------------------------

void Bundle::operator= ( const Bundle& )
{
}

//------------------------------------------------------------------------------

} // namespace fwRuntime

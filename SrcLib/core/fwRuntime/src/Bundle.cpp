/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <algorithm>
#include <cassert>
#include <exception>
#include <memory>

#include <fwCore/base.hpp>

#include "fwRuntime/EmptyPlugin.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/ExecutableFactory.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/predicates.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/io/BundleDescriptorReader.hpp"
#include "fwRuntime/utils/GenericExecutableFactory.hpp"
#include "fwRuntime/profile/Profile.hpp"
#include "fwRuntime/profile/Initializer.hpp"
#include "fwRuntime/profile/Stopper.hpp"

#include "fwRuntime/Bundle.hpp"


namespace fwRuntime
{

//------------------------------------------------------------------------------

namespace
{

    /**
     * @brief   Defines a STL compatible predicate telling if an executable factory
     *          matches the given type identifier
     *
     * @author  IRCAD (Research and Development Team).
     */
    struct IsOfType
    {
        IsOfType( const std::string & type )
        :   m_type( type )
        {}

        const bool operator() ( const SPTR( ExecutableFactory ) factory ) const
        {
            return factory->getType() == m_type;
        }

    private:

        std::string m_type;
    };
}

//------------------------------------------------------------------------------

SPTR( Bundle ) Bundle::m_loadingBundle;

//------------------------------------------------------------------------------

SPTR( Bundle ) Bundle::getLoadingBundle()
{
    return m_loadingBundle;
}

//------------------------------------------------------------------------------

Bundle::Bundle( const boost::filesystem::path   & location,
                const std::string               & id,
                const std::string               & version )
:   m_location    ( location ),
    m_identifier  ( id ),
    m_version     ( version ),
    m_enable      ( false ),
    m_started     ( false ),
    m_initialized ( false )

{
    // Post-condition.
    assert( m_location.is_complete() == true );
}

//------------------------------------------------------------------------------

Bundle::Bundle(
            const boost::filesystem::path   & location,
            const std::string               & id,
            const std::string               & version,
            const std::string               & c )
:   m_location    ( location ),
    m_identifier  ( id ),
    m_version     ( version ),
    m_class       ( c ),
    m_enable      ( false ),
    m_started     ( false ),
    m_initialized ( false )

{
    // Post-condition.
    assert( m_location.is_complete() == true );
}

//------------------------------------------------------------------------------

void Bundle::addExecutableFactory( SPTR( ExecutableFactory ) factory )
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
    ExecutableFactoryConstIterator  found = std::find_if( m_executableFactories.begin(), m_executableFactories.end(), IsOfType(type) );
    return found != m_executableFactories.end() ? *found : SPTR( ExecutableFactory )();
}

//------------------------------------------------------------------------------

void Bundle::addExtension( SPTR( Extension ) extension )
{
    m_extensions.insert( extension );
}

//------------------------------------------------------------------------------

const bool Bundle::hasExtension(const std::string & identifier) const
{
    ExtensionContainer::const_iterator extpt = m_extensions.begin();
    while( extpt != m_extensions.end() )
    {
        if( (*extpt)->getIdentifier() ==  identifier )
        {
            return true;
        }
        ++extpt;
    }

    return false;
}

//------------------------------------------------------------------------------

void Bundle::setEnableExtension(const std::string & identifier, const bool enable)
{
    for( ExtensionContainer::iterator extpt = m_extensions.begin();
            extpt != m_extensions.end();
            ++extpt )
    {

        if( (*extpt)->getIdentifier() ==  identifier )
        {
            (*extpt)->setEnable(enable);
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

void Bundle::addExtensionPoint( SPTR( ExtensionPoint ) extensionPoint )
{
    m_extensionPoints.insert( extensionPoint );
}

//------------------------------------------------------------------------------

SPTR( ExtensionPoint ) Bundle::findExtensionPoint( const std::string & identifier ) const
{
    ExtensionPointContainer::const_iterator found;

    found = std::find_if( m_extensionPoints.begin(), m_extensionPoints.end(), IsEnableAndHasIdentifier<ExtensionPoint>(identifier) );
    return (found != m_extensionPoints.end()) ? (*found) : SPTR(ExtensionPoint)();
}

//------------------------------------------------------------------------------

const bool Bundle::hasExtensionPoint(const std::string & identifier) const
{
    ExtensionPointContainer::const_iterator extpt = m_extensionPoints.begin();
    while( extpt != m_extensionPoints.end() )
    {
        if( (*extpt)->getIdentifier() ==  identifier )
        {
            return true;
        }
        ++extpt;
    }

    return false;
}

//------------------------------------------------------------------------------

void Bundle::setEnableExtensionPoint(const std::string & identifier, const bool enable)
{
    for( ExtensionPointContainer::iterator extpt = m_extensionPoints.begin();
         extpt != m_extensionPoints.end();
         ++extpt )
    {

        if( (*extpt)->getIdentifier() ==  identifier )
        {
            (*extpt)->setEnable(enable);
            break; // The identifier is unique => can break the loop
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

void Bundle::addLibrary( SPTR( dl::Library ) library )
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

void Bundle::addRequirement(const std::string & requirement)
{
    m_requirements.insert(requirement);
}

//------------------------------------------------------------------------------

const std::string Bundle::getClass() const
{
    return m_class;
}

//------------------------------------------------------------------------------

const std::string & Bundle::getIdentifier() const
{
    return m_identifier;
}

//------------------------------------------------------------------------------

const boost::filesystem::path & Bundle::getLocation() const
{
    return m_location;
}

//------------------------------------------------------------------------------

SPTR( IPlugin ) Bundle::getPlugin() const
{
    return m_plugin;
}

//------------------------------------------------------------------------------

const Version & Bundle::getVersion() const
{
    return m_version;
}

//------------------------------------------------------------------------------

void Bundle::loadLibraries() throw(RuntimeException)
{
    // Ensure the bundle is enabled.
    if( m_enable == false )
    {
        throw RuntimeException( m_identifier + ": bundle is not enabled." );
    }

    // Pre-condition
    SLM_ASSERT("Bundle is already loaded", m_loadingBundle == 0 );

    OSLM_TRACE( "Loading " << this->getIdentifier() << " library...");

    // References the current bundle as the loading bundle.
    m_loadingBundle = shared_from_this();

    // Loads all libraries.
    LibraryContainer::iterator curEntry;
    LibraryContainer::iterator endEntry = m_libraries.end();
    for(curEntry = m_libraries.begin(); curEntry != endEntry; ++curEntry)
    {
        SPTR(dl::Library) library(*curEntry);
        if(library->isLoaded() == false)
        {
            try
            {
                library->load();
            }
            catch( std::exception & e )
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

    OSLM_TRACE(this->getIdentifier() << " library loaded");
}

//------------------------------------------------------------------------------

void Bundle::loadRequirements() throw(RuntimeException)
{
    try
    {
        Runtime *rntm(Runtime::getDefault());
        RequirementContainer::const_iterator iter;
        for(iter = m_requirements.begin(); iter != m_requirements.end(); ++iter)
        {
            const std::string requirement ( *iter);
            SPTR( Bundle ) bundle( rntm->findBundle(requirement) );

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
    catch( const std::exception & e )
    {
        std::string message;

        message += "Bundle " + getIdentifier() + " was not able to load requirements. ";
        message += e.what();
        throw RuntimeException( message );
    }
}

//------------------------------------------------------------------------------

void Bundle::start() throw(RuntimeException)
{
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " already started.", !m_started );
    if( m_enable == false )
    {
        throw RuntimeException( m_identifier + ": bundle is not enabled." );
    }

    if( m_plugin == 0 )
    {
        loadRequirements();
        loadLibraries();
        try
        {
            startPlugin();
            OSLM_TRACE(this->getIdentifier() << " Started");
        }
        catch( std::exception & e )
        {
            throw RuntimeException( m_identifier + ": start plugin error (after load requirement) :" + e.what() );
        }
    }
}

//------------------------------------------------------------------------------

void Bundle::startPlugin() throw(RuntimeException)
{
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " plugin is already started.", !m_started );
    // Retrieves the type of the plugin.
    const std::string   pluginType( getClass() );

    // According to the presence of a class or not, build and empty
    // plugin or attempt to instantiate a user defined plugin.
    SPTR( IPlugin ) plugin;

    if( pluginType.empty() )
    {
        plugin = SPTR( IPlugin )( new EmptyPlugin() );
    }
    else
    {
        Runtime * rntm  ( Runtime::getDefault() );
        SPTR( IExecutable ) executable ( rntm->createExecutableInstance(pluginType) );

        plugin = ::boost::dynamic_pointer_cast< IPlugin >( executable );
    }

    // Ensures that a plugin has been created.
    if( plugin == 0 )
    {
        throw RuntimeException( getIdentifier() + ": unable to create a plugin instance." );
    }

    OSLM_TRACE("Starting " << this->getIdentifier() << " Bundle's plugin.");
    // Stores and start the plugin.
    try
    {
        OSLM_TRACE("Register stopper for " << this->getIdentifier() << " Bundle's plugin.");
        ::fwRuntime::profile::getCurrentProfile()->add( SPTR(profile::Stopper) (new profile::Stopper(this->getIdentifier())));
        m_plugin = plugin;
        m_plugin->start();
        ::fwRuntime::profile::getCurrentProfile()->add( SPTR(profile::Initializer) (new profile::Initializer(this->getIdentifier())) );
        m_started = true;
    }
    catch( std::exception & e )
    {
        throw RuntimeException( getIdentifier() + ": start plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

void Bundle::stop() throw(RuntimeException)
{
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " not started.", m_started );
    OSLM_ASSERT(this->getIdentifier() << " : m_plugin not an intance.", m_plugin != 0 );
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " not uninitialized.", !m_initialized );

    OSLM_TRACE("Stopping " << this->getIdentifier() << "Bundle's plugin.");
    try
    {
        m_plugin->stop();
        m_started = false;
        OSLM_TRACE(this->getIdentifier() << " Stopped");
    }
    catch( std::exception & e )
    {
        throw RuntimeException( this->getIdentifier() + ": stop plugin error : " + e.what() );
    }

    ::fwRuntime::Runtime::getDefault()->unregisterBundle(this->shared_from_this());

    //Unloads all libraries.
//    LibraryContainer::iterator curEntry;
//    LibraryContainer::iterator endEntry = m_libraries.end();
//    for(curEntry = m_libraries.begin(); curEntry != endEntry; ++curEntry)
//    {
//        ::boost::shared_ptr<dl::Library> library(*curEntry);
//        if(library->isLoaded() == true )
//        {
//            library->unload();
//        }
//    }
}

//------------------------------------------------------------------------------
void Bundle::initialize() throw(RuntimeException)
{
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " not started.", m_started );
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " already initialized.", !m_initialized );
    try
    {
        m_initialized = true;
        OSLM_TRACE("Initializing " << this->getIdentifier() << " ...");
        m_plugin->initialize();
        OSLM_TRACE("             " <<this->getIdentifier() << " Initialized");
    }
    catch( std::exception & e )
    {
        throw RuntimeException( this->getIdentifier() + ": initialize plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

void Bundle::uninitialize() throw(RuntimeException)
{
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " has not been started.", m_plugin );
    OSLM_ASSERT("Bundle "<< this->getIdentifier() << " not initialized.", m_initialized );
    try
    {
        OSLM_TRACE("Uninitializing " << this->getIdentifier() << " ...");
        m_plugin->uninitialize();
        m_initialized = false;
        OSLM_TRACE("               " << this->getIdentifier() << " Uninitialized");
    }
    catch( std::exception & e )
    {
        throw RuntimeException( this->getIdentifier() + ": initialize plugin error : " + e.what() );
    }
}

//------------------------------------------------------------------------------

const bool Bundle::isEnable() const
{
    return m_enable;
}

//------------------------------------------------------------------------------

void Bundle::setEnable( const bool state )
{
    m_enable = state;
}

//------------------------------------------------------------------------------

void Bundle::addParameter( const std::string & identifier, const std::string & value )
{
    m_parameters[identifier] = value;
}

//------------------------------------------------------------------------------

const std::string Bundle::getParameterValue( const std::string & identifier ) const
{
    ParameterContainer::const_iterator  found = m_parameters.find(identifier);

    return (found != m_parameters.end()) ? found->second : std::string();
}

//------------------------------------------------------------------------------

const bool Bundle::hasParameter( const std::string & identifier ) const
{
    return (m_parameters.find(identifier) != m_parameters.end());
}

//------------------------------------------------------------------------------

void Bundle::operator= ( const Bundle & )
{}

//------------------------------------------------------------------------------

} // namespace fwRuntime

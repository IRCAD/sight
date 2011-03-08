/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/Runtime.hpp"

#include <algorithm>
#include <cassert>
#include <boost/filesystem/operations.hpp>

#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/ExecutableFactory.hpp"
#include "fwRuntime/IPlugin.hpp"
#include "fwRuntime/predicates.hpp"
#include "fwRuntime/io/BundleDescriptorReader.hpp"



namespace fwRuntime
{

namespace
{

    /**
     * @brief   Implements an STL compliant functor that will store given
     *          bundles in the given runtime instance
     */
    struct StoreBundle
    {
        StoreBundle( Runtime * runtime )
        :   m_runtime( runtime )
        {}

        void operator() ( ::boost::shared_ptr< Bundle > bundle ) const
        {
            m_runtime->addBundle( bundle );
        }

    private:

        Runtime * m_runtime;
    };

    /**
     * @brief   Implements an STL compliant functor that will store given
     *          extensions in the given runtime instance
     */
    struct StoreExtension
    {
        StoreExtension( Runtime * runtime )
        :   m_runtime( runtime )
        {}

        void operator() ( ::boost::shared_ptr< Extension > extension ) const
        {
            m_runtime->addExtension( extension );
        }

    private:

        Runtime * m_runtime;
    };

    /**
     * @brief   Implements an STL compliant functor that will store given
     *          extension points in the given runtime instance
     */
    struct StoreExtensionPoint
    {
        StoreExtensionPoint( Runtime * runtime )
        :   m_runtime( runtime )
        {}

        void operator() ( ::boost::shared_ptr< ExtensionPoint > extensionPoint ) const
        {
            m_runtime->addExtensionPoint( extensionPoint );
        }

    private:

        Runtime * m_runtime;
    };

    /**
     * @brief   Implements an STL compliant functor that will store given
     *          executable factory in the given runtime instance
     */
    struct StoreExecutableFactory
    {
        StoreExecutableFactory( Runtime * runtime )
        :   m_runtime( runtime )
        {}

        void operator() ( ::boost::shared_ptr< ExecutableFactory > factory ) const
        {
            m_runtime->addExecutableFactory( factory );
        }

    private:

        Runtime * m_runtime;
    };

    /**
     * @brief   Implements a STL compliant predicate that tests if a given
     *          object has a given type and is enabled.
     *
     * @remark  Intended to be used on executable factories
     */
    template< typename T >
    struct IsEnableAndHasType
    {
        IsEnableAndHasType( const std::string & type )
        :   m_type( type )
        {}

        const bool operator() ( const ::boost::shared_ptr< T > p ) const
        {
            return p->getType() == m_type && p->isEnable();
        }

    private:

        std::string m_type;
    };
}



::boost::shared_ptr<Runtime> Runtime::m_instance;



Runtime::Runtime()
{}



Runtime::~Runtime()
{}



void Runtime::addBundle( ::boost::shared_ptr< Bundle > bundle ) throw(RuntimeException)
{
    m_bundles.insert( bundle );
    std::for_each( bundle->extensionsBegin(), bundle->extensionsEnd(), StoreExtension(this) );
    std::for_each( bundle->extensionPointsBegin(), bundle->extensionPointsEnd(), StoreExtensionPoint(this) );
    std::for_each( bundle->executableFactoriesBegin(), bundle->executableFactoriesEnd(), StoreExecutableFactory(this) );
}



void Runtime::addBundles( const ::boost::filesystem::path & repository ) throw(RuntimeException)
{
    try
    {
        using ::fwRuntime::io::BundleDescriptorReader;

        const BundleDescriptorReader::BundleContainer bundles = BundleDescriptorReader::createBundles( repository );
        std::for_each( bundles.begin(), bundles.end(), StoreBundle(this) );
    }
    catch(const std::exception& exception)
    {
        throw RuntimeException( std::string("Error while adding bundles. ") + exception.what() );
    }
}



Runtime::BundleIterator Runtime::bundlesBegin()
{
    return m_bundles.begin();
}



Runtime::BundleIterator Runtime::bundlesEnd()
{
    return m_bundles.end();
}



void Runtime::addExecutableFactory( ::boost::shared_ptr< ExecutableFactory > factory ) throw(RuntimeException)
{
    // Ensures no registered factory has the same identifier.
    const std::string   type( factory->getType() );
    if( findExecutableFactory(type) != 0 )
    {
        throw RuntimeException(type + ": type already used by an executable factory.");
    }

    // Stores the executable factory.
    m_executableFactories.insert( factory );
}



::boost::shared_ptr< ExecutableFactory > Runtime::findExecutableFactory( const std::string & type ) const
{
    ExecutableFactoryContainer::const_iterator  found;

    found = std::find_if( m_executableFactories.begin(), m_executableFactories.end(), IsEnableAndHasType<ExecutableFactory>(type) );
    return ( found == m_executableFactories.end() ) ? ::boost::shared_ptr< ExecutableFactory >() : *found;
}



void Runtime::addExtension( ::boost::shared_ptr<Extension> extension) throw(RuntimeException)
{
    // Asserts no registered extension has the same identifier.
    std::string identifier(extension->getIdentifier());
    if( identifier.length() != 0 && findExtension(identifier) != 0 )
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension.");
    }

    // Stores the extension.
    m_extensions.insert( extension );
}



Runtime::ExtensionIterator Runtime::extensionsBegin()
{
    return m_extensions.begin();
}



Runtime::ExtensionIterator Runtime::extensionsEnd()
{
    return m_extensions.end();
}



void Runtime::addExtensionPoint( ::boost::shared_ptr<ExtensionPoint> point) throw(RuntimeException)
{
    // Asserts no registered extension point has the same identifier.
    std::string identifier(point->getIdentifier());
    if( findExtensionPoint(identifier) != 0)
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension point.");
    }

    // Stores the extension.
    m_extensionPoints.insert(point);
}



::boost::shared_ptr< Bundle > Runtime::findBundle( const std::string & identifier, const Version & version ) const
{
    BundleContainer::const_iterator it;
    BundleContainer::const_iterator end;
    for(it = m_bundles.begin(), end = m_bundles.end(); it != end; ++it)
    {
         ::boost::shared_ptr<Bundle>    bundle(*it);
        if(bundle->getIdentifier() == identifier && bundle->getVersion() == version)
        {
            break;
        }
    }
    return it != end ? (*it) : ::boost::shared_ptr<Bundle>();
}


Runtime * Runtime::getDefault()
{
    if(m_instance.get() == 0)
    {
        m_instance = ::boost::shared_ptr<Runtime>(new Runtime());
    }
    return m_instance.get();
}


::boost::shared_ptr<Extension> Runtime::findExtension( const std::string & identifier ) const
{
    ExtensionContainer::const_iterator  found;

    found = std::find_if( m_extensions.begin(), m_extensions.end(), IsEnableAndHasIdentifier<Extension>(identifier) );
    return (found != m_extensions.end()) ? (*found) : ::boost::shared_ptr<Extension>();
}



::boost::shared_ptr<ExtensionPoint> Runtime::findExtensionPoint( const std::string & identifier ) const
{
    ExtensionPointContainer::const_iterator found;

    found = std::find_if( m_extensionPoints.begin(), m_extensionPoints.end(), IsEnableAndHasIdentifier<ExtensionPoint>(identifier) );
    return (found != m_extensionPoints.end()) ? (*found) : ::boost::shared_ptr<ExtensionPoint>();
}




IExecutable * Runtime::createExecutableInstance( const std::string & type ) throw( RuntimeException )
{
     ::boost::shared_ptr< ExecutableFactory > factory;

    // Retrieves the executable factory.
    factory = findExecutableFactory( type );
    if( factory == 0 )
    {
        throw RuntimeException( type + ": no executable factory found for that type." );
    }

    // Creates the executable instance
    IExecutable     * result( factory->createExecutable() );

    result->setBundle( factory->getBundle() );

    // Job's done.
    return result;
}



IExecutable * Runtime::createExecutableInstance( const std::string & type, ::boost::shared_ptr< ConfigurationElement > configurationElement ) throw( RuntimeException )
{
     ::boost::shared_ptr< ExecutableFactory > factory;

    // Retrieves the executable factory.
    factory = findExecutableFactory( type );

    // If there is no factory has been found, it is possible that
    // it has not been registered since the bundle of the given configuration element
    // is not started.
    // So we start that bundle and look for the executablefactory one more type.
    if( factory == 0)
    {
        configurationElement->getBundle()->start();
        factory = findExecutableFactory( type );
    }

    // If we still have not found any executable factory, then notify the problem.
    if( factory == 0 )
    {
        throw RuntimeException( type + ": no executable factory found for that type." );
    }

    // Creates the executable instance
    IExecutable     * result( 0 );
    try
    {
        factory->getBundle()->start();
        result = factory->createExecutable();

        result->setBundle( factory->getBundle() );
        result->setInitializationData( configurationElement );
    }
    catch( const std::exception & e )
    {
        std::string message( "Unable to create an executable instance. " );

        throw RuntimeException( message + e.what() );
    }

    // Job's done.
    return result;
}





} // namespace fwRuntime

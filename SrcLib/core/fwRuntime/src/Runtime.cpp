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

#include "fwRuntime/Runtime.hpp"

#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/ExecutableFactory.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/io/BundleDescriptorReader.hpp"
#include "fwRuntime/IPlugin.hpp"

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/operations.hpp>

#include <limits.h>

#include <algorithm>
#include <cassert>
#include <functional>

namespace fwRuntime
{

//------------------------------------------------------------------------------

std::shared_ptr<Runtime> Runtime::m_instance;

//------------------------------------------------------------------------------

Runtime::Runtime()
{
    auto execPath = ::boost::dll::program_location();

    // The program location is 'path/bin/executable', real working path is 'path'
    m_workingPath = execPath.normalize().parent_path().parent_path();
}

//------------------------------------------------------------------------------

Runtime::~Runtime()
{
}

//------------------------------------------------------------------------------

void Runtime::addBundle( std::shared_ptr< Bundle > bundle )
{
    m_bundles.insert( bundle );
    std::for_each( bundle->extensionsBegin(), bundle->extensionsEnd(),
                   std::bind(&Runtime::addExtension, this, std::placeholders::_1));
    std::for_each( bundle->extensionPointsBegin(), bundle->extensionPointsEnd(),
                   std::bind(&Runtime::addExtensionPoint, this, std::placeholders::_1));
    std::for_each( bundle->executableFactoriesBegin(), bundle->executableFactoriesEnd(),
                   std::bind(&Runtime::addExecutableFactory, this, std::placeholders::_1));
}

//------------------------------------------------------------------------------

void Runtime::unregisterBundle( std::shared_ptr< Bundle > bundle )
{
    FwCoreNotUsedMacro(bundle);
}

//------------------------------------------------------------------------------

void Runtime::addBundles( const ::boost::filesystem::path& repository )
{
    try
    {
        using ::fwRuntime::io::BundleDescriptorReader;
        const BundleDescriptorReader::BundleContainer bundles = BundleDescriptorReader::createBundles( repository );
        std::for_each( bundles.begin(), bundles.end(), std::bind(&Runtime::addBundle, this, std::placeholders::_1) );
    }
    catch(const std::exception& exception)
    {
        throw RuntimeException( std::string("Error while adding bundles. ") + exception.what() );
    }
}

//------------------------------------------------------------------------------

void Runtime::addDefaultBundles()
{
    // Bundles location
    const auto location = this->getWorkingPath() / BUNDLE_RC_PREFIX;

    SLM_ASSERT("Default Bundles location not found: " + location.string(), ::boost::filesystem::exists(location));

    // Read bundles
    this->addBundles(location);
    SLM_ASSERT("Couldn't load any bundle from path: " + location.string(), this->bundlesBegin() != this->bundlesEnd());
}

//------------------------------------------------------------------------------

Runtime::BundleIterator Runtime::bundlesBegin()
{
    return m_bundles.begin();
}

//------------------------------------------------------------------------------

Runtime::BundleIterator Runtime::bundlesEnd()
{
    return m_bundles.end();
}

//------------------------------------------------------------------------------

void Runtime::addExecutableFactory( std::shared_ptr< ExecutableFactory > factory )
{
    // Ensures no registered factory has the same identifier.
    const std::string type( factory->getType() );
    if( this->findExecutableFactory(type) != 0 )
    {
        throw RuntimeException(type + ": type already used by an executable factory.");
    }
    // Stores the executable factory.
    m_executableFactories.insert( factory );
}

//------------------------------------------------------------------------------

void Runtime::unregisterExecutableFactory( std::shared_ptr< ExecutableFactory > factory )
{
    // Ensures no registered factory has the same identifier.
    const std::string type( factory->getType() );
    SLM_WARN_IF("ExecutableFactory Type " + type + " not found.", this->findExecutableFactory(type) == 0 );
    // Removes the executable factory.
    m_executableFactories.erase(factory);
}

//------------------------------------------------------------------------------

std::shared_ptr< ExecutableFactory > Runtime::findExecutableFactory( const std::string& type ) const
{
    std::shared_ptr< ExecutableFactory > resFactory;
    for(const ExecutableFactoryContainer::value_type& factory : m_executableFactories)
    {
        if(factory->getType() == type && factory->isEnable())
        {
            resFactory = factory;
            break;
        }
    }
    return resFactory;
}

//------------------------------------------------------------------------------

void Runtime::addExtension( std::shared_ptr<Extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(extension->getIdentifier());
    if( !identifier.empty() && this->findExtension(identifier) != 0 )
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension.");
    }
    // Stores the extension.
    m_extensions.insert( extension );
}

//------------------------------------------------------------------------------

void Runtime::unregisterExtension( std::shared_ptr<Extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(extension->getIdentifier());
    SLM_WARN_IF("Extension " + identifier + " not found.",
                !identifier.empty() && this->findExtension(identifier) == 0 );
    // Removes the extension.
    m_extensions.erase( extension );
}

//------------------------------------------------------------------------------

Runtime::ExtensionIterator Runtime::extensionsBegin()
{
    return m_extensions.begin();
}

//------------------------------------------------------------------------------

Runtime::ExtensionIterator Runtime::extensionsEnd()
{
    return m_extensions.end();
}

//------------------------------------------------------------------------------

void Runtime::addExtensionPoint( std::shared_ptr<ExtensionPoint> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(point->getIdentifier());
    if( this->findExtensionPoint(identifier) != 0)
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension point.");
    }
    // Stores the extension.
    m_extensionPoints.insert(point);
}

//------------------------------------------------------------------------------

void Runtime::unregisterExtensionPoint( std::shared_ptr<ExtensionPoint> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(point->getIdentifier());
    SLM_WARN_IF("ExtensionPoint " + identifier + " not found.",
                this->findExtensionPoint(identifier) == 0);
    // Removes the extension.
    m_extensionPoints.erase(point);
}

//------------------------------------------------------------------------------

std::shared_ptr< Bundle > Runtime::findBundle( const std::string& identifier, const Version& version ) const
{
    std::shared_ptr<Bundle> resBundle;
    for(const std::shared_ptr<Bundle>& bundle :  m_bundles)
    {
        if(bundle->getIdentifier() == identifier && bundle->getVersion() == version)
        {
            resBundle = bundle;
            break;
        }
    }
    return resBundle;
}

//------------------------------------------------------------------------------

std::shared_ptr< Bundle > Runtime::findEnabledBundle( const std::string& identifier, const Version& version ) const
{
    std::shared_ptr<Bundle> resBundle;
    for(const std::shared_ptr<Bundle>& bundle :  m_bundles)
    {
        if(bundle->getIdentifier() == identifier && bundle->getVersion() == version && bundle->isEnable())
        {
            resBundle = bundle;
            break;
        }
    }
    return resBundle;
}

//------------------------------------------------------------------------------

Runtime* Runtime::getDefault()
{
    if(m_instance.get() == 0)
    {
        m_instance = std::shared_ptr<Runtime>(new Runtime());
    }
    return m_instance.get();
}

//------------------------------------------------------------------------------

std::shared_ptr<Extension> Runtime::findExtension( const std::string& identifier ) const
{
    std::shared_ptr<Extension> resExtension;
    for(const ExtensionContainer::value_type& extension :  m_extensions)
    {
        if(extension->getIdentifier() == identifier && extension->isEnable())
        {
            resExtension = extension;
            break;
        }
    }
    return resExtension;
}

//------------------------------------------------------------------------------

std::shared_ptr<ExtensionPoint> Runtime::findExtensionPoint( const std::string& identifier ) const
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

IExecutable* Runtime::createExecutableInstance( const std::string& type )
{
    std::shared_ptr< ExecutableFactory > factory;

    // Retrieves the executable factory.
    factory = this->findExecutableFactory( type );
    if( factory == 0 )
    {
        throw RuntimeException( type + ": no executable factory found for that type." );
    }

    // Creates the executable instance
    IExecutable* result( factory->createExecutable() );
    result->setBundle( factory->getBundle() );

    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

IExecutable* Runtime::createExecutableInstance( const std::string& type,
                                                ConfigurationElement::sptr configurationElement )
{
    std::shared_ptr< ExecutableFactory > factory;

    // Retrieves the executable factory.
    factory = this->findExecutableFactory( type );

    // If there is no factory has been found, it is possible that
    // it has not been registered since the bundle of the given configuration element
    // is not started.
    // So we start that bundle and look for the executable factory one more type.
    if( factory == 0)
    {
        configurationElement->getBundle()->start();
        factory = this->findExecutableFactory( type );
    }

    // If we still have not found any executable factory, then notify the problem.
    if( factory == 0 )
    {
        throw RuntimeException( type + ": no executable factory found for that type." );
    }

    // Creates the executable instance
    IExecutable* result( 0 );
    try
    {
        factory->getBundle()->start();
        result = factory->createExecutable();

        result->setBundle( factory->getBundle() );
        result->setInitializationData( configurationElement );
    }
    catch( const std::exception& e )
    {
        std::string message( "Unable to create an executable instance. " );
        throw RuntimeException( message + e.what() );
    }
    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

void Runtime::setWorkingPath(const ::boost::filesystem::path& workingPath)
{
    m_workingPath = workingPath;
}

//------------------------------------------------------------------------------

::boost::filesystem::path Runtime::getWorkingPath() const
{
    return m_workingPath;
}

//------------------------------------------------------------------------------

} // namespace fwRuntime

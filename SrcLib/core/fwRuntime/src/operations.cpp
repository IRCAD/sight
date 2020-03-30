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

#include "fwRuntime/operations.hpp"

#include "fwCore/spyLog.hpp"

#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/impl/ExtensionPoint.hpp"
#include "fwRuntime/impl/io/ProfileReader.hpp"
#include "fwRuntime/impl/Module.hpp"
#include "fwRuntime/impl/profile/Profile.hpp"
#include "fwRuntime/impl/Runtime.hpp"

#include <algorithm>
#include <vector>

namespace fwRuntime
{

namespace
{

//------------------------------------------------------------------------------

/**
 * @brief   Functor that matches configuration element identifiers
 *          against the given identifier
 *
 *
 */
struct ConfigurationElementIdentifierPredicate
{
    ConfigurationElementIdentifierPredicate( const std::string& identifier ) :
        m_identifier( identifier )
    {
    }

    //------------------------------------------------------------------------------

    bool operator() ( std::shared_ptr< ConfigurationElement > element )
    {
        return element->getAttributeValue("id") == m_identifier;
    }

    private:

        std::string m_identifier;

};

}

//------------------------------------------------------------------------------

void init(const std::filesystem::path& directory)
{
    Runtime& rntm = Runtime::get();
    rntm.setWorkingPath(directory);
    rntm.addDefaultModules();
}

//------------------------------------------------------------------------------

ConfigurationElement::sptr findConfigurationElement( const std::string& identifier,
                                                     const std::string& pointIdentifier )
{
    ConfigurationElement::sptr resultConfig;
    auto elements     = getAllConfigurationElementsForPoint( pointIdentifier );
    auto foundElement = ::std::find_if( elements.begin(), elements.end(),
                                        ConfigurationElementIdentifierPredicate(identifier) );
    if(foundElement != elements.end())
    {
        resultConfig = *foundElement;
    }
    return resultConfig;
}

//------------------------------------------------------------------------------

std::shared_ptr< Extension > findExtension( const std::string& identifier )
{
    ::fwRuntime::Runtime* rntm = ::fwRuntime::Runtime::getDefault();
    return rntm->findExtension( identifier );
}

//------------------------------------------------------------------------------

std::filesystem::path getBundleResourcePath(const std::string& moduleIdentifier) noexcept
{
    FW_DEPRECATED_MSG("getBundleResourcePath", "22.0");
    return getModuleResourcePath(moduleIdentifier);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath(const std::string& moduleIdentifier) noexcept
{
    Runtime* rntm                  = Runtime::getDefault();
    std::shared_ptr<Module> module = rntm->findModule( moduleIdentifier );

    if(module == nullptr)
    {
        SLM_ERROR("Could not find bundle " + moduleIdentifier + "'");
        return std::filesystem::path();
    }
    return module->getResourcesLocation();
}

//------------------------------------------------------------------------------

std::filesystem::path getBundleResourceFilePath(const std::string& bundleIdentifier,
                                                const std::filesystem::path& path) noexcept
{

    FW_DEPRECATED_MSG("getBundleResourceFilePath", "22.0");
    return getModuleResourceFilePath(bundleIdentifier, path);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourceFilePath(const std::string& bundleIdentifier,
                                                const std::filesystem::path& path) noexcept
{
    Runtime* rntm                     = Runtime::getDefault();
    std::shared_ptr<Module>    bundle = rntm->findModule( bundleIdentifier );

    if(bundle == nullptr)
    {
        SLM_ERROR("Could not find bundle '" + bundleIdentifier + "'");
        return std::filesystem::path();
    }
    return getModuleResourcePath(bundle, path);
}

//------------------------------------------------------------------------------

std::filesystem::path getBundleResourceFilePath(const std::filesystem::path& path) noexcept
{
    FW_DEPRECATED_MSG("getBundleResourceFilePath", "22.0");
    return getModuleResourceFilePath(path);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourceFilePath(const std::filesystem::path& path) noexcept
{
    SLM_ASSERT("Path should be relative", path.is_relative());
    const std::string bundleIdentifierAndVersion = path.begin()->string();

    // TEMP_FB: Change _ into - when version refactor is made
    auto itVersionDelimiter = bundleIdentifierAndVersion.find(impl::Module::s_VERSION_DELIMITER);
    auto bundleIdentifier   = bundleIdentifierAndVersion.substr(0, itVersionDelimiter);
    auto bundleVersion      = bundleIdentifierAndVersion.substr(itVersionDelimiter + 1);

    // Strip the bundle name
    std::filesystem::path pathWithoutBundle;
    for(auto itPath = ++path.begin(); itPath != path.end(); itPath++)
    {
        pathWithoutBundle /= *itPath;
    }

    try
    {
        Runtime* rntm = Runtime::getDefault();
        Version version(bundleVersion);
        std::shared_ptr<Module>    bundle = rntm->findModule( bundleIdentifier, version );

        if(bundle == nullptr)
        {
            SLM_ERROR("Could not find bundle '" + bundleIdentifier + "' with version '" + version.string() + "'");
            return std::filesystem::path();
        }
        return getModuleResourcePath(bundle, pathWithoutBundle );
    }
    catch(...)
    {
        SLM_ERROR("Error looking for bundle '" + bundleIdentifier + "' with version '" + bundleVersion + "'");
        return std::filesystem::path();
    }
}

//------------------------------------------------------------------------------

std::filesystem::path getLibraryResourceFilePath(const std::filesystem::path& path) noexcept
{
    // Currently the library resources are at the same location than bundles
    // This might change in the future
    Runtime* rntm = Runtime::getDefault();
    return std::filesystem::weakly_canonical(rntm->getWorkingPath() / BUNDLE_RC_PREFIX / path);
}

//------------------------------------------------------------------------------

std::filesystem::path getResourceFilePath(const std::filesystem::path& path) noexcept
{
    auto file = ::fwRuntime::getModuleResourceFilePath(path);
    if(file.empty())
    {
        // If not found in a bundle, look into libraries
        file = ::fwRuntime::getLibraryResourceFilePath(path);
        SLM_ERROR_IF("Resource '" + path.string() + "' has not been found in any bundle or library", file.empty());
    }
    return file;
}

//------------------------------------------------------------------------------

std::filesystem::path getBundleResourcePath( std::shared_ptr<Module> bundle,
                                             const std::filesystem::path& path) noexcept
{
    FW_DEPRECATED_MSG("getBundleResourcePath", "22.0");
    return getModuleResourcePath(bundle, path);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath( std::shared_ptr<Module> bundle,
                                             const std::filesystem::path& path) noexcept
{
    return bundle->getResourcesLocation() / path;
}

//------------------------------------------------------------------------------

std::filesystem::path getBundleResourcePath( ConfigurationElement::sptr element,
                                             const std::filesystem::path& path) noexcept
{
    FW_DEPRECATED_MSG("getBundleResourcePath", "22.0");
    return getModuleResourcePath(element, path);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath( ConfigurationElement::sptr element,
                                             const std::filesystem::path& path) noexcept
{
    return getModuleResourcePath(element->getModule(), path);
}

//------------------------------------------------------------------------------

std::filesystem::path getBundleResourcePath(const IExecutable* executable,
                                            const std::filesystem::path& path) noexcept
{
    FW_DEPRECATED_MSG("getBundleResourcePath", "22.0");
    return getModuleResourcePath(executable, path);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath(const IExecutable* executable,
                                            const std::filesystem::path& path) noexcept
{
    return getModuleResourcePath(executable->getModule(), path);
}

//------------------------------------------------------------------------------

void addBundles( const std::filesystem::path& directory)
{
    FW_DEPRECATED_MSG("addBundles", "22.0");
    addModules(directory);
}

//------------------------------------------------------------------------------

void addModules( const std::filesystem::path& directory)
{
    Runtime* rntm = Runtime::getDefault();
    SLM_ASSERT("Default runtime not found", rntm);

    rntm->addModules( directory );
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> loadBundle(const std::string& identifier, const Version& version)
{
    FW_DEPRECATED_MSG("loadBundle", "22.0");
    return loadModule(identifier, version);
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> loadModule(const std::string& identifier, const Version& version)
{
    auto bundle = std::dynamic_pointer_cast< impl::Module >(Runtime::get().findModule(identifier, version));

    if(bundle)
    {
        bundle->setEnable(true);
        if(!bundle->isStarted())
        {
            bundle->start();
        }
    }

    return std::move(bundle);
}

//------------------------------------------------------------------------------

::fwRuntime::Profile::sptr startProfile( const std::filesystem::path& path )
{
    try
    {
        ::fwRuntime::Profile::sptr profile = ::fwRuntime::impl::io::ProfileReader::createProfile(path);
        profile->start();
        return profile;
    }
    catch( const std::exception& exception )
    {
        throw RuntimeException( std::string(path.string() + ": invalid profile file. ") + exception.what() );
    }
}

//------------------------------------------------------------------------------

std::shared_ptr< Module > findBundle( const std::string& identifier, const Version& version )
{
    FW_DEPRECATED_MSG("findBundle", "22.0");
    return findModule( identifier, version );
}

//------------------------------------------------------------------------------

std::shared_ptr< Module > findModule( const std::string& identifier, const Version& version )
{
    return Runtime::getDefault()->findModule( identifier, version );
}

//------------------------------------------------------------------------------

std::shared_ptr< impl::ExtensionPoint > findExtensionPoint(const std::string& identifier)
{
    return impl::Runtime::get().findExtensionPoint( identifier );
}

//------------------------------------------------------------------------------

void startBundle(const std::string& identifier)
{
    FW_DEPRECATED_MSG("startBundle", "22.0");
    startModule(identifier);
}

//------------------------------------------------------------------------------

void startModule(const std::string& identifier)
{
    // Retrieves the specified bundle.
    std::shared_ptr<Module> bundle = impl::Runtime::get().findModule( identifier );
    if( bundle == nullptr )
    {
        throw RuntimeException(identifier + ": bundle not found.");
    }
    // Starts the found bundle.
    bundle->start();
}

//------------------------------------------------------------------------------

std::vector<ConfigurationElement::sptr> getAllConfigurationElementsForPoint(const std::string& identifier)
{
    std::vector< ConfigurationElement::sptr > elements;
    std::shared_ptr< impl::ExtensionPoint >  point = findExtensionPoint(identifier);

    OSLM_TRACE("getAllConfigurationElementsForPoint(" << identifier << "Bundle" <<
               point->getModule()->getIdentifier() );

    if( !point )
    {
        throw RuntimeException( identifier + ": invalid extension point identifier." );
    }

    if ( point->isEnable() )
    {
        elements = point->getAllConfigurationElements();
    }
    else
    {
        OSLM_DEBUG( "Ignoring getAllConfigurationElementsForPoint(" << identifier << ") extension point disabled");
    }

    // The job is done!
    return elements;
}

//------------------------------------------------------------------------------

} // namespace fwRuntime

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

#include "core/runtime/operations.hpp"

#include "core/runtime/ConfigurationElement.hpp"
#include "core/runtime/detail/dl/Library.hpp"
#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/io/ProfileReader.hpp"
#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/profile/Profile.hpp"
#include "core/runtime/detail/Runtime.hpp"

#include <core/spyLog.hpp>

#include <algorithm>
#include <vector>

namespace sight::core::runtime
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

//------------------------------------------------------------------------- -----

void init(const std::filesystem::path& directory)
{
    if(!directory.empty())
    {
        FW_DEPRECATED_MSG("Specifying a directory for Sight installation is now deprecated, the path will be ignored",
                          "22.0");
    }

    // Load default modules
    core::runtime::Runtime* rntm = core::runtime::Runtime::getDefault();

    const auto location = rntm->getWorkingPath() / MODULE_RC_PREFIX;
    SIGHT_INFO("Launching Sight runtime in: " + location.string());

    auto profile = std::make_shared<detail::profile::Profile>();
    detail::profile::setCurrentProfile(profile);

    SIGHT_ASSERT("Default Modules location not found: " + location.string(), std::filesystem::exists(location));

    // Read modules
    rntm->addModules(location);
    SIGHT_ASSERT("Couldn't load any module from path: " + location.string(), !rntm->getModules().empty());
}

//------------------------------------------------------------------------------

ConfigurationElement::sptr findConfigurationElement( const std::string& identifier,
                                                     const std::string& pointIdentifier )
{
    ConfigurationElement::sptr resultConfig;
    const auto elements     = getAllConfigurationElementsForPoint( pointIdentifier );
    const auto foundElement = ::std::find_if( elements.begin(), elements.end(),
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
    core::runtime::Runtime* rntm = core::runtime::Runtime::getDefault();
    return rntm->findExtension( identifier );
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath(const std::string& moduleIdentifier) noexcept
{
    Runtime* rntm                  = Runtime::getDefault();
    std::shared_ptr<Module> module = rntm->findModule( moduleIdentifier );

    if(module == nullptr)
    {
        SIGHT_ERROR("Could not find module " + moduleIdentifier + "'");
        return std::filesystem::path();
    }
    return module->getResourcesLocation();
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourceFilePath(const std::string& moduleIdentifier,
                                                const std::filesystem::path& path) noexcept
{
    Runtime* rntm                     = Runtime::getDefault();
    std::shared_ptr<Module>    module = rntm->findModule( moduleIdentifier );

    if(module == nullptr)
    {
        SIGHT_ERROR("Could not find module '" + moduleIdentifier + "'");
        return std::filesystem::path();
    }
    return getModuleResourcePath(module, path);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourceFilePath(const std::filesystem::path& path) noexcept
{
    SIGHT_ASSERT("Path should not be empty", !path.empty());
    SIGHT_ASSERT("Path should be relative", path.is_relative());

    const std::string moduleFolder = path.begin()->string();

    // Strip the module name
    std::filesystem::path pathWithoutModule;
    for(auto itPath = ++path.begin(); itPath != path.end(); itPath++)
    {
        pathWithoutModule /= *itPath;
    }

    try
    {
        Runtime* rntm                  = Runtime::getDefault();
        std::shared_ptr<Module> module = rntm->findModule( moduleFolder );

        if(module == nullptr)
        {
            SIGHT_ERROR("Could not find module '" + moduleFolder + "'");
            return std::filesystem::path();
        }
        return getModuleResourcePath(module, pathWithoutModule );
    }
    catch(...)
    {
        SIGHT_ERROR("Error looking for module '" + moduleFolder + "'");
        return std::filesystem::path();
    }
}

//------------------------------------------------------------------------------

std::filesystem::path getLibraryResourceFilePath(const std::filesystem::path& path) noexcept
{
    // Currently the library resources are at the same location than modules
    // This might change in the future
    Runtime* rntm = Runtime::getDefault();
    return std::filesystem::weakly_canonical(rntm->getWorkingPath() / MODULE_RC_PREFIX / path);
}

//------------------------------------------------------------------------------

std::filesystem::path getResourceFilePath(const std::filesystem::path& path) noexcept
{
    auto file = core::runtime::getModuleResourceFilePath(path);
    if(file.empty())
    {
        // If not found in a module, look into libraries
        file = core::runtime::getLibraryResourceFilePath(path);
        SIGHT_ERROR_IF("Resource '" + path.string() + "' has not been found in any module or library", file.empty());
    }
    return file;
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath( std::shared_ptr<Module> module,
                                             const std::filesystem::path& path) noexcept
{
    return module->getResourcesLocation() / path;
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath( ConfigurationElement::sptr element,
                                             const std::filesystem::path& path) noexcept
{
    return getModuleResourcePath(element->getModule(), path);
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath(const IExecutable* executable,
                                            const std::filesystem::path& path) noexcept
{
    return getModuleResourcePath(executable->getModule(), path);
}

//------------------------------------------------------------------------------

void addModules( const std::filesystem::path& directory)
{
    SIGHT_INFO("Loading modules from: " + directory.string());

    Runtime& rntm = Runtime::get();
    rntm.addModules( directory );
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> loadModule(const std::string& identifier)
{
    auto module = std::dynamic_pointer_cast< detail::Module >(Runtime::get().findModule(identifier));

    if(module)
    {
        module->setEnable(true);
        if(!module->isStarted())
        {
            module->start();
        }
    }

    return module;
}

//------------------------------------------------------------------------------

bool loadLibrary(const std::string& identifier)
{
    static std::map<std::string, std::shared_ptr<detail::dl::Library> > s_LIBRARIES;

    // Even if dlopen does not actually load twice the same library, we avoid this
    if( s_LIBRARIES.find(identifier) != std::end(s_LIBRARIES) )
    {
        return true;
    }

    auto library                       = std::make_shared<detail::dl::Library>(identifier);
    const core::runtime::Runtime& rntm = core::runtime::Runtime::get();

    // Try to load from all known paths
    const auto repositories = rntm.getRepositoriesPath();
    for(auto repo : repositories)
    {
        library->setSearchPath(repo);
        try
        {
            library->load();
            s_LIBRARIES[identifier] = library;
            return true;
        }
        catch (const RuntimeException& e)
        {
            // Fail silently and potentially try in the next repository
        }

    }
    SIGHT_ERROR("Could not load library '" + identifier);

    return false;
}

//------------------------------------------------------------------------------

core::runtime::Profile::sptr startProfile( const std::filesystem::path& path )
{
    try
    {
        core::runtime::Profile::sptr profile = core::runtime::detail::io::ProfileReader::createProfile(path);
        profile->start();
        return profile;
    }
    catch( const std::exception& exception )
    {
        throw RuntimeException( std::string(path.string() + ": invalid profile file. ") + exception.what() );
    }
}

//------------------------------------------------------------------------------

std::shared_ptr< Module > findModule( const std::string& identifier )
{
    return Runtime::getDefault()->findModule( identifier );
}

//------------------------------------------------------------------------------

std::shared_ptr< detail::ExtensionPoint > findExtensionPoint(const std::string& identifier)
{
    return detail::Runtime::get().findExtensionPoint( identifier );
}

//------------------------------------------------------------------------------

void startModule(const std::string& identifier)
{
    // Retrieves the specified module.
    std::shared_ptr<Module> module = detail::Runtime::get().findModule( identifier );
    if( module == nullptr )
    {
        throw RuntimeException(identifier + ": module not found.");
    }
    // Starts the found module.
    module->start();
}

//------------------------------------------------------------------------------

std::vector<ConfigurationElement::sptr> getAllConfigurationElementsForPoint(const std::string& identifier)
{
    std::vector< ConfigurationElement::sptr > elements;
    std::shared_ptr< detail::ExtensionPoint >  point = findExtensionPoint(identifier);

    if( !point )
    {
        throw RuntimeException( identifier + ": invalid extension point identifier." );
    }

    if ( point->isEnabled() )
    {
        elements = point->getAllConfigurationElements();
    }
    else
    {
        SIGHT_DEBUG( "Ignoring getAllConfigurationElementsForPoint(" << identifier << ") extension point disabled");
    }

    // The job is done!
    return elements;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime

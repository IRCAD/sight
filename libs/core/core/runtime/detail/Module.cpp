/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/runtime/detail/Module.hpp"

#include "core/runtime/detail/EmptyPlugin.hpp"
#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/profile/Profile.hpp"
#include "core/runtime/detail/Runtime.hpp"
#include "core/runtime/ExecutableFactory.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/IExecutable.hpp"
#include "core/runtime/utils/GenericExecutableFactory.hpp"

#include <core/base.hpp>

#include <algorithm>
#include <cassert>
#include <exception>
#include <memory>

namespace sight::core::runtime::detail
{

//------------------------------------------------------------------------------

SPTR(Module) Module::m_loadingModule;

//------------------------------------------------------------------------------

SPTR(Module) Module::getLoadingModule()
{
    return m_loadingModule;
}

//------------------------------------------------------------------------------

Module::Module(
    const std::filesystem::path& location,
    std::string id,
    std::string c,
    int priority
) :
    m_resourcesLocation(location.lexically_normal()),
    m_identifier(std::move(id)),
    m_class(std::move(c)),
    m_priority(priority)
{
    // Post-condition.
    SIGHT_ASSERT("Invalid module location.", m_resourcesLocation.is_absolute() == true);

    const auto locationNormalized = std::filesystem::weakly_canonical(location);
    const auto strLocation        = std::regex_replace(
        locationNormalized.string(),
        Runtime::s_MATCH_MODULE_PATH,
        MODULE_LIB_PREFIX
    );

    // This may fail if the module does not contain any library, so we ignore the returned error
    m_libraryLocation = std::filesystem::weakly_canonical(std::filesystem::path(strLocation));
}

//------------------------------------------------------------------------------

void Module::addExecutableFactory(SPTR(ExecutableFactory)factory)
{
    m_executableFactories.insert(factory);
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

SPTR(ExecutableFactory) Module::findExecutableFactory(const std::string& type) const
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

void Module::addExtension(SPTR(Extension)extension)
{
    m_extensions.insert(extension);
}

//------------------------------------------------------------------------------

bool Module::hasExtension(const std::string& identifier) const
{
    bool hasExtension = false;
    for(const auto& extension : m_extensions)
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
    for(const auto& extension : m_extensions)
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

void Module::addExtensionPoint(SPTR(ExtensionPoint)extensionPoint)
{
    m_extensionPoints.insert(extensionPoint);
}

//------------------------------------------------------------------------------

SPTR(ExtensionPoint) Module::findExtensionPoint(const std::string& identifier) const
{
    std::shared_ptr<ExtensionPoint> resExtensionPoint;
    for(const ExtensionPointContainer::value_type& extensionPoint : m_extensionPoints)
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
    for(const ExtensionPointContainer::value_type& extensionPoint : m_extensionPoints)
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
    for(const ExtensionPointContainer::value_type& extensionPoint : m_extensionPoints)
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

void Module::setLibrary(SPTR(dl::Library)library)
{
    library->setSearchPath(this->getLibraryLocation());
    m_library = library;
}

//------------------------------------------------------------------------------

void Module::addRequirement(const std::string& requirement)
{
    m_requirements.insert(requirement);
}

//------------------------------------------------------------------------------

std::string Module::getClass() const
{
    return m_class;
}

//------------------------------------------------------------------------------

const std::string& Module::getIdentifier() const
{
    return m_identifier;
}

//------------------------------------------------------------------------------

std::string Module::getLibraryName() const
{
    return m_library ? m_library->getName().string() : "";
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

SPTR(IPlugin) Module::getPlugin() const
{
    return m_plugin;
}

//------------------------------------------------------------------------------

void Module::loadLibraries()
{
    // Ensure the module is enabled.
    if(!m_enabled)
    {
        throw RuntimeException(getModuleStr(m_identifier) + ": module is not enabled.");
    }

    // Pre-condition
    SIGHT_ASSERT("Module is already loaded", m_loadingModule == nullptr);

    // References the current module as the loading module.
    m_loadingModule = shared_from_this();

    // Loads the library
    if(m_library && !m_library->isLoaded())
    {
        try
        {
            m_library->load();
        }
        catch(std::exception& e)
        {
            std::string message;

            message += "Unable to load module ";
            message += m_library->getName().string();
            message += ". ";
            message += e.what();

            SIGHT_ERROR(message);
            m_loadingModule.reset();

            throw RuntimeException(message);
        }
    }

    // Unreferences the current module from the loading module.
    m_loadingModule.reset();

    // Post-condition
    assert(m_loadingModule == nullptr);
}

//------------------------------------------------------------------------------

void Module::loadRequirements()
{
    try
    {
        Runtime& runtime = Runtime::get();
        RequirementContainer::const_iterator iter;
        for(const RequirementContainer::value_type& requirement : m_requirements)
        {
            auto module = std::dynamic_pointer_cast<detail::Module>(runtime.findModule(requirement));

            // Ensure that a module has been retrieved.
            if(module == nullptr)
            {
                throw RuntimeException(requirement + ": required module not found.");
            }

            // Enable the required module if necessary.
            if(!module->isEnabled())
            {
                module->setEnable(true);
            }

            // Starts the module if necessary (loads its libraries and requirements module).
            module->start();
        }
    }
    catch(const std::exception& e)
    {
        std::string message;

        message += "Module " + getModuleStr(m_identifier) + " was not able to load requirements. ";
        message += e.what();
        throw RuntimeException(message);
    }
}

//------------------------------------------------------------------------------

void Module::start()
{
    if(!m_started)
    {
        if(!m_enabled)
        {
            throw RuntimeException(getModuleStr(m_identifier) + ": module is not enabled.");
        }

        if(m_plugin == nullptr)
        {
            loadRequirements();
            loadLibraries();
            try
            {
                startPlugin();
            }
            catch(std::exception& e)
            {
                throw RuntimeException(
                          getModuleStr(m_identifier)
                          + ": start plugin error (after load requirement) " + e.what()
                );
            }
            SIGHT_INFO("Loaded module '" + m_identifier + "' successfully");
        }
    }
}

//------------------------------------------------------------------------------

void Module::startPlugin()
{
    SIGHT_ASSERT(
        "Module " + getModuleStr(m_identifier) + " plugin is already started.",
        !m_started
    );

    // Retrieves the type of the plugin.
    std::string pluginType(getClass());

    pluginType = std::regex_replace(pluginType, std::regex("_"), "::");

    // According to the presence of a class or not, build and empty
    // plugin or attempt to instantiate a user defined plugin.
    SPTR(IPlugin) plugin;

    if(pluginType.empty())
    {
        plugin = std::make_shared<EmptyPlugin>();
    }
    else
    {
        Runtime* runtime(Runtime::getDefault());
        SPTR(IExecutable) executable(runtime->createExecutableInstance(pluginType));

        plugin = std::dynamic_pointer_cast<IPlugin>(executable);
    }

    // Ensures that a plugin has been created.
    if(plugin == nullptr)
    {
        throw RuntimeException(getModuleStr(m_identifier) + ": unable to create a plugin instance.");
    }

    if(core::runtime::getCurrentProfile())
    {
        // Stores and start the plugin.
        try
        {
            auto prof = std::dynamic_pointer_cast<detail::profile::Profile>(core::runtime::getCurrentProfile());
            prof->addStopper(this->getIdentifier(), this->priority());

            m_plugin = plugin;
            m_plugin->start();

            m_started = true;
        }
        catch(std::exception& e)
        {
            throw RuntimeException(getModuleStr(m_identifier) + ": start plugin error : " + e.what());
        }
    }
}

//------------------------------------------------------------------------------

void Module::stop()
{
    if(m_started)
    {
        SIGHT_ASSERT(getModuleStr(m_identifier) + " : m_plugin not an instance.", m_plugin != nullptr);

        try
        {
            m_plugin->stop();
            m_started = false;
        }
        catch(std::exception& e)
        {
            throw RuntimeException(getModuleStr(m_identifier) + ": stop plugin error : " + e.what());
        }

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
}

//------------------------------------------------------------------------------

bool Module::isEnabled() const
{
    return m_enabled;
}

//------------------------------------------------------------------------------

void Module::setEnable(const bool state)
{
    m_enabled = state;
}

//------------------------------------------------------------------------------

void Module::addParameter(const std::string& identifier, const std::string& value)
{
    m_parameters[identifier] = value;
}

//------------------------------------------------------------------------------

std::string Module::getParameterValue(const std::string& identifier) const
{
    auto found = m_parameters.find(identifier);

    return (found != m_parameters.end()) ? found->second : std::string();
}

//------------------------------------------------------------------------------

bool Module::hasParameter(const std::string& identifier) const
{
    return m_parameters.find(identifier) != m_parameters.end();
}

//------------------------------------------------------------------------------

core::runtime::Module::ExtensionContainer core::runtime::detail::Module::getExtensions() const
{
    core::runtime::Module::ExtensionContainer container;
    std::ranges::transform(m_extensions, std::inserter(container, container.begin()), [](const auto& e){return e;});
    return container;
}

//------------------------------------------------------------------------------

std::string Module::getModuleStr(const std::string& identifier)
{
    return identifier;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail

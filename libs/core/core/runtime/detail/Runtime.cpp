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

#include "core/runtime/detail/Runtime.hpp"

#include "core/runtime/ConfigurationElement.hpp"
#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/io/ModuleDescriptorReader.hpp"
#include "core/runtime/detail/Module.hpp"
#include "core/runtime/ExecutableFactory.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/IExecutable.hpp"
#include "core/runtime/IPlugin.hpp"
#include "core/runtime/Profile.hpp"
#include "core/runtime/runtime.hpp"

#include <core/tools/Os.hpp>

#include <boost/dll.hpp>

#include <filesystem>
#include <functional>

namespace sight::core::runtime::detail
{

#ifdef WIN32
const std::regex Runtime::s_MATCH_MODULE_PATH("share(?!.*share)\\\\.*");
#else
const std::regex Runtime::s_MATCH_MODULE_PATH("share(?!.*share)/.*");
#endif

//------------------------------------------------------------------------------

std::shared_ptr<Runtime> Runtime::m_instance;

//------------------------------------------------------------------------------

Runtime::Runtime()
{
    // Here we try to find the location of the root of a Sight install

    // In most cases, we can rely on finding sight_core library and then go upward in the filesystem tree
    // The lib location looks like 'SIGHT_DIR/lib/<arch>/libsight_core.*', where arch is optional
    const std::string corePath = boost::dll::this_line_location().generic_string();
    const std::string libPrefix("/" MODULE_LIB_PREFIX "/");
    auto it = std::search(corePath.rbegin(), corePath.rend(), libPrefix.rbegin(), libPrefix.rend());

    if(it == corePath.rend())
    {
        // But if we link statically, for instance linking with sight_core as an object library, then
        // boost::dll::this_line_location() will return the location of the current executable
        // In this case, we know that have to locate the bin directory instead of the library directory
        const std::string binPrefix("/" MODULE_BIN_PREFIX "/");
        it = std::search(corePath.rbegin(), corePath.rend(), binPrefix.rbegin(), binPrefix.rend());
        SIGHT_FATAL_IF("Failed to locate Sight runtime. We tried to guess it from: " + corePath, it == corePath.rend());
    }

    const auto dist = std::distance(it, corePath.rend());
    const std::filesystem::path libPath(corePath.begin(), corePath.begin() + dist - 1);
    m_workingPath = libPath.parent_path().make_preferred();
    SIGHT_INFO("Located Sight runtime in folder: " + m_workingPath.string());
}

//------------------------------------------------------------------------------

Runtime::~Runtime()
= default;

//------------------------------------------------------------------------------

void Runtime::addModule(std::shared_ptr<Module> module)
{
    SIGHT_DEBUG("Module " + module->getIdentifier() + " added.")
    m_modules.insert(module);
    std::for_each(
        module->extensionsBegin(),
        module->extensionsEnd(),
        [this](auto e){addExtension(e);});
    std::for_each(
        module->extensionPointsBegin(),
        module->extensionPointsEnd(),
        [this](auto&& PH1, auto&& ...){addExtensionPoint(std::forward<decltype(PH1)>(PH1));});
    std::for_each(
        module->executableFactoriesBegin(),
        module->executableFactoriesEnd(),
        [this](auto&& PH1, auto&& ...){addExecutableFactory(std::forward<decltype(PH1)>(PH1));});
}

//------------------------------------------------------------------------------

void Runtime::unregisterModule(std::shared_ptr<Module> module)
{
    std::for_each(
        module->executableFactoriesBegin(),
        module->executableFactoriesEnd(),
        [this](auto&& PH1, auto&& ...){unregisterExecutableFactory(std::forward<decltype(PH1)>(PH1));});
    std::for_each(
        module->extensionPointsBegin(),
        module->extensionPointsEnd(),
        [this](auto&& PH1, auto&& ...){unregisterExtensionPoint(std::forward<decltype(PH1)>(PH1));});
    std::for_each(
        module->extensionsBegin(),
        module->extensionsEnd(),
        [this](auto&& PH1, auto&& ...){unregisterExtension(std::forward<decltype(PH1)>(PH1));});
    m_modules.erase(module);
}

//------------------------------------------------------------------------------

void Runtime::addModules(const std::filesystem::path& repository)
{
    for(const auto& repo : m_repositories)
    {
        if(repo.second == repository)
        {
            // Avoid adding modules several times, but we don't consider this as an error for the sake of simplicity...
            return;
        }
    }

    try
    {
        const auto modules = core::runtime::detail::io::ModuleDescriptorReader::createModules(repository);
        std::for_each(
            modules.begin(),
            modules.end(),
            [this](auto&& PH1, auto&& ...)
            {
                addModule(std::forward<decltype(PH1)>(PH1));
            });
        const auto libRepoStr = std::regex_replace(
            repository.lexically_normal().string(),
            s_MATCH_MODULE_PATH,
            MODULE_LIB_PREFIX
        );
        m_repositories.emplace_back(
            std::filesystem::weakly_canonical(std::filesystem::path(libRepoStr)),
            repository

        );
    }
    catch(const std::exception& exception)
    {
        throw RuntimeException(std::string("Error while adding modules. ") + exception.what());
    }
}

//------------------------------------------------------------------------------

void Runtime::addExecutableFactory(std::shared_ptr<ExecutableFactory> factory)
{
    // Ensures no registered factory has the same identifier
    const std::string type(factory->getType());
    if(this->findExecutableFactory(type) != nullptr)
    {
        throw RuntimeException(type + ": type already used by an executable factory.");
    }

    // Stores the executable factory.
    m_executableFactories.insert(factory);
}

//------------------------------------------------------------------------------

void Runtime::unregisterExecutableFactory(std::shared_ptr<ExecutableFactory> factory)
{
    // Ensures no registered factory has the same identifier.
    const std::string type(factory->getType());
    SIGHT_WARN_IF("ExecutableFactory Type " + type + " not found.", this->findExecutableFactory(type) == nullptr);

    // Removes the executable factory.
    m_executableFactories.erase(factory);
}

//------------------------------------------------------------------------------

std::shared_ptr<ExecutableFactory> Runtime::findExecutableFactory(const std::string& _type) const
{
    const std::string type = filterID(_type);
    std::shared_ptr<ExecutableFactory> resFactory;
    for(const ExecutableFactoryContainer::value_type& factory : m_executableFactories)
    {
        if(factory->getType() == type && factory->isEnabled())
        {
            resFactory = factory;
            break;
        }
    }

    return resFactory;
}

//------------------------------------------------------------------------------

void Runtime::addExtension(std::shared_ptr<detail::Extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(filterID(extension->getIdentifier()));
    if(!identifier.empty() && this->findExtension(identifier) != nullptr)
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension.");
    }

    // Stores the extension.
    m_extensions.insert(extension);
}

//------------------------------------------------------------------------------

void Runtime::unregisterExtension(std::shared_ptr<detail::Extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(filterID(extension->getIdentifier()));
    SIGHT_WARN_IF(
        "Extension " + identifier + " not found.",
        !identifier.empty() && this->findExtension(identifier) == nullptr
    );

    // Removes the extension.
    m_extensions.erase(extension);
}

//------------------------------------------------------------------------------

Runtime::ExtensionContainer Runtime::getExtensions() const
{
    return m_extensions;
}

//------------------------------------------------------------------------------

Runtime::ExtensionIterator Runtime::extensionsBegin() const
{
    return m_extensions.begin();
}

//------------------------------------------------------------------------------

Runtime::ExtensionIterator Runtime::extensionsEnd() const
{
    return m_extensions.end();
}

//------------------------------------------------------------------------------

void Runtime::addExtensionPoint(std::shared_ptr<ExtensionPoint> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(filterID(point->getIdentifier()));
    if(this->findExtensionPoint(identifier) != nullptr)
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension point.");
    }

    // Stores the extension.
    m_extensionPoints.insert(point);
}

//------------------------------------------------------------------------------

void Runtime::unregisterExtensionPoint(std::shared_ptr<ExtensionPoint> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(filterID(point->getIdentifier()));
    SIGHT_WARN_IF("ExtensionPoint " + identifier + " not found.", this->findExtensionPoint(identifier) == nullptr);

    m_extensionPoints.erase(point);
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::Module> Runtime::findModule(const std::string& identifier) const
{
    SIGHT_ASSERT("Module identifier should not be empty", !identifier.empty());

    const std::string id = filterID(identifier);

    std::shared_ptr<core::runtime::Module> resModule;
    for(const auto& module : m_modules)
    {
        if(module->getIdentifier() == id)
        {
            resModule = module;
            break;
        }
    }

    return resModule;
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::Module> Runtime::findEnabledModule(const std::string& identifier) const
{
    SIGHT_ASSERT("Module identifier should not be empty", !identifier.empty());

    const std::string id = filterID(identifier);

    std::shared_ptr<core::runtime::Module> resModule;
    for(const auto& module : m_modules)
    {
        if(module->getIdentifier() == id && module->isEnabled())
        {
            resModule = module;
            break;
        }
    }

    return resModule;
}

//------------------------------------------------------------------------------

Runtime& Runtime::get()
{
    if(m_instance == nullptr)
    {
        m_instance = std::make_shared<Runtime>();
    }

    auto* runtime = m_instance.get();
    return *runtime;
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::Extension> Runtime::findExtension(const std::string& identifier) const
{
    const std::string id = filterID(identifier);
    std::shared_ptr<core::runtime::Extension> resExtension;
    for(const ExtensionContainer::value_type& extension : m_extensions)
    {
        if(extension->getIdentifier() == id && extension->isEnabled())
        {
            resExtension = extension;
            break;
        }
    }

    return resExtension;
}

//------------------------------------------------------------------------------

core::runtime::detail::Runtime::ModuleContainer Runtime::getModules() const
{
    ModuleContainer modules;
    std::copy(m_modules.begin(), m_modules.end(), std::inserter(modules, modules.begin()));
    return modules;
}

//------------------------------------------------------------------------------

std::filesystem::path Runtime::getWorkingPath() const
{
    return m_workingPath;
}

//------------------------------------------------------------------------------

std::vector<std::pair<std::filesystem::path, std::filesystem::path> > Runtime::getRepositoriesPath() const
{
    return m_repositories;
}

//------------------------------------------------------------------------------

std::shared_ptr<ExtensionPoint> Runtime::findExtensionPoint(const std::string& identifier) const
{
    const std::string id = filterID(identifier);
    std::shared_ptr<ExtensionPoint> resExtensionPoint;
    for(const ExtensionPointContainer::value_type& extensionPoint : m_extensionPoints)
    {
        if(extensionPoint->getIdentifier() == id && extensionPoint->isEnabled())
        {
            resExtensionPoint = extensionPoint;
            break;
        }
    }

    return resExtensionPoint;
}

//------------------------------------------------------------------------------

IExecutable* Runtime::createExecutableInstance(const std::string& type) const
{
    std::shared_ptr<ExecutableFactory> factory;

    // Retrieves the executable factory.
    factory = this->findExecutableFactory(type);
    if(factory == nullptr)
    {
        throw RuntimeException(type + ": no executable factory found for that type.");
    }

    // Creates the executable instance
    IExecutable* result(factory->createExecutable());

    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

IExecutable* Runtime::createExecutableInstance(
    const std::string& type,
    ConfigurationElement::sptr configurationElement
) const
{
    std::shared_ptr<ExecutableFactory> factory;

    // Retrieves the executable factory.
    factory = this->findExecutableFactory(type);

    // If there is no factory has been found, it is possible that
    // it has not been registered since the module of the given configuration element
    // is not started.
    // So we start that module and look for the executable factory one more type.
    if(factory == nullptr)
    {
        configurationElement->getModule()->start();
        factory = this->findExecutableFactory(type);
    }

    // If we still have not found any executable factory, then notify the problem.
    if(factory == nullptr)
    {
        throw RuntimeException(type + ": no executable factory found for that type.");
    }

    // Creates the executable instance
    IExecutable* result(nullptr);
    try
    {
        factory->getModule()->start();
        result = factory->createExecutable();
        result->setInitializationData(configurationElement);
    }
    catch(const std::exception& e)
    {
        std::string message("Unable to create an executable instance. ");
        throw RuntimeException(message + e.what());
    }

    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail

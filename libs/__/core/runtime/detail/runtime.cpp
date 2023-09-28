/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/runtime/detail/runtime.hpp"
#include "core/runtime/runtime.hpp"

#include "core/runtime/detail/extension_point.hpp"
#include "core/runtime/detail/io/module_descriptor_reader.hpp"
#include "core/runtime/detail/module.hpp"
#include "core/runtime/executable.hpp"
#include "core/runtime/executable_factory.hpp"
#include "core/runtime/extension.hpp"
#include "core/runtime/plugin.hpp"
#include "core/runtime/profile.hpp"

#include <core/tools/os.hpp>

#include <boost/dll.hpp>

#include <filesystem>
#include <functional>

namespace sight::core::runtime::detail
{

#ifdef WIN32
const std::regex runtime::s_match_module_path("share(?!.*share)\\\\.*");
#else
const std::regex runtime::s_match_module_path("share(?!.*share)/.*");
#endif

//------------------------------------------------------------------------------

std::shared_ptr<runtime> runtime::m_instance;

//------------------------------------------------------------------------------

runtime::runtime()
{
    // Here we try to find the location of the root of a Sight install

    // In most cases, we can rely on finding sight_core library and then go upward in the filesystem tree
    // The lib location looks like 'SIGHT_DIR/lib/<arch>/libsight_core.*', where arch is optional
    const std::string core_path = boost::dll::this_line_location().generic_string();
    const std::string lib_prefix("/" MODULE_LIB_PREFIX "/");
    auto it = std::search(core_path.rbegin(), core_path.rend(), lib_prefix.rbegin(), lib_prefix.rend());

    if(it == core_path.rend())
    {
        // But if we link statically, for instance linking with sight_core as an object library, then
        // boost::dll::this_line_location() will return the location of the current executable
        // In this case, we know that have to locate the bin directory instead of the library directory
        const std::string bin_prefix("/" MODULE_BIN_PREFIX "/");
        it = std::search(core_path.rbegin(), core_path.rend(), bin_prefix.rbegin(), bin_prefix.rend());
        SIGHT_FATAL_IF(
            "Failed to locate Sight runtime. We tried to guess it from: " + core_path,
            it == core_path.rend()
        );
    }

    const auto dist = std::distance(it, core_path.rend());
    const std::filesystem::path lib_path(core_path.begin(), core_path.begin() + dist - 1);
    m_working_path = lib_path.parent_path().make_preferred();
    SIGHT_INFO("Located Sight runtime in folder: " + m_working_path.string());
}

//------------------------------------------------------------------------------

runtime::~runtime()
= default;

//------------------------------------------------------------------------------

void runtime::add_module(std::shared_ptr<module> module)
{
    SIGHT_DEBUG("Module " + module->identifier() + " added.")
    m_modules.insert(module);
    std::for_each(
        module->extensions_begin(),
        module->extensions_end(),
        [this](auto e){add_extension(e);});
    std::for_each(
        module->extension_points_begin(),
        module->extension_points_end(),
        [this](auto&& PH1, auto&& ...){add_extension_point(std::forward<decltype(PH1)>(PH1));});
    std::for_each(
        module->executable_factories_begin(),
        module->executable_factories_end(),
        [this](auto&& PH1, auto&& ...){add_executable_factory(std::forward<decltype(PH1)>(PH1));});
}

//------------------------------------------------------------------------------

void runtime::unregister_module(std::shared_ptr<module> module)
{
    std::for_each(
        module->executable_factories_begin(),
        module->executable_factories_end(),
        [this](auto&& PH1, auto&& ...){unregister_executable_factory(std::forward<decltype(PH1)>(PH1));});
    std::for_each(
        module->extension_points_begin(),
        module->extension_points_end(),
        [this](auto&& PH1, auto&& ...){unregister_extension_point(std::forward<decltype(PH1)>(PH1));});
    std::for_each(
        module->extensions_begin(),
        module->extensions_end(),
        [this](auto&& PH1, auto&& ...){unregister_extension(std::forward<decltype(PH1)>(PH1));});
    m_modules.erase(module);
}

//------------------------------------------------------------------------------

void runtime::add_modules(const std::filesystem::path& repository)
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
        const auto modules = core::runtime::detail::io::module_descriptor_reader::create_modules(repository);
        std::for_each(
            modules.begin(),
            modules.end(),
            [this](auto&& PH1, auto&& ...)
            {
                add_module(std::forward<decltype(PH1)>(PH1));
            });
        const auto lib_repo_str = std::regex_replace(
            repository.lexically_normal().string(),
            s_match_module_path,
            MODULE_LIB_PREFIX
        );
        m_repositories.emplace_back(
            std::filesystem::weakly_canonical(std::filesystem::path(lib_repo_str)),
            repository

        );
    }
    catch(const std::exception& exception)
    {
        throw runtime_exception(std::string("Error while adding modules. ") + exception.what());
    }
}

//------------------------------------------------------------------------------

void runtime::add_executable_factory(std::shared_ptr<executable_factory> factory)
{
    // Ensures no registered factory has the same identifier
    const std::string type(factory->get_type());
    if(this->find_executable_factory(type) != nullptr)
    {
        throw runtime_exception(type + ": type already used by an executable factory.");
    }

    // Stores the executable factory.
    m_executable_factories.insert(factory);
}

//------------------------------------------------------------------------------

void runtime::unregister_executable_factory(std::shared_ptr<executable_factory> factory)
{
    // Ensures no registered factory has the same identifier.
    const std::string type(factory->get_type());
    SIGHT_WARN_IF("ExecutableFactory Type " + type + " not found.", this->find_executable_factory(type) == nullptr);

    // Removes the executable factory.
    m_executable_factories.erase(factory);
}

//------------------------------------------------------------------------------

std::shared_ptr<executable_factory> runtime::find_executable_factory(const std::string& _type) const
{
    const std::string type = filter_id(_type);
    std::shared_ptr<executable_factory> res_factory;
    for(const executable_factory_container::value_type& factory : m_executable_factories)
    {
        if(factory->get_type() == type && factory->enabled())
        {
            res_factory = factory;
            break;
        }
    }

    return res_factory;
}

//------------------------------------------------------------------------------

void runtime::add_extension(std::shared_ptr<detail::extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(filter_id(extension->identifier()));
    if(!identifier.empty() && this->find_extension(identifier) != nullptr)
    {
        throw runtime_exception(identifier + ": identifier already used by a registered extension.");
    }

    // Stores the extension.
    m_extensions.insert(extension);
}

//------------------------------------------------------------------------------

void runtime::unregister_extension(std::shared_ptr<detail::extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(filter_id(extension->identifier()));
    SIGHT_WARN_IF(
        "Extension " + identifier + " not found.",
        !identifier.empty() && this->find_extension(identifier) == nullptr
    );

    // Removes the extension.
    m_extensions.erase(extension);
}

//------------------------------------------------------------------------------

runtime::extension_container runtime::extensions() const
{
    return m_extensions;
}

//------------------------------------------------------------------------------

runtime::extension_iterator runtime::extensions_begin() const
{
    return m_extensions.begin();
}

//------------------------------------------------------------------------------

runtime::extension_iterator runtime::extensions_end() const
{
    return m_extensions.end();
}

//------------------------------------------------------------------------------

void runtime::add_extension_point(std::shared_ptr<extension_point> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(filter_id(point->identifier()));
    if(this->find_extension_point(identifier) != nullptr)
    {
        throw runtime_exception(identifier + ": identifier already used by a registered extension point.");
    }

    // Stores the extension.
    m_extension_points.insert(point);
}

//------------------------------------------------------------------------------

void runtime::unregister_extension_point(std::shared_ptr<extension_point> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(filter_id(point->identifier()));
    SIGHT_WARN_IF("ExtensionPoint " + identifier + " not found.", this->find_extension_point(identifier) == nullptr);

    m_extension_points.erase(point);
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::module> runtime::find_module(const std::string& identifier) const
{
    SIGHT_ASSERT("Module identifier should not be empty", !identifier.empty());

    const std::string id = filter_id(identifier);

    std::shared_ptr<core::runtime::module> res_module;
    for(const auto& module : m_modules)
    {
        if(module->identifier() == id)
        {
            res_module = module;
            break;
        }
    }

    return res_module;
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::module> runtime::find_enabled_module(const std::string& identifier) const
{
    SIGHT_ASSERT("Module identifier should not be empty", !identifier.empty());

    const std::string id = filter_id(identifier);

    std::shared_ptr<core::runtime::module> res_module;
    for(const auto& module : m_modules)
    {
        if(module->identifier() == id && module->enabled())
        {
            res_module = module;
            break;
        }
    }

    return res_module;
}

//------------------------------------------------------------------------------

runtime& runtime::get()
{
    if(m_instance == nullptr)
    {
        m_instance = std::make_shared<runtime>();
    }

    auto* runtime = m_instance.get();
    return *runtime;
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::extension> runtime::find_extension(const std::string& identifier) const
{
    const std::string id = filter_id(identifier);
    std::shared_ptr<core::runtime::extension> res_extension;
    for(const extension_container::value_type& extension : m_extensions)
    {
        if(extension->identifier() == id && extension->enabled())
        {
            res_extension = extension;
            break;
        }
    }

    return res_extension;
}

//------------------------------------------------------------------------------

core::runtime::detail::runtime::module_container runtime::modules() const
{
    module_container modules;
    std::copy(m_modules.begin(), m_modules.end(), std::inserter(modules, modules.begin()));
    return modules;
}

//------------------------------------------------------------------------------

std::filesystem::path runtime::working_path() const
{
    return m_working_path;
}

//------------------------------------------------------------------------------

std::vector<std::pair<std::filesystem::path, std::filesystem::path> > runtime::get_repositories_path() const
{
    return m_repositories;
}

//------------------------------------------------------------------------------

std::shared_ptr<extension_point> runtime::find_extension_point(const std::string& identifier) const
{
    const std::string id = filter_id(identifier);
    std::shared_ptr<extension_point> res_extension_point;
    for(const extension_point_container::value_type& extension_point : m_extension_points)
    {
        if(extension_point->identifier() == id && extension_point->enabled())
        {
            res_extension_point = extension_point;
            break;
        }
    }

    return res_extension_point;
}

//------------------------------------------------------------------------------

executable* runtime::create_executable_instance(const std::string& type) const
{
    std::shared_ptr<executable_factory> factory;

    // Retrieves the executable factory.
    factory = this->find_executable_factory(type);
    if(factory == nullptr)
    {
        throw runtime_exception(type + ": no executable factory found for that type.");
    }

    // Creates the executable instance
    executable* result(factory->create_executable());

    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail

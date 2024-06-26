/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
const std::regex runtime::MATCH_MODULE_PATH("share(?!.*share)\\\\.*");
#else
const std::regex runtime::MATCH_MODULE_PATH("share(?!.*share)/.*");
#endif

//------------------------------------------------------------------------------

std::shared_ptr<runtime> runtime::s_instance;

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
    m_working_path = std::filesystem::weakly_canonical(lib_path.parent_path().make_preferred());
    SIGHT_INFO("Located Sight runtime in folder: " + m_working_path.string());
}

//------------------------------------------------------------------------------

runtime::~runtime()
= default;

//------------------------------------------------------------------------------

void runtime::add_module(std::shared_ptr<module> _module)
{
    SIGHT_DEBUG("Module " + _module->identifier() + " added.")
    SIGHT_ASSERT(
        "Module " + _module->identifier() + " already added.",
        std::ranges::find_if(
            m_modules,
            [&](const auto& x){return x->identifier() == _module->identifier();}) == m_modules.cend()
    );

    m_modules.insert(_module);
    std::for_each(
        _module->extensions_begin(),
        _module->extensions_end(),
        [this](auto _e){add_extension(_e);});
    std::for_each(
        _module->extension_points_begin(),
        _module->extension_points_end(),
        [this](auto&& _p_h1, auto&& ...){add_extension_point(std::forward<decltype(_p_h1)>(_p_h1));});
    std::for_each(
        _module->executable_factories_begin(),
        _module->executable_factories_end(),
        [this](auto&& _p_h1, auto&& ...){add_executable_factory(std::forward<decltype(_p_h1)>(_p_h1));});
}

//------------------------------------------------------------------------------

void runtime::unregister_module(std::shared_ptr<module> _module)
{
    std::for_each(
        _module->executable_factories_begin(),
        _module->executable_factories_end(),
        [this](auto&& _p_h1, auto&& ...){unregister_executable_factory(std::forward<decltype(_p_h1)>(_p_h1));});
    std::for_each(
        _module->extension_points_begin(),
        _module->extension_points_end(),
        [this](auto&& _p_h1, auto&& ...){unregister_extension_point(std::forward<decltype(_p_h1)>(_p_h1));});
    std::for_each(
        _module->extensions_begin(),
        _module->extensions_end(),
        [this](auto&& _p_h1, auto&& ...){unregister_extension(std::forward<decltype(_p_h1)>(_p_h1));});
    m_modules.erase(_module);
}

//------------------------------------------------------------------------------

void runtime::add_modules(const std::filesystem::path& _repository)
{
    for(const auto& repo : m_repositories)
    {
        if(repo.second == _repository)
        {
            // Avoid adding modules several times, but we don't consider this as an error for the sake of simplicity...
            return;
        }
    }

    try
    {
        const auto&& [abs_repo_path, modules] = detail::io::module_descriptor_reader::create_modules(_repository);
        std::for_each(
            modules.begin(),
            modules.end(),
            [this](auto&& _p_h1, auto&& ...)
            {
                add_module(std::forward<decltype(_p_h1)>(_p_h1));
            });
        const auto lib_repo_str = std::regex_replace(
            _repository.lexically_normal().string(),
            MATCH_MODULE_PATH,
            MODULE_LIB_PREFIX
        );
        m_repositories.emplace_back(
            std::filesystem::weakly_canonical(std::filesystem::path(lib_repo_str)),
            abs_repo_path
        );
    }
    catch(const std::exception& exception)
    {
        throw runtime_exception(std::string("Error while adding modules. ") + exception.what());
    }
}

//------------------------------------------------------------------------------

void runtime::add_executable_factory(std::shared_ptr<executable_factory> _factory)
{
    // Ensures no registered factory has the same identifier
    const std::string type(_factory->type());
    if(this->find_executable_factory(type) != nullptr)
    {
        throw runtime_exception(type + ": type already used by an executable factory.");
    }

    // Stores the executable factory.
    m_executable_factories.insert(_factory);
}

//------------------------------------------------------------------------------

void runtime::unregister_executable_factory(std::shared_ptr<executable_factory> _factory)
{
    // Ensures no registered factory has the same identifier.
    const std::string type(_factory->type());
    SIGHT_WARN_IF("ExecutableFactory Type " + type + " not found.", this->find_executable_factory(type) == nullptr);

    // Removes the executable factory.
    m_executable_factories.erase(_factory);
}

//------------------------------------------------------------------------------

std::shared_ptr<executable_factory> runtime::find_executable_factory(const std::string& _type) const
{
    const std::string type = filter_id(_type);
    std::shared_ptr<executable_factory> res_factory;
    for(const auto& factory : m_executable_factories)
    {
        if(factory->type() == type && factory->enabled())
        {
            res_factory = factory;
            break;
        }
    }

    return res_factory;
}

//------------------------------------------------------------------------------

void runtime::add_extension(std::shared_ptr<detail::extension> _extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(filter_id(_extension->identifier()));
    if(!identifier.empty() && this->find_extension(identifier) != nullptr)
    {
        throw runtime_exception(identifier + ": identifier already used by a registered extension.");
    }

    // Stores the extension.
    m_extensions.insert(_extension);
}

//------------------------------------------------------------------------------

void runtime::unregister_extension(std::shared_ptr<detail::extension> _extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(filter_id(_extension->identifier()));
    SIGHT_WARN_IF(
        "Extension " + identifier + " not found.",
        !identifier.empty() && this->find_extension(identifier) == nullptr
    );

    // Removes the extension.
    m_extensions.erase(_extension);
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

void runtime::add_extension_point(std::shared_ptr<extension_point> _point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(filter_id(_point->identifier()));
    if(this->find_extension_point(identifier) != nullptr)
    {
        throw runtime_exception(identifier + ": identifier already used by a registered extension point.");
    }

    // Stores the extension.
    m_extension_points.insert(_point);
}

//------------------------------------------------------------------------------

void runtime::unregister_extension_point(std::shared_ptr<extension_point> _point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(filter_id(_point->identifier()));
    SIGHT_WARN_IF("ExtensionPoint " + identifier + " not found.", this->find_extension_point(identifier) == nullptr);

    m_extension_points.erase(_point);
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::module> runtime::find_module(const std::string& _identifier) const
{
    SIGHT_ASSERT("Module identifier should not be empty", !_identifier.empty());

    const std::string id = filter_id(_identifier);

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

std::shared_ptr<core::runtime::module> runtime::find_enabled_module(const std::string& _identifier) const
{
    SIGHT_ASSERT("Module identifier should not be empty", !_identifier.empty());

    const std::string id = filter_id(_identifier);

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
    if(s_instance == nullptr)
    {
        s_instance = std::make_shared<runtime>();
    }

    auto* runtime = s_instance.get();
    return *runtime;
}

//------------------------------------------------------------------------------

std::shared_ptr<core::runtime::extension> runtime::find_extension(const std::string& _identifier) const
{
    const std::string id = filter_id(_identifier);
    std::shared_ptr<core::runtime::extension> res_extension;
    for(const auto& extension : m_extensions)
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

std::shared_ptr<extension_point> runtime::find_extension_point(const std::string& _identifier) const
{
    const std::string id = filter_id(_identifier);
    std::shared_ptr<extension_point> res_extension_point;
    for(const auto& extension_point : m_extension_points)
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

executable* runtime::create_executable_instance(const std::string& _type) const
{
    std::shared_ptr<executable_factory> factory;

    // Retrieves the executable factory.
    factory = this->find_executable_factory(_type);
    if(factory == nullptr)
    {
        throw runtime_exception(_type + ": no executable factory found for that type.");
    }

    // Creates the executable instance
    executable* result(factory->create_executable());

    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail

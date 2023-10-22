/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "core/runtime/path.hpp"

#include "core/runtime/detail/module.hpp"
#include "core/runtime/detail/runtime.hpp"

#include <core/spy_log.hpp>

#include <algorithm>
#include <ranges>
#include <regex>
#include <utility>
#include <vector>

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

std::filesystem::path working_path() noexcept
{
    const auto& runtime = detail::runtime::get();
    return runtime.working_path();
}

//------------------------------------------------------------------------------

std::filesystem::path get_module_resource_path(const std::string& _module_identifier) noexcept
{
    const auto& runtime            = detail::runtime::get();
    std::shared_ptr<module> module = runtime.find_module(_module_identifier);

    if(module == nullptr)
    {
        SIGHT_ERROR("Could not find module " + _module_identifier + "'");
        return {};
    }

    return module->get_resources_location();
}

//------------------------------------------------------------------------------

std::filesystem::path get_module_resource_file_path(
    const std::string& _module_identifier,
    const std::filesystem::path& _path
) noexcept
{
    const auto& runtime            = detail::runtime::get();
    std::shared_ptr<module> module = runtime.find_module(_module_identifier);

    if(module == nullptr)
    {
        SIGHT_ERROR("Could not find module '" + _module_identifier + "'");
        return {};
    }

    return get_module_resource_path(module, _path);
}

//------------------------------------------------------------------------------

std::filesystem::path get_module_resource_file_path(const std::filesystem::path& _path) noexcept
{
    SIGHT_ASSERT("Path should not be empty", !_path.empty());
    SIGHT_ASSERT("Path should be relative", _path.is_relative());

    const std::string module_folder = _path.begin()->string();

    // Strip the module name
    std::filesystem::path path_without_module;
    for(const auto& it_path : std::views::drop(_path, 1))
    {
        path_without_module /= it_path;
    }

    try
    {
        const auto& runtime            = detail::runtime::get();
        std::shared_ptr<module> module = runtime.find_module(module_folder);

        if(module == nullptr)
        {
            SIGHT_DEBUG("Could not find module '" + module_folder + "'");
            return {};
        }

        return get_module_resource_path(module, path_without_module);
    }
    catch(...)
    {
        SIGHT_ERROR("Error looking for module '" + module_folder + "'");
        return {};
    }
}

//------------------------------------------------------------------------------

std::filesystem::path get_library_resource_file_path(const std::filesystem::path& _path) noexcept
{
    // The path argument can be a filesystem path or a module identifier followed by a system paths, i.e.
    // - viz_scene3d/media
    // - sight::viz::scene3d/media
    // The module identifier is the more robust choice since it can not be ambiguous
    const auto& runtime = core::runtime::detail::runtime::get();

    // Extract the namespace
    std::smatch match;
    std::string path_str = _path.string();
    std::string lib_namespace;

    // If we have sight::viz::scene3d/media
    if(std::regex_match(path_str, match, std::regex("(?:^:*)(\\w*)::(.*)")))
    {
        lib_namespace = match[1].str(); // equals sight
        path_str      = match[2].str(); // equals viz::scene3d/media
    }

    path_str = std::regex_replace(path_str, std::regex("::"), "_"); // equals viz_scene3d/media

    // The library resources are at the same location than modules
    // Find a repo that matches the library namespace
    const auto repositories = runtime.get_repositories_path();
    for(const auto& repo : repositories)
    {
        std::filesystem::path last_path = *(--repo.second.end());
        if(last_path.string() == lib_namespace)
        {
            return repo.second / path_str;
        }
    }

    // Fallback, if we did not find anything. Especially useful at start for the default module path.
    return std::filesystem::weakly_canonical(core::runtime::working_path() / MODULE_RC_PREFIX / path_str);
}

//------------------------------------------------------------------------------

std::filesystem::path get_resource_file_path(const std::filesystem::path& _path) noexcept
{
    auto file = core::runtime::get_module_resource_file_path(_path);
    if(file.empty())
    {
        // If not found in a module, look into libraries
        file = core::runtime::get_library_resource_file_path(_path);
        SIGHT_ERROR_IF("Resource '" + _path.string() + "' has not been found in any module or library", file.empty());
    }

    return file;
}

//------------------------------------------------------------------------------

std::filesystem::path get_module_resource_path(
    std::shared_ptr<module> _module,
    const std::filesystem::path& _path
) noexcept
{
    return _module->get_resources_location() / _path;
}

//------------------------------------------------------------------------------

std::set<std::filesystem::path> get_binaries_paths() noexcept
{
    std::set<std::filesystem::path> binaries_paths;

    const auto& runtime = core::runtime::detail::runtime::get();
    for(const auto& [lib, module] : runtime.get_repositories_path())
    {
        binaries_paths.emplace(std::filesystem::weakly_canonical(lib / ".." / "bin"));
    }

    return binaries_paths;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime

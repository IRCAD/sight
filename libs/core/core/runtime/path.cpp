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

#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/Runtime.hpp"

#include <core/spyLog.hpp>

#include <algorithm>
#include <ranges>
#include <regex>
#include <utility>
#include <vector>

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

std::filesystem::path getWorkingPath() noexcept
{
    const auto& runtime = detail::Runtime::get();
    return runtime.getWorkingPath();
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourcePath(const std::string& moduleIdentifier) noexcept
{
    const auto& runtime            = detail::Runtime::get();
    std::shared_ptr<Module> module = runtime.findModule(moduleIdentifier);

    if(module == nullptr)
    {
        SIGHT_ERROR("Could not find module " + moduleIdentifier + "'");
        return {};
    }

    return module->getResourcesLocation();
}

//------------------------------------------------------------------------------

std::filesystem::path getModuleResourceFilePath(
    const std::string& moduleIdentifier,
    const std::filesystem::path& path
) noexcept
{
    const auto& runtime            = detail::Runtime::get();
    std::shared_ptr<Module> module = runtime.findModule(moduleIdentifier);

    if(module == nullptr)
    {
        SIGHT_ERROR("Could not find module '" + moduleIdentifier + "'");
        return {};
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
    for(auto itPath = ++path.begin() ; itPath != path.end() ; itPath++)
    {
        pathWithoutModule /= *itPath;
    }

    try
    {
        const auto& runtime            = detail::Runtime::get();
        std::shared_ptr<Module> module = runtime.findModule(moduleFolder);

        if(module == nullptr)
        {
            SIGHT_DEBUG("Could not find module '" + moduleFolder + "'");
            return {};
        }

        return getModuleResourcePath(module, pathWithoutModule);
    }
    catch(...)
    {
        SIGHT_ERROR("Error looking for module '" + moduleFolder + "'");
        return {};
    }
}

//------------------------------------------------------------------------------

std::filesystem::path getLibraryResourceFilePath(const std::filesystem::path& path) noexcept
{
    // The path argument can be a filesystem path or a module identifier followed by a system paths, i.e.
    // - viz_scene3d/media
    // - sight::viz::scene3d/media
    // The module identifier is the more robust choice since it can not be ambiguous
    const auto& runtime = core::runtime::detail::Runtime::get();

    // Extract the namespace
    std::smatch match;
    std::string pathStr = path.string();
    std::string libNamespace;

    // If we have sight::viz::scene3d/media
    if(std::regex_match(pathStr, match, std::regex("(?:^:*)(\\w*)::(.*)")))
    {
        libNamespace = match[1].str(); // equals sight
        pathStr      = match[2].str(); // equals viz::scene3d/media
    }

    pathStr = std::regex_replace(pathStr, std::regex("::"), "_"); // equals viz_scene3d/media

    // The library resources are at the same location than modules
    // Find a repo that matches the library namespace
    const auto repositories = runtime.getRepositoriesPath();
    for(const auto& repo : repositories)
    {
        std::filesystem::path lastPath = *(--repo.second.end());
        if(lastPath.string() == libNamespace)
        {
            return repo.second / pathStr;
        }
    }

    // Fallback, if we did not find anything. Especially useful at start for the default module path.
    return std::filesystem::weakly_canonical(core::runtime::getWorkingPath() / MODULE_RC_PREFIX / pathStr);
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

std::filesystem::path getModuleResourcePath(
    std::shared_ptr<Module> module,
    const std::filesystem::path& path
) noexcept
{
    return module->getResourcesLocation() / path;
}

//------------------------------------------------------------------------------

std::set<std::filesystem::path> getBinariesPaths() noexcept
{
    std::set<std::filesystem::path> binaries_paths;

    const auto& runtime = core::runtime::detail::Runtime::get();
    for(const auto& [lib, module] : runtime.getRepositoriesPath())
    {
        binaries_paths.emplace(std::filesystem::weakly_canonical(lib / ".." / "bin"));
    }

    return binaries_paths;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime

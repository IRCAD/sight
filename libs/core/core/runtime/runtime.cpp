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

#include "core/runtime/runtime.hpp"

#include "core/runtime/ConfigurationElement.hpp"
#include "core/runtime/detail/dl/Library.hpp"
#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/io/ProfileReader.hpp"
#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/profile/Profile.hpp"
#include "core/runtime/detail/Runtime.hpp"

#include <core/spyLog.hpp>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <ranges>
#include <regex>
#include <utility>
#include <vector>

namespace sight::core::runtime
{

//------------------------------------------------------------------------- -----

void init()
{
    // Load default modules
    auto& runtime       = detail::Runtime::get();
    const auto location = (runtime.getWorkingPath() / MODULE_RC_PREFIX).lexically_normal();
    SIGHT_INFO("Launching Sight runtime in: " + location.string());

    auto profile = std::make_shared<detail::profile::Profile>();
    detail::profile::setCurrentProfile(profile);

    SIGHT_ASSERT("Default Modules location not found: " + location.string(), std::filesystem::exists(location));

    // Read modules
    runtime.addModules(location);
    SIGHT_ASSERT("Couldn't load any module from path: " + location.string(), !runtime.getModules().empty());
}

//------------------------------------------------------------------------- -----

void shutdown()
{
    auto profile = detail::profile::getCurrentProfile();
    profile->stop();
}

//------------------------------------------------------------------------------

void addModules(const std::filesystem::path& directory)
{
    SIGHT_INFO("Loading modules from: " + directory.string());

    auto& runtime = detail::Runtime::get();
    runtime.addModules(directory);
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> loadModule(const std::string& identifier)
{
    auto module = std::dynamic_pointer_cast<detail::Module>(detail::Runtime::get().findModule(identifier));

    if(module)
    {
        module->setEnable(true);
        module->start();
    }

    return module;
}

//------------------------------------------------------------------------------

void unloadModule(const std::string& identifier)
{
    auto module = std::dynamic_pointer_cast<detail::Module>(detail::Runtime::get().findModule(identifier));

    if(module)
    {
        module->stop();
    }
}

//------------------------------------------------------------------------------

bool loadLibrary(const std::string& identifier)
{
    static std::map<std::string, std::shared_ptr<detail::dl::Library> > s_LIBRARIES;

    // Even if dlopen does not actually load twice the same library, we avoid this
    if(s_LIBRARIES.find(identifier) != std::end(s_LIBRARIES))
    {
        return true;
    }

    auto library        = std::make_shared<detail::dl::Library>(identifier);
    const auto& runtime = core::runtime::detail::Runtime::get();

    // Try to load from all known paths
    const auto repositories = runtime.getRepositoriesPath();
    for(const auto& repo : repositories)
    {
        library->setSearchPath(repo.first);
        try
        {
            library->load();
            s_LIBRARIES[identifier] = library;
            return true;
        }
        catch(const RuntimeException&)
        {
            // Fail silently and potentially try in the next repository
        }
    }

    SIGHT_ERROR("Could not load library '" + identifier);

    return false;
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> findModule(const std::string& identifier)
{
    return detail::Runtime::get().findModule(identifier);
}

//------------------------------------------------------------------------------

std::set<std::shared_ptr<Module> > getModules()
{
    return detail::Runtime::get().getModules();
}

//------------------------------------------------------------------------------

void startModule(const std::string& identifier)
{
    // Retrieves the specified module.
    std::shared_ptr<Module> module = detail::Runtime::get().findModule(identifier);
    if(module == nullptr)
    {
        throw RuntimeException(identifier + ": module not found.");
    }

    // Starts the found module.
    module->start();
}

//------------------------------------------------------------------------------

std::shared_ptr<Extension> findExtension(const std::string& identifier)
{
    return detail::Runtime::get().findExtension(identifier);
}

//------------------------------------------------------------------------------

std::vector<std::shared_ptr<core::runtime::Extension> > getAllExtensionsForPoint(std::string extension_pt)
{
    auto& runtime                                 = core::runtime::detail::Runtime::get();
    std::shared_ptr<detail::ExtensionPoint> point = runtime.findExtensionPoint(extension_pt);

    if(!point)
    {
        throw RuntimeException(extension_pt + ": invalid extension point identifier.");
    }

    std::vector<std::shared_ptr<core::runtime::Extension> > extensions;
    std::ranges::transform(point->getAllExtensions(), std::back_inserter(extensions), [](auto e){return e;});

    return extensions;
}

//-----------------------------------------------------------------------------

std::string filterID(const std::string& identifier)
{
    return boost::algorithm::trim_left_copy_if(identifier, [](auto x){return x == ':';});
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime

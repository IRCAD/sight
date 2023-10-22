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

#include "core/runtime/detail/dl/library.hpp"
#include "core/runtime/detail/extension_point.hpp"
#include "core/runtime/detail/io/profile_reader.hpp"
#include "core/runtime/detail/module.hpp"
#include "core/runtime/detail/profile/profile.hpp"

#include <core/spy_log.hpp>

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
    auto& runtime       = detail::runtime::get();
    const auto location = (runtime.working_path() / MODULE_RC_PREFIX).lexically_normal();
    SIGHT_INFO("Launching Sight runtime in: " + location.string());

    auto profile = std::make_shared<detail::profile::profile>();
    detail::profile::set_current_profile(profile);

    SIGHT_ASSERT("Default Modules location not found: " + location.string(), std::filesystem::exists(location));

    // Read modules
    runtime.add_modules(location);
    SIGHT_ASSERT("Couldn't load any module from path: " + location.string(), !runtime.modules().empty());
}

//------------------------------------------------------------------------- -----

void shutdown()
{
    auto profile = detail::profile::get_current_profile();
    profile->stop();
}

//------------------------------------------------------------------------------

void add_modules(const std::filesystem::path& _directory)
{
    SIGHT_INFO("Loading modules from: " + _directory.string());

    auto& runtime = detail::runtime::get();
    runtime.add_modules(_directory);
}

//------------------------------------------------------------------------------

std::shared_ptr<module> load_module(const std::string& _identifier)
{
    auto module = std::dynamic_pointer_cast<detail::module>(detail::runtime::get().find_module(_identifier));

    if(module)
    {
        module->set_enable(true);
        module->start();
    }

    return module;
}

//------------------------------------------------------------------------------

void unload_module(const std::string& _identifier)
{
    auto module = std::dynamic_pointer_cast<detail::module>(detail::runtime::get().find_module(_identifier));

    if(module)
    {
        module->stop();
    }
}

//------------------------------------------------------------------------------

bool load_library(const std::string& _identifier)
{
    static std::map<std::string, std::shared_ptr<detail::dl::library> > s_libraries;

    // Even if dlopen does not actually load twice the same library, we avoid this
    if(s_libraries.find(_identifier) != std::end(s_libraries))
    {
        return true;
    }

    auto library        = std::make_shared<detail::dl::library>(_identifier);
    const auto& runtime = core::runtime::detail::runtime::get();

    // Try to load from all known paths
    const auto repositories = runtime.get_repositories_path();
    for(const auto& repo : repositories)
    {
        library->set_search_path(repo.first);
        try
        {
            library->load();
            s_libraries[_identifier] = library;
            return true;
        }
        catch(const runtime_exception&)
        {
            // Fail silently and potentially try in the next repository
        }
    }

    SIGHT_ERROR("Could not load library '" + _identifier);

    return false;
}

//------------------------------------------------------------------------------

std::shared_ptr<module> find_module(const std::string& _identifier)
{
    return detail::runtime::get().find_module(_identifier);
}

//------------------------------------------------------------------------------

std::set<std::shared_ptr<module> > modules()
{
    return detail::runtime::get().modules();
}

//------------------------------------------------------------------------------

void start_module(const std::string& _identifier)
{
    // Retrieves the specified module.
    std::shared_ptr<module> module = detail::runtime::get().find_module(_identifier);
    if(module == nullptr)
    {
        throw runtime_exception(_identifier + ": module not found.");
    }

    // Starts the found module.
    module->start();
}

//------------------------------------------------------------------------------

std::shared_ptr<extension> find_extension(const std::string& _identifier)
{
    return detail::runtime::get().find_extension(_identifier);
}

//------------------------------------------------------------------------------

std::vector<std::shared_ptr<core::runtime::extension> > get_all_extensions_for_point(std::string _extension_pt)
{
    auto& runtime                                  = core::runtime::detail::runtime::get();
    std::shared_ptr<detail::extension_point> point = runtime.find_extension_point(_extension_pt);

    if(!point)
    {
        throw runtime_exception(_extension_pt + ": invalid extension point identifier.");
    }

    std::vector<std::shared_ptr<core::runtime::extension> > extensions;
    std::ranges::transform(point->get_all_extensions(), std::back_inserter(extensions), [](auto _e){return _e;});

    return extensions;
}

//-----------------------------------------------------------------------------

std::string filter_id(const std::string& _identifier)
{
    return boost::algorithm::trim_left_copy_if(_identifier, [](auto _x){return _x == ':';});
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime

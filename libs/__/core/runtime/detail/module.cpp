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

#include "core/runtime/detail/module.hpp"

#include "core/runtime/detail/empty_plugin.hpp"
#include "core/runtime/detail/extension_point.hpp"
#include "core/runtime/detail/profile/profile.hpp"
#include "core/runtime/detail/runtime.hpp"
#include "core/runtime/executable.hpp"
#include "core/runtime/executable_factory.hpp"
#include "core/runtime/extension.hpp"
#include "core/runtime/utils/generic_executable_factory.hpp"

#include <core/base.hpp>

#include <algorithm>
#include <cassert>
#include <exception>
#include <memory>

namespace sight::core::runtime::detail
{

//------------------------------------------------------------------------------

SPTR(module) module::s_loading_module;

//------------------------------------------------------------------------------

SPTR(module) module::get_loading_module()
{
    return s_loading_module;
}

//------------------------------------------------------------------------------

module::module(
    const std::filesystem::path& _location,
    std::string _id,
    std::string _c,
    int _priority
) :
    m_resources_location(_location.lexically_normal()),
    m_identifier(std::move(_id)),
    m_class(std::move(_c)),
    m_priority(_priority)
{
    // Post-condition.
    SIGHT_ASSERT("Invalid module location.", m_resources_location.is_absolute() == true);

    const auto location_normalized = std::filesystem::weakly_canonical(_location);
    const auto str_location        = std::regex_replace(
        location_normalized.string(),
        runtime::MATCH_MODULE_PATH,
        MODULE_LIB_PREFIX
    );

    // This may fail if the module does not contain any library, so we ignore the returned error
    m_library_location = std::filesystem::weakly_canonical(std::filesystem::path(str_location));
}

//------------------------------------------------------------------------------

void module::add_executable_factory(SPTR(executable_factory)_factory)
{
    m_executable_factories.insert(_factory);
}

//------------------------------------------------------------------------------

module::executable_factory_const_iterator module::executable_factories_begin() const
{
    return m_executable_factories.begin();
}

//------------------------------------------------------------------------------

module::executable_factory_const_iterator module::executable_factories_end() const
{
    return m_executable_factories.end();
}

//------------------------------------------------------------------------------

SPTR(executable_factory) module::find_executable_factory(const std::string& _type) const
{
    std::shared_ptr<executable_factory> res_executable_factory;
    for(const auto& factory : m_executable_factories)
    {
        if(factory->type() == _type)
        {
            res_executable_factory = factory;
            break;
        }
    }

    return res_executable_factory;
}

//------------------------------------------------------------------------------

void module::add_extension(SPTR(extension)_extension)
{
    m_extensions.insert(_extension);
}

//------------------------------------------------------------------------------

bool module::has_extension(const std::string& _identifier) const
{
    bool has_extension = false;
    for(const auto& extension : m_extensions)
    {
        if(extension->identifier() == _identifier)
        {
            has_extension = true;
            break;
        }
    }

    return has_extension;
}

//------------------------------------------------------------------------------

void module::set_enable_extension(const std::string& _identifier, const bool _enable)
{
    for(const auto& extension : m_extensions)
    {
        if(extension->identifier() == _identifier)
        {
            extension->set_enable(_enable);
            break; // The identifier is unique => can break the loop
        }
    }
}

//------------------------------------------------------------------------------

module::extension_const_iterator module::extensions_begin() const
{
    return m_extensions.begin();
}

//------------------------------------------------------------------------------

module::extension_const_iterator module::extensions_end() const
{
    return m_extensions.end();
}

//------------------------------------------------------------------------------

void module::add_extension_point(SPTR(extension_point)_extension_point)
{
    m_extension_points.insert(_extension_point);
}

//------------------------------------------------------------------------------

SPTR(extension_point) module::find_extension_point(const std::string& _identifier) const
{
    std::shared_ptr<extension_point> res_extension_point;
    for(const auto& extension_point : m_extension_points)
    {
        if(extension_point->identifier() == _identifier && extension_point->enabled())
        {
            res_extension_point = extension_point;
            break;
        }
    }

    return res_extension_point;
}

//------------------------------------------------------------------------------

bool module::has_extension_point(const std::string& _identifier) const
{
    bool has_extension_point = false;
    for(const auto& extension_point : m_extension_points)
    {
        if(extension_point->identifier() == _identifier)
        {
            has_extension_point = true;
            break;
        }
    }

    return has_extension_point;
}

//------------------------------------------------------------------------------

void module::set_enable_extension_point(const std::string& _identifier, const bool _enable)
{
    for(const auto& extension_point : m_extension_points)
    {
        if(extension_point->identifier() == _identifier)
        {
            extension_point->set_enable(_enable);
            break;
        }
    }
}

//------------------------------------------------------------------------------

module::extension_point_const_iterator module::extension_points_begin() const
{
    return m_extension_points.begin();
}

//------------------------------------------------------------------------------

module::extension_point_const_iterator module::extension_points_end() const
{
    return m_extension_points.end();
}

//------------------------------------------------------------------------------

void module::set_library(SPTR(dl::library)_library)
{
    _library->set_search_path(this->get_library_location());
    m_library = _library;
}

//------------------------------------------------------------------------------

void module::add_requirement(const std::string& _requirement)
{
    m_requirements.insert(_requirement);
}

//------------------------------------------------------------------------------

std::string module::get_class() const
{
    return m_class;
}

//------------------------------------------------------------------------------

const std::string& module::identifier() const
{
    return m_identifier;
}

//------------------------------------------------------------------------------

std::string module::get_library_name() const
{
    return m_library ? m_library->name().string() : "";
}

//------------------------------------------------------------------------------

const std::filesystem::path& module::get_library_location() const
{
    return m_library_location;
}

//------------------------------------------------------------------------------

const std::filesystem::path& module::get_resources_location() const
{
    return m_resources_location;
}

//------------------------------------------------------------------------------

SPTR(plugin) module::get_plugin() const
{
    return m_plugin;
}

//------------------------------------------------------------------------------

void module::load_libraries()
{
    // Ensure the module is enabled.
    if(!m_enabled)
    {
        throw runtime_exception(get_module_str(m_identifier) + ": module is not enabled.");
    }

    // Pre-condition
    SIGHT_ASSERT("Module is already loaded", s_loading_module == nullptr);

    // References the current module as the loading module.
    s_loading_module = shared_from_this();

    // Loads the library
    if(m_library && !m_library->is_loaded())
    {
        try
        {
            m_library->load();
        }
        catch(std::exception& e)
        {
            std::string message;

            message += "Unable to load module ";
            message += m_library->name().string();
            message += ". ";
            message += e.what();

            SIGHT_ERROR(message);
            s_loading_module.reset();

            throw runtime_exception(message);
        }
    }

    // Unreferences the current module from the loading module.
    s_loading_module.reset();

    // Post-condition
    assert(s_loading_module == nullptr);
}

//------------------------------------------------------------------------------

void module::load_requirements()
{
    try
    {
        runtime& runtime = runtime::get();
        requirement_container::const_iterator iter;
        for(const auto& requirement : m_requirements)
        {
            auto module = std::dynamic_pointer_cast<detail::module>(runtime.find_module(requirement));

            // Ensure that a module has been retrieved.
            if(module == nullptr)
            {
                throw runtime_exception(requirement + ": required module not found.");
            }

            // Enable the required module if necessary.
            if(!module->enabled())
            {
                module->set_enable(true);
            }

            // Starts the module if necessary (loads its libraries and requirements module).
            module->start();
        }
    }
    catch(const std::exception& e)
    {
        std::string message;

        message += "Module " + get_module_str(m_identifier) + " was not able to load requirements. ";
        message += e.what();
        throw runtime_exception(message);
    }
}

//------------------------------------------------------------------------------

void module::start()
{
    if(!m_started)
    {
        if(!m_enabled)
        {
            throw runtime_exception(get_module_str(m_identifier) + ": module is not enabled.");
        }

        if(m_plugin == nullptr)
        {
            load_requirements();
            load_libraries();
            try
            {
                start_plugin();
            }
            catch(std::exception& e)
            {
                throw runtime_exception(
                          get_module_str(m_identifier)
                          + ": start plugin error (after load requirement) " + e.what()
                );
            }
            SIGHT_INFO("Loaded module '" + m_identifier + "' successfully");
        }
    }
}

//------------------------------------------------------------------------------

void module::start_plugin()
{
    SIGHT_ASSERT(
        "Module " + get_module_str(m_identifier) + " plugin is already started.",
        !m_started
    );

    // Retrieves the type of the plugin.
    std::string plugin_type(get_class());

    // According to the presence of a class or not, build and empty
    // plugin or attempt to instantiate a user defined plugin.
    SPTR(plugin) plugin;

    if(plugin_type.empty())
    {
        plugin = std::make_shared<empty_plugin>();
    }
    else
    {
        auto& runtime = runtime::get();
        SPTR(executable) executable(runtime.create_executable_instance(plugin_type));

        plugin = std::dynamic_pointer_cast<sight::core::runtime::plugin>(executable);
    }

    // Ensures that a plugin has been created.
    if(plugin == nullptr)
    {
        throw runtime_exception(get_module_str(m_identifier) + ": unable to create a plugin instance.");
    }

    if(core::runtime::get_current_profile())
    {
        // Stores and start the plugin.
        try
        {
            auto prof = std::dynamic_pointer_cast<detail::profile::profile>(core::runtime::get_current_profile());
            prof->add_stopper(this->identifier(), this->priority());

            m_plugin = plugin;
            m_plugin->start();

            m_started = true;
        }
        catch(std::exception& e)
        {
            throw runtime_exception(get_module_str(m_identifier) + ": start plugin error : " + e.what());
        }
    }
}

//------------------------------------------------------------------------------

void module::stop()
{
    if(m_started)
    {
        SIGHT_ASSERT(get_module_str(m_identifier) + " : m_plugin not an instance.", m_plugin != nullptr);

        try
        {
            m_plugin->stop();
            m_started = false;
        }
        catch(std::exception& e)
        {
            throw runtime_exception(get_module_str(m_identifier) + ": stop plugin error : " + e.what());
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

bool module::enabled() const
{
    return m_enabled;
}

//------------------------------------------------------------------------------

void module::set_enable(const bool _state)
{
    m_enabled = _state;
}

//------------------------------------------------------------------------------

void module::add_parameter(const std::string& _identifier, const std::string& _value)
{
    m_parameters[_identifier] = _value;
}

//------------------------------------------------------------------------------

std::string module::get_parameter_value(const std::string& _identifier) const
{
    auto found = m_parameters.find(_identifier);

    return (found != m_parameters.end()) ? found->second : std::string();
}

//------------------------------------------------------------------------------

bool module::has_parameter(const std::string& _identifier) const
{
    return m_parameters.find(_identifier) != m_parameters.end();
}

//------------------------------------------------------------------------------

core::runtime::module::extension_container core::runtime::detail::module::extensions() const
{
    core::runtime::module::extension_container container;
    std::ranges::transform(m_extensions, std::inserter(container, container.begin()), [](const auto& _e){return _e;});
    return container;
}

//------------------------------------------------------------------------------

std::string module::get_module_str(const std::string& _identifier)
{
    return _identifier;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail

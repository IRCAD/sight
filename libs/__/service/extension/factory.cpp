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

#include "service/extension/factory.hpp"

#include "service/base.hpp"

#include <core/lazy_instantiator.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/runtime.hpp>

#include <data/exception.hpp>

#include <functional>
#include <vector>

namespace sight::service::extension
{

//-----------------------------------------------------------------------------

factory::sptr factory::get()
{
    return core::lazy_instantiator<factory>::get_instance();
}

//-----------------------------------------------------------------------------

void factory::parse_plugin_infos()
{
    srv_reg_container_t module_info_map;

    using extension_t = std::shared_ptr<core::runtime::extension>;

    std::vector<extension_t> ext_elements;
    ext_elements = core::runtime::get_all_extensions_for_point("sight::service::extension::factory");
    for(const extension_t& ext_elt : ext_elements)
    {
        const core::runtime::config_t& cfg_elt_vec = ext_elt->get_config();
        std::string type;
        std::string service;
        std::vector<std::string> objects;
        std::string desc;
        std::string tags;

        for(const auto& cfg_elt : cfg_elt_vec)
        {
            const std::string elt = cfg_elt.first;
            const auto value      = cfg_elt.second.get_value<std::string>();
            if(elt == "type")
            {
                type = core::runtime::filter_id(value);
            }
            else if(elt == "service")
            {
                service = core::runtime::filter_id(value);
            }
            else if(elt == "object")
            {
                objects.push_back(core::runtime::filter_id(value));
            }
            else if(elt == "desc")
            {
                desc = value;
            }
            else if(elt == "tags")
            {
                tags = value;
            }
            else
            {
                SIGHT_FATAL("Unknown element !");
            }
        }

        SIGHT_ASSERT("Missing type element.", !type.empty());
        SIGHT_ASSERT("Missing service element.", !service.empty());

        service_info info;
        info.service_type            = type;
        info.objects_set_from_module = !objects.empty();
        info.object_impl             = std::move(objects);
        info.desc                    = desc;
        info.tags                    = tags;
        info.module                  = ext_elt->get_module();
        SIGHT_ASSERT("Module not found.", info.module);

        module_info_map.emplace(std::make_pair(service, info));
    }

    sight::service::extension::factory::print_info_map(module_info_map);

    core::mt::read_to_write_lock lock(m_srv_impl_to_srv_info_mutex);
    // Merge data info
    for(const auto& module : module_info_map)
    {
        auto iter = m_srv_impl_to_srv_info.find(module.first);

        if(iter != m_srv_impl_to_srv_info.end())
        {
            SIGHT_DEBUG(
                "We already have informations about this service  (from register macro) ( " << module.first
                << " )."
            );

            service_info& info              = iter->second;
            const service_info& info_module = module.second;

            SIGHT_ASSERT("Try to add a module, but this module already exists.", !info.module);
            SIGHT_ASSERT(
                "Try to add a module, but this srv is already registered and doesn't have the same srv type.",
                info_module.service_type == info.service_type
            );
            SIGHT_ASSERT(
                "Try to add a module, but the service '"
                << module.first << "' is already registered and does not have the same objects.",
                info_module.object_impl.empty() || info_module.object_impl == info.object_impl
            );

            info.module                  = info_module.module;
            info.desc                    = info_module.desc;
            info.objects_set_from_module = info_module.objects_set_from_module;
        }
        else
        {
            core::mt::upgrade_to_write_lock upgrade(lock);
            m_srv_impl_to_srv_info.emplace(std::make_pair(module.first, module.second));
        }
    }

    sight::service::extension::factory::print_info_map(m_srv_impl_to_srv_info);
    this->check_services_not_declared_in_plugin_xml();
}

//-----------------------------------------------------------------------------

base::sptr factory::create(const std::string& _srv_impl) const
{
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);
    base::sptr service;

    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);

    SIGHT_ASSERT(
        "The service called '" << srv_impl << "' does not exist in the factory ",
        iter != m_srv_impl_to_srv_info.end()
    );

    const service_info& info = iter->second;

    SIGHT_DEBUG("SR creates a new service ( classname = " << srv_impl << " )");

    if(info.factory)
    {
        service = info.factory();
    }
    else
    {
        SIGHT_ASSERT(
            "A module must declare the factory named"
            << srv_impl
            << ", the service declaration might be missing (or misspelled) in a module plugin.",
            info.module
        );
        SIGHT_ASSERT(
            "The module '" + info.module->identifier() + "' is already loaded and the factory '"
            + srv_impl + "' is still missing. The service declaration might be missing (or misspelled)"
                         "in a .cpp file.",
            !info.module->is_started()
        );

        lock.unlock(); // module->start() may trigger calls to add_factory
        info.module->start();
        lock.lock();

        SIGHT_THROW_EXCEPTION_IF(
            data::exception(
                "After loading the module " + info.module->identifier() + " , factory " + srv_impl
                + " is still missing. The service declaration might be missing (or misspelled) "
                  "in a cpp file."
            ),
            !info.factory
        );

        service = info.factory();
    }

    // Setup worker here, this is a better place than the constructor
    // because here, the service slots are also set up
    // This allows to setup
    service->set_worker(core::thread::get_default_worker());

    return service;
}

//------------------------------------------------------------------------------

void factory::add_service_factory(
    factoryType _factory,
    const std::string& _srv_impl,
    const std::string& _srv_type
)
{
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);
    const std::string srv_type = core::runtime::filter_id(_srv_type);

    SIGHT_DEBUG(std::string("New service registering : srvImpl =") + srv_impl + " srvType=" + srv_type);

    core::mt::read_to_write_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);

    if(iter != m_srv_impl_to_srv_info.end())
    {
        SIGHT_DEBUG("We already have informations about this service ( " + srv_impl + " ).");
        service_info& info = iter->second;
        SIGHT_ASSERT(
            "Try to add factory, but this srv ( " << srv_impl << " ) already has a registered factory.",
            !info.factory
        );
        SIGHT_ASSERT(
            "Try to add factory, but this srv ( "
            << srv_impl << " ) is already registered and doesn't have the same srv type. ( "
            << srv_type << " != " << info.service_type << " )",
            srv_type == info.service_type
        );

        core::mt::upgrade_to_write_lock upgrade(lock);
        info.factory = _factory;
    }
    else
    {
        SIGHT_DEBUG("Add new service factory in registry ( " + srv_impl + " ).");
        core::mt::upgrade_to_write_lock upgrade(lock);
        service_info info;
        info.service_type = srv_type;
        info.factory      = _factory;
        m_srv_impl_to_srv_info.emplace(std::make_pair(srv_impl, info));
    }
}

//-----------------------------------------------------------------------------

void factory::add_object_factory(const std::string& _srv_impl, const std::string& _oimpl)
{
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);
    const std::string o_impl   = core::runtime::filter_id(_oimpl);

    SIGHT_DEBUG(std::string("New object oImpl=") + o_impl + "registering to service: srvImpl =" + srv_impl);
    SIGHT_ASSERT("Empty oImpl", !o_impl.empty());

    core::mt::read_to_write_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);

    SIGHT_ASSERT(
        "Try to associate an object to a service factory, but this srv is not yet registered.",
        iter != m_srv_impl_to_srv_info.end()
    );

    if(iter != m_srv_impl_to_srv_info.end())
    {
        service_info& info = iter->second;

        // Either the module does not contain objects informations or this service does not belong to a module
        if(info.objects_set_from_module)
        {
#ifdef _DEBUG
            const auto it_find = std::find(info.object_impl.begin(), info.object_impl.end(), o_impl);
#endif
            SIGHT_ASSERT(
                "Try to add factory, but the service '" + srv_impl + "' is already registered and does not have the "
                                                                     "same objects.",
                info.object_impl.empty() || it_find != info.object_impl.end()
            );
        }
        else
        {
            core::mt::upgrade_to_write_lock upgrade(lock);
            info.object_impl.push_back(o_impl);
        }
    }
}

//-----------------------------------------------------------------------------

void factory::print_info_map(const srv_reg_container_t& _src)
{
    // not thread-safe

    //Print information
    for(const auto& srv_reg : _src)
    {
        SIGHT_DEBUG(" Service name = " << srv_reg.first);
        SIGHT_DEBUG("  - type   = " << srv_reg.second.service_type);

#if SIGHT_DEBUG_ENABLED
        std::size_t objNum = 0;
        for(const auto& objImpl : srvReg.second.objectImpl)
        {
            SIGHT_DEBUG("  - object " << objNum++ << " = " << objImpl)
        }
#endif

        SIGHT_DEBUG_IF("  - module = " << srv_reg.second.module->identifier(), srv_reg.second.module);
        SIGHT_DEBUG_IF("  - module = ( no module registered )", !srv_reg.second.module);

        SIGHT_DEBUG_IF(
            "  - name after creation = "
            << srv_reg.second.factory()->get_classname(),
            srv_reg.second.factory
        );
        SIGHT_DEBUG_IF("  - name after creation = ( no factory registered )", !srv_reg.second.factory);
    }
}

//-----------------------------------------------------------------------------

void factory::check_services_not_declared_in_plugin_xml() const
{
    // not thread-safe
    //Print information
    for(const auto& srv_reg : m_srv_impl_to_srv_info)
    {
        if(!srv_reg.second.module)
        {
            SIGHT_WARN("Service " << srv_reg.first << " is not declared/found in a plugin.xml.");
        }
    }
}

//-----------------------------------------------------------------------------

void factory::clear_factory()
{
    core::mt::write_lock lock(m_srv_impl_to_srv_info_mutex);
    m_srv_impl_to_srv_info.clear();
}

//-----------------------------------------------------------------------------

std::vector<std::string> factory::get_implementation_id_from_object_and_type(
    const std::string& _object,
    const std::string& _type
) const
{
    const std::string object = core::runtime::filter_id(_object);
    const std::string type   = core::runtime::filter_id(_type);

    std::vector<std::string> service_impl;

    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    for(const auto& srv : m_srv_impl_to_srv_info)
    {
        const service_info& srv_info = srv.second;
        for(const auto& oimpl : srv_info.object_impl)
        {
            if(srv_info.service_type == type
               && (oimpl == object || oimpl == "sight::data::object"))
            {
                service_impl.push_back(srv.first);
                break;
            }
        }
    }

    return service_impl;
}

//-----------------------------------------------------------------------------

std::string factory::get_default_implementation_id_from_object_and_type(
    const std::string& _object,
    const std::string& _type
) const
{
    const std::string object = core::runtime::filter_id(_object);
    const std::string type   = core::runtime::filter_id(_type);
    SIGHT_ASSERT("This case is not managed ", object != "sight::data::object");

    std::string service_impl;
#ifdef _DEBUG
    bool generic_impl_is_found = false;
#endif
    bool specific_impl_is_found = false;

    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    for(const auto& srv : m_srv_impl_to_srv_info)
    {
        const service_info& srv_info = srv.second;
        if(srv_info.service_type == type)
        {
            for(const auto& oimpl : srv_info.object_impl)
            {
                if(oimpl == object)
                {
                    SIGHT_ASSERT(
                        "Method has already found a specific ("
                        << service_impl << " != " << srv.first
                        << ") service for the object " << oimpl << ".",
                        !specific_impl_is_found
                    );

                    specific_impl_is_found = true;
                    service_impl           = srv.first;
                    break;
                }

                if(oimpl == "sight::data::object")
                {
                    SIGHT_ASSERT(
                        "Method has already found a generic service for the object ("
                        << oimpl << ").",
                        !generic_impl_is_found
                    );
#ifdef _DEBUG
                    generic_impl_is_found = true;
#endif
                    if(!specific_impl_is_found)
                    {
                        service_impl = srv.first;
                        break;
                    }
                }
            }
        }
    }

    SIGHT_ASSERT("A default implementation is not found for this type of service " << type, !service_impl.empty());

    return service_impl;
}

//-----------------------------------------------------------------------------

const std::vector<std::string>& factory::get_service_objects(const std::string& _srv_impl) const
{
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);
    std::string obj_impl;
    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);
    SIGHT_ASSERT("The service " << srv_impl << " is not found.", iter != m_srv_impl_to_srv_info.end());
    return iter->second.object_impl;
}

//-----------------------------------------------------------------------------

std::string factory::get_service_description(const std::string& _srv_impl) const
{
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);
    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);
    SIGHT_ASSERT("The service " << srv_impl << " is not found.", iter != m_srv_impl_to_srv_info.end());
    return iter->second.desc;
}

//-----------------------------------------------------------------------------

std::string factory::get_service_tags(const std::string& _srv_impl) const
{
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);
    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);
    SIGHT_ASSERT("The service " << srv_impl << " is not found.", iter != m_srv_impl_to_srv_info.end());
    return iter->second.tags;
}

//-----------------------------------------------------------------------------

bool factory::check_service_validity(const std::string& _object, const std::string& _srv_impl) const
{
    const std::string object   = core::runtime::filter_id(_object);
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);
    bool is_valid              = true;
    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);
    is_valid &= (iter != m_srv_impl_to_srv_info.end());
    if(is_valid)
    {
        const service_info& srv_info = iter->second;

        is_valid = false;
        for(const auto& oimpl : srv_info.object_impl)
        {
            if(oimpl == "sight::data::object" || oimpl == object)
            {
                is_valid = true;
                break;
            }
        }
    }

    return is_valid;
}

//-----------------------------------------------------------------------------

bool factory::support(const std::string& _object, const std::string& _srv_type, const std::string& _srv_impl) const
{
    const std::string object   = core::runtime::filter_id(_object);
    const std::string srv_type = core::runtime::filter_id(_srv_type);
    const std::string srv_impl = core::runtime::filter_id(_srv_impl);

    bool is_supported = true;
    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    auto iter = m_srv_impl_to_srv_info.find(srv_impl);
    is_supported &= (iter != m_srv_impl_to_srv_info.end());
    if(is_supported)
    {
        const service_info& srv_info = iter->second;

        is_supported = false;
        if(srv_info.service_type == srv_type)
        {
            for(const auto& oimpl : srv_info.object_impl)
            {
                if(oimpl == "sight::data::object" || oimpl == object)
                {
                    is_supported = true;
                    break;
                }
            }
        }
    }

    return is_supported;
}

//-----------------------------------------------------------------------------

bool factory::support(const std::string& _object, const std::string& _srv_type)
{
    const std::string object   = core::runtime::filter_id(_object);
    const std::string srv_type = core::runtime::filter_id(_srv_type);

    bool is_supported = false;
    support_map_t::key_type key(object, srv_type);
    core::mt::read_to_write_lock support_map_lock(m_support_map_mutex);
    support_map_t::const_iterator iter = m_support_map.find(key);
    if(iter != m_support_map.end())
    {
        is_supported = iter->second;
    }
    else
    {
        core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
        for(const auto& srv : m_srv_impl_to_srv_info)
        {
            const service_info& srv_info = srv.second;

            if(srv_info.service_type == srv_type)
            {
                for(const auto& oimpl : srv_info.object_impl)
                {
                    if(oimpl == "sight::data::object" || oimpl == object)
                    {
                        is_supported = true;
                        break;
                    }
                }
            }
        }

        core::mt::upgrade_to_write_lock upgrade(support_map_lock);
        m_support_map.insert(support_map_t::value_type(key, is_supported));
    }

    return is_supported;
}

//-----------------------------------------------------------------------------

factory::key_vector_t factory::get_factory_keys() const
{
    core::mt::read_lock lock(m_srv_impl_to_srv_info_mutex);
    key_vector_t vect_keys;
    std::transform(
        m_srv_impl_to_srv_info.begin(),
        m_srv_impl_to_srv_info.end(),
        std::back_inserter(vect_keys),
        [](const auto& _e){return _e.first;});
    return vect_keys;
}

//-----------------------------------------------------------------------------

} // namespace sight::service::extension

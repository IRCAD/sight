/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "config_test.hpp"

#include <core/ptree.hpp>
#include <core/runtime/path.hpp>

#include <data/extension/config.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::config_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void config_test::setUp()
{
    if(!m_data_module || !m_ut_config_module)
    {
        // Initialize the runtime if not already done
        core::runtime::init();
    }

    if(!m_data_module)
    {
        core::runtime::add_modules(core::runtime::get_resource_file_path("module_data"));
        m_data_module = core::runtime::load_module("sight::module::data");
    }

    CPPUNIT_ASSERT(m_data_module);

    if(!m_ut_config_module)
    {
        core::runtime::add_modules(core::runtime::get_resource_file_path("data_ut"));
        m_ut_config_module = core::runtime::load_module("ut_config");
    }

    CPPUNIT_ASSERT(m_ut_config_module);
}

//------------------------------------------------------------------------------

void config_test::tearDown()
{
    if(m_ut_config_module)
    {
        if(auto config_ext = sight::data::extension::config::get(); config_ext)
        {
            config_ext->clear_registry();
        }

        const auto& ut_plugin_id = m_ut_config_module->identifier();
        core::runtime::unload_module(ut_plugin_id);

        CPPUNIT_ASSERT(!m_ut_config_module->is_started());
        m_ut_config_module.reset();
    }

    if(m_data_module)
    {
        const auto& data_plugin_id = m_data_module->identifier();
        core::runtime::unload_module(data_plugin_id);

        CPPUNIT_ASSERT(!m_data_module->is_started());
        m_data_module.reset();
    }

    core::runtime::shutdown();
}

//------------------------------------------------------------------------------

void config_test::basic()
{
    auto config_ext = sight::data::extension::config::get();
    config_ext->parse_plugin_infos();

    static const std::string s_CONFIG_ID("ut_config");
    static const std::string s_DATA_CLASS("sight::data::map");

    const auto& config_names = config_ext->get_all_config_for_data(s_DATA_CLASS);
    CPPUNIT_ASSERT(std::ranges::find(config_names, s_CONFIG_ID) != config_names.end());

    const auto& config = config_ext->get_data_config(s_CONFIG_ID, s_DATA_CLASS);
    CPPUNIT_ASSERT(!config.empty());

    for(const auto& item : config)
    {
        if(item.first == "item")
        {
            if(item.second.get<std::string>("<xmlattr>.key") == "integer")
            {
                CPPUNIT_ASSERT_EQUAL(1, item.second.get<int>("<xmlattr>.value"));
            }
            else if(item.second.get<std::string>("<xmlattr>.key") == "string")
            {
                CPPUNIT_ASSERT_EQUAL(std::string("abc"), item.second.get<std::string>("<xmlattr>.value"));
            }
            else if(item.second.get<std::string>("<xmlattr>.key") == "boolean")
            {
                CPPUNIT_ASSERT(item.second.get<bool>("<xmlattr>.value"));
            }
            else if(item.second.get<std::string>("<xmlattr>.key") == "ivec3")
            {
                const auto value = item.second.get<std::string>("<xmlattr>.value");
                CPPUNIT_ASSERT_EQUAL(std::string("0;0;0"), value);
            }
            else if(item.second.get<std::string>("<xmlattr>.key") == "dvec3")
            {
                const auto value = item.second.get<std::string>("<xmlattr>.value");
                CPPUNIT_ASSERT_EQUAL(std::string("1.0;1.0;1.0"), value);
            }
            else if(item.second.get<std::string>("<xmlattr>.key") == "transfer_function")
            {
                const auto& transfer_function = item.second.get_child("object");

                const auto type = transfer_function.get<std::string>("<xmlattr>.type");
                CPPUNIT_ASSERT_EQUAL(std::string("sight::data::transfer_function"), type);

                CPPUNIT_ASSERT_EQUAL(std::string("CT-GreyLevel"), transfer_function.get<std::string>("name"));
                CPPUNIT_ASSERT(!transfer_function.get_child("colors").empty());
                const auto& colors = transfer_function.get_child("colors");
                CPPUNIT_ASSERT_EQUAL(false, colors.get<bool>("<xmlattr>.is_clamped"));
            }
        }
    }
}

} // namespace sight::data::ut

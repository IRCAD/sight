/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <core/ptree.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/extension/config.hpp>

#include <doctest/doctest.h>

namespace
{

class fixture
{
public:

    fixture()
    {
        // Initialize the runtime if not already done
        sight::core::runtime::init();

        sight::core::runtime::add_modules(sight::core::runtime::get_resource_file_path("module_data"));
        m_data_module = sight::core::runtime::load_module("sight::module::data");

        CHECK(m_data_module);

        sight::core::runtime::add_modules(sight::core::runtime::get_resource_file_path("data_ut"));
        m_ut_config_module = sight::core::runtime::load_module("ut_config");

        CHECK(m_ut_config_module);
    }

    ~fixture()
    {
        if(auto config_ext = sight::data::extension::config::get(); config_ext)
        {
            config_ext->clear_registry();
        }

        const auto& ut_plugin_id = m_ut_config_module->identifier();
        sight::core::runtime::unload_module(ut_plugin_id);

        CHECK(!m_ut_config_module->is_started());
        m_ut_config_module.reset();

        const auto& data_plugin_id = m_data_module->identifier();
        sight::core::runtime::unload_module(data_plugin_id);

        CHECK(!m_data_module->is_started());
        m_data_module.reset();

        sight::core::runtime::shutdown();
    }

    std::shared_ptr<sight::core::runtime::module> m_data_module;
    std::shared_ptr<sight::core::runtime::module> m_ut_config_module;
};

} // namespace

TEST_SUITE("sight::data::extension::config")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "basic")
    {
        auto config_ext = sight::data::extension::config::get();
        config_ext->parse_plugin_infos();

        static const std::string s_CONFIG_ID("ut_config");
        static const std::string s_DATA_CLASS("sight::data::map");

        const auto& config_names = config_ext->get_all_config_for_data(s_DATA_CLASS);
        CHECK(std::ranges::find(config_names, s_CONFIG_ID) != config_names.end());

        const auto& config = config_ext->get_data_config(s_CONFIG_ID, s_DATA_CLASS);
        CHECK(!config.empty());

        for(const auto& item : config)
        {
            if(item.first == "item")
            {
                if(item.second.get<std::string>("<xmlattr>.key") == "integer")
                {
                    CHECK_EQ(1, item.second.get<int>("<xmlattr>.value"));
                }
                else if(item.second.get<std::string>("<xmlattr>.key") == "string")
                {
                    CHECK_EQ(std::string("abc"), item.second.get<std::string>("<xmlattr>.value"));
                }
                else if(item.second.get<std::string>("<xmlattr>.key") == "boolean")
                {
                    CHECK(item.second.get<bool>("<xmlattr>.value"));
                }
                else if(item.second.get<std::string>("<xmlattr>.key") == "ivec3")
                {
                    const auto value = item.second.get<std::string>("<xmlattr>.value");
                    CHECK_EQ(std::string("0;0;0"), value);
                }
                else if(item.second.get<std::string>("<xmlattr>.key") == "dvec3")
                {
                    const auto value = item.second.get<std::string>("<xmlattr>.value");
                    CHECK_EQ(std::string("1.0;1.0;1.0"), value);
                }
                else if(item.second.get<std::string>("<xmlattr>.key") == "transfer_function")
                {
                    const auto& transfer_function = item.second.get_child("object");

                    const auto type = transfer_function.get<std::string>("<xmlattr>.type");
                    CHECK_EQ(std::string("sight::data::transfer_function"), type);

                    CHECK_EQ(std::string("CT-GreyLevel"), transfer_function.get<std::string>("name"));
                    CHECK(!transfer_function.get_child("colors").empty());
                    const auto& colors = transfer_function.get_child("colors");
                    CHECK_EQ(false, colors.get<bool>("<xmlattr>.is_clamped"));
                }
            }
        }
    }
} // TEST_SUITE("sight::data")

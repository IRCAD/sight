/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <activity/launcher.hpp>

#include <doctest/doctest.h>

#include <map>

namespace
{

class launcher_test final : public sight::activity::launcher
{
public:

    using launcher::parse_configuration;

    //------------------------------------------------------------------------------

    [[nodiscard]] const parameters_t& parameters() const
    {
        return m_parameters;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] const value_parameters_t& value_parameters() const
    {
        return m_value_parameters;
    }
};

} // namespace

TEST_SUITE("sight::activity::launcher")
{
    TEST_CASE("value_inout_parsing")
    {
        launcher_test test_launcher;

        sight::activity::launcher::configuration_t config;
        sight::activity::launcher::configuration_t inout_cfg;

        inout_cfg.put("<xmlattr>.group", "data");

        sight::activity::launcher::configuration_t key_runtime;
        key_runtime.put("<xmlattr>.name", "activity_set");
        key_runtime.put("<xmlattr>.uid", "activity_set_uid");
        inout_cfg.add_child("key", key_runtime);

        sight::activity::launcher::configuration_t key_literal;
        key_literal.put("<xmlattr>.name", "position");
        key_literal.put("<xmlattr>.value", "9;8;7");
        inout_cfg.add_child("key", key_literal);

        sight::activity::launcher::configuration_t key_optional;
        key_optional.put("<xmlattr>.name", "optional_obj");
        key_optional.put("<xmlattr>.uid", "optional_obj_uid");
        key_optional.put("<xmlattr>.optional", true);
        inout_cfg.add_child("key", key_optional);

        config.add_child("inout", inout_cfg);

        test_launcher.parse_configuration(config, {"runtime_activity_set_uid"});

        std::map<std::string, std::string> parameters_map;
        for(const auto& parameter : test_launcher.parameters())
        {
            parameters_map[parameter.replace] = parameter.by;
        }

        CHECK_EQ(std::size_t(3), parameters_map.size());
        CHECK_EQ(std::string("runtime_activity_set_uid"), parameters_map.at("activity_set"));
        CHECK_EQ(std::string("9;8;7"), parameters_map.at("position"));
        CHECK_EQ(std::string("optional_obj_uid"), parameters_map.at("optional_obj"));

        CHECK_EQ(std::size_t(1), test_launcher.value_parameters().size());
        CHECK_EQ(std::string("9;8;7"), test_launcher.value_parameters().at("position"));
    }
} // end TEST_SUITE

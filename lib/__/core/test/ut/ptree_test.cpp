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

#include <core/ptree.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::ptree")
{
    TEST_CASE("get_value")
    {
        SUBCASE("boolean values")
        {
            boost::property_tree::ptree config;
            config.put("test.true", "true");
            config.put("test.false", "false");

            bool value = sight::core::ptree::get_value(config, "test.true", true);
            CHECK_EQ(true, value);

            value = sight::core::ptree::get_value(config, "test.false", true);
            CHECK_EQ(false, value);

            value = sight::core::ptree::get_value(config, "not_set!!!", true);
            CHECK_EQ(true, value);

            value = sight::core::ptree::get_value(config, "not_set!!!", false);
            CHECK_EQ(false, value);
        }

        SUBCASE("numeric and string values")
        {
            boost::property_tree::ptree config;
            config.put("test.int", "42");
            config.put("test.real", "2.34");
            config.put("test.string", "digits");

            CHECK_THROWS_AS(sight::core::ptree::get_value<int>(config, "test.real", 23), sight::core::exception);
            CHECK_THROWS_AS(sight::core::ptree::get_value<int>(config, "test.string", 23), sight::core::exception);
            CHECK_THROWS_AS(sight::core::ptree::get_value<float>(config, "test.string", 23.F), sight::core::exception);
            CHECK_THROWS_AS(sight::core::ptree::get_value<double>(config, "test.string", 23.), sight::core::exception);

            int value = sight::core::ptree::get_value(config, "test.int", 23);
            CHECK_EQ(42, value);

            value = sight::core::ptree::get_value(config, "not_set!!!", 23);
            CHECK_EQ(23, value);

            double real = sight::core::ptree::get_value(config, "test.int", 23);
            CHECK_EQ(42., real);

            real = sight::core::ptree::get_value(config, "test.real", 42.3);
            CHECK_EQ(doctest::Approx(2.34).epsilon(0.0001), real);

            std::string str = sight::core::ptree::get_value(config, "test.string", std::string("foo"));
            CHECK_EQ(std::string("digits"), str);

            str = sight::core::ptree::get_value(config, "test.missing_string", std::string("foo"));
            CHECK_EQ(std::string("foo"), str);
        }
    }

    TEST_CASE("get_and_deprecate")
    {
        SUBCASE("new key exists")
        {
            boost::property_tree::ptree config;
            config.put("test.new", "1");

            int value = sight::core::ptree::get_and_deprecate(config, "test.new", "test.old", "X.Y", 0);
            CHECK_EQ(1, value);

            value = sight::core::ptree::get_and_deprecate(config, "test.miss", "test.old", "X.Y", 0);
            CHECK_EQ(0, value);
        }

        SUBCASE("deprecated key exists")
        {
            boost::property_tree::ptree config;
            config.put("test.old", "2");

            int value = sight::core::ptree::get_and_deprecate(config, "test.new", "test.old", "X.Y", 0);
            CHECK_EQ(2, value);

            value = sight::core::ptree::get_and_deprecate(config, "test.new", "test.miss", "X.Y", 0);
            CHECK_EQ(0, value);
        }
    }
}

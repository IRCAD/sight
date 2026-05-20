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

#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/string.hpp>
#include <data/validator/greater.hpp>
#include <data/validator/less.hpp>

#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::validator::less")
{
//------------------------------------------------------------------------------

    TEST_CASE("real_below_threshold")
    {
        auto validator = std::make_shared<sight::data::validator::less>();

        boost::property_tree::ptree config;
        config.put("value", "10.5");
        validator->configure(config);

        auto real_obj     = std::make_shared<sight::data::real>(5.0);
        const auto result = validator->validate(real_obj);

        CHECK_EQ(result.first, true);
        CHECK(result.second.find("less than") != std::string::npos);
    }

    TEST_CASE("real_above_threshold")
    {
        auto validator = std::make_shared<sight::data::validator::less>();

        boost::property_tree::ptree config;
        config.put("value", "10.5");
        validator->configure(config);

        auto real_obj     = std::make_shared<sight::data::real>(15.0);
        const auto result = validator->validate(real_obj);

        CHECK_EQ(result.first, false);
        CHECK(result.second.find("not less than") != std::string::npos);
    }

    TEST_CASE("real_equal_to_threshold")
    {
        auto validator = std::make_shared<sight::data::validator::less>();

        boost::property_tree::ptree config;
        config.put("value", "10.5");
        validator->configure(config);

        auto real_obj     = std::make_shared<sight::data::real>(10.5);
        const auto result = validator->validate(real_obj);

        CHECK_EQ(result.first, false);
        CHECK(result.second.find("not less than") != std::string::npos);
    }

    TEST_CASE("integer_below_threshold")
    {
        auto validator = std::make_shared<sight::data::validator::less>();

        boost::property_tree::ptree config;
        config.put("value", "10");
        validator->configure(config);

        auto int_obj      = std::make_shared<sight::data::integer>(5);
        const auto result = validator->validate(int_obj);

        CHECK_EQ(result.first, true);
        CHECK(result.second.find("less than") != std::string::npos);
    }

    TEST_CASE("integer_above_threshold")
    {
        auto validator = std::make_shared<sight::data::validator::less>();

        boost::property_tree::ptree config;
        config.put("value", "10");
        validator->configure(config);

        auto int_obj      = std::make_shared<sight::data::integer>(15);
        const auto result = validator->validate(int_obj);

        CHECK_EQ(result.first, false);
        CHECK(result.second.find("not less than") != std::string::npos);
    }

    TEST_CASE("invalid_data_type")
    {
        auto validator = std::make_shared<sight::data::validator::less>();

        boost::property_tree::ptree config;
        config.put("value", "10");
        validator->configure(config);

        auto string_obj   = std::make_shared<sight::data::string>("test");
        const auto result = validator->validate(string_obj);

        CHECK_EQ(result.first, false);
        CHECK(result.second.find("not sight::data::real or sight::data::integer") != std::string::npos);
    }

    TEST_CASE("negative_numbers")
    {
        auto less_validator = std::make_shared<sight::data::validator::less>();

        boost::property_tree::ptree config;
        config.put("value", "-5");
        less_validator->configure(config);

        auto real_obj_neg = std::make_shared<sight::data::real>(-10.0);
        auto real_obj_pos = std::make_shared<sight::data::real>(0.0);

        const auto less_result_neg = less_validator->validate(real_obj_neg);
        const auto less_result_pos = less_validator->validate(real_obj_pos);

        CHECK_EQ(less_result_neg.first, true);  // -10 < -5
        CHECK_EQ(less_result_pos.first, false); // 0 is not < -5
    }
}

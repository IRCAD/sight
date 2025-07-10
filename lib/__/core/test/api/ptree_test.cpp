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

#include "ptree_test.hpp"

#include <core/ptree.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::ptree_test);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void ptree_test::setUp()
{
}

//------------------------------------------------------------------------------

void ptree_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ptree_test::get_value()
{
    {
        boost::property_tree::ptree config;
        config.put("test.true", "true");
        config.put("test.false", "false");
        bool value = core::ptree::get_value(config, "test.true", true);
        CPPUNIT_ASSERT_EQUAL(true, value);
        value = core::ptree::get_value(config, "test.false", true);
        CPPUNIT_ASSERT_EQUAL(false, value);
        value = core::ptree::get_value(config, "not_set!!!", true);
        CPPUNIT_ASSERT_EQUAL(true, value);
        value = core::ptree::get_value(config, "not_set!!!", false);
        CPPUNIT_ASSERT_EQUAL(false, value);
    }

    {
        boost::property_tree::ptree config;
        config.put("test.int", "42");
        config.put("test.real", "2.34");
        config.put("test.string", "digits");

        CPPUNIT_ASSERT_THROW(core::ptree::get_value<int>(config, "test.real", 23), core::exception);
        CPPUNIT_ASSERT_THROW(core::ptree::get_value<int>(config, "test.string", 23), core::exception);
        CPPUNIT_ASSERT_THROW(core::ptree::get_value<float>(config, "test.string", 23.F), core::exception);
        CPPUNIT_ASSERT_THROW(core::ptree::get_value<double>(config, "test.string", 23.), core::exception);

        int value = core::ptree::get_value(config, "test.int", 23);
        CPPUNIT_ASSERT_EQUAL(42, value);
        value = core::ptree::get_value(config, "not_set!!!", 23);
        CPPUNIT_ASSERT_EQUAL(23, value);

        double real = core::ptree::get_value(config, "test.int", 23);
        CPPUNIT_ASSERT_EQUAL(42., real);
        real = core::ptree::get_value(config, "test.real", 42.3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(2.34, real, 0.0001);

        std::string str = core::ptree::get_value(config, "test.string", std::string("foo"));
        CPPUNIT_ASSERT_EQUAL(std::string("digits"), str);
        str = core::ptree::get_value(config, "test.missing_string", std::string("foo"));
        CPPUNIT_ASSERT_EQUAL(std::string("foo"), str);
    }
}

//------------------------------------------------------------------------------

void ptree_test::get_and_deprecate()
{
    {
        boost::property_tree::ptree config;
        config.put("test.new", "1");
        int value = core::ptree::get_and_deprecate(config, "test.new", "test.old", "X.Y", 0);
        CPPUNIT_ASSERT_EQUAL(1, value);
        value = core::ptree::get_and_deprecate(config, "test.miss", "test.old", "X.Y", 0);
        CPPUNIT_ASSERT_EQUAL(0, value);
    }
    {
        boost::property_tree::ptree config;
        config.put("test.old", "2");
        int value = core::ptree::get_and_deprecate(config, "test.new", "test.old", "X.Y", 0);
        CPPUNIT_ASSERT_EQUAL(2, value);
        value = core::ptree::get_and_deprecate(config, "test.new", "test.miss", "X.Y", 0);
        CPPUNIT_ASSERT_EQUAL(0, value);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::core::ut

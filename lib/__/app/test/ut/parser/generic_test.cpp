/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "generic_test.hpp"

#include <app/parser/boolean.hpp>
#include <app/parser/color.hpp>
#include <app/parser/dvec2.hpp>
#include <app/parser/dvec3.hpp>
#include <app/parser/dvec4.hpp>
#include <app/parser/integer.hpp>
#include <app/parser/ivec2.hpp>
#include <app/parser/ivec3.hpp>
#include <app/parser/ivec4.hpp>
#include <app/parser/real.hpp>
#include <app/parser/string.hpp>

#include <data/boolean.hpp>
#include <data/color.hpp>
#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::parser::ut::generic_test);

namespace sight::app::parser::ut
{

//------------------------------------------------------------------------------

void generic_test::boolean_test()
{
    auto object = std::make_shared<data::boolean>();
    CPPUNIT_ASSERT_EQUAL(false, object->value());
    parser::boolean parser;
    CPPUNIT_ASSERT(parser.is_a("sight::app::parser::boolean"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));

    boost::property_tree::ptree bad_ptree;
    bad_ptree.put("<xmlattr>.value", "89");
    service::object_parser::objects_t sub_objects;
    CPPUNIT_ASSERT_THROW(parser.parse(bad_ptree, object, sub_objects), data::exception);

    boost::property_tree::ptree ptree;
    ptree.put("<xmlattr>.value", "true");
    parser.parse(ptree, object, sub_objects);
    CPPUNIT_ASSERT_EQUAL(true, object->value());
}

//------------------------------------------------------------------------------

void generic_test::color_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("<xmlattr>.value", "#0077FFA3");
    auto object = std::make_shared<data::color>();

    parser::color parser;
    CPPUNIT_ASSERT(parser.is_a("sight::app::parser::color"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
    service::object_parser::objects_t sub_objects;
    parser.parse(ptree, object, sub_objects);
    CPPUNIT_ASSERT_EQUAL(0.F, object->red());
    CPPUNIT_ASSERT_EQUAL(119.F / 255.F, object->green());
    CPPUNIT_ASSERT_EQUAL(1.F, object->blue());
    CPPUNIT_ASSERT_EQUAL(163.F / 255.F, object->alpha());
}

//------------------------------------------------------------------------------

void generic_test::integer_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("<xmlattr>.value", "10");
    auto object = std::make_shared<data::integer>();
    CPPUNIT_ASSERT_EQUAL(std::int64_t(0), object->value());

    parser::integer parser;
    CPPUNIT_ASSERT(parser.is_a("sight::app::parser::integer"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
    service::object_parser::objects_t sub_objects;
    parser.parse(ptree, object, sub_objects);
    CPPUNIT_ASSERT_EQUAL(std::int64_t(10), object->value());
}

//------------------------------------------------------------------------------

void generic_test::real_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("value", "12.1234");
    auto object = std::make_shared<data::real>();
    parser::real parser;
    CPPUNIT_ASSERT(parser.is_a("sight::app::parser::real"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
    service::object_parser::objects_t sub_objects;
    parser.parse(ptree, object, sub_objects);
    CPPUNIT_ASSERT_EQUAL(12.1234, object->value());
}

//------------------------------------------------------------------------------

void generic_test::string_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("<xmlattr>.value", "this is a string");
    auto object = std::make_shared<data::string>();
    parser::string parser;
    CPPUNIT_ASSERT(parser.is_a("sight::app::parser::string"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
    CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
    service::object_parser::objects_t sub_objects;
    parser.parse(ptree, object, sub_objects);
    CPPUNIT_ASSERT_EQUAL(std::string("this is a string"), object->value());
}

//------------------------------------------------------------------------------

void generic_test::vec_test()
{
    {
        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "12.12;45.98");
        auto object = std::make_shared<data::dvec2>();
        parser::dvec2 parser;
        CPPUNIT_ASSERT(parser.is_a("sight::app::parser::dvec2"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
        service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CPPUNIT_ASSERT_EQUAL(12.12, object->value()[0]);
        CPPUNIT_ASSERT_EQUAL(45.98, object->value()[1]);
    }
    {
        auto object = std::make_shared<data::ivec2>();
        parser::ivec2 parser;
        CPPUNIT_ASSERT(parser.is_a("sight::app::parser::ivec2"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));

        service::object_parser::objects_t sub_objects;
        boost::property_tree::ptree bad_ptree;
        bad_ptree.put("<xmlattr>.value", "12.12;45.98");
        CPPUNIT_ASSERT_THROW(parser.parse(bad_ptree, object, sub_objects), boost::bad_lexical_cast);

        boost::property_tree::ptree ptree;
        ptree.put("value", "789;874");

        parser.parse(ptree, object, sub_objects);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(789), object->value()[0]);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(874), object->value()[1]);
    }
    {
        auto object = std::make_shared<data::dvec3>();
        parser::dvec3 parser;
        CPPUNIT_ASSERT(parser.is_a("sight::app::parser::dvec3"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));

        service::object_parser::objects_t sub_objects;
        boost::property_tree::ptree bad_ptree;
        bad_ptree.put("<xmlattr>.value", "12.12;45.98");
        CPPUNIT_ASSERT_THROW(parser.parse(bad_ptree, object, sub_objects), sight::data::exception);

        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "-12.4;45.98;45");
        parser.parse(ptree, object, sub_objects);
        CPPUNIT_ASSERT_EQUAL(-12.4, object->value()[0]);
        CPPUNIT_ASSERT_EQUAL(45.98, object->value()[1]);
        CPPUNIT_ASSERT_EQUAL(45., object->value()[2]);
    }
    {
        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "12;45;98");
        auto object = std::make_shared<data::ivec3>();
        parser::ivec3 parser;
        CPPUNIT_ASSERT(parser.is_a("sight::app::parser::ivec3"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
        service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(12), object->value()[0]);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(45), object->value()[1]);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(98), object->value()[2]);
    }
    {
        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "12.12;45.98;45;-89.3");
        auto object = std::make_shared<data::dvec4>();
        parser::dvec4 parser;
        CPPUNIT_ASSERT(parser.is_a("sight::app::parser::dvec4"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
        service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CPPUNIT_ASSERT_EQUAL(12.12, object->value()[0]);
        CPPUNIT_ASSERT_EQUAL(45.98, object->value()[1]);
        CPPUNIT_ASSERT_EQUAL(45., object->value()[2]);
        CPPUNIT_ASSERT_EQUAL(-89.3, object->value()[3]);
    }
    {
        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "789; 874; 41 ; -78");
        auto object = std::make_shared<data::ivec4>();
        parser::ivec4 parser;
        CPPUNIT_ASSERT(parser.is_a("sight::app::parser::ivec4"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::app::parser::generic"));
        CPPUNIT_ASSERT(parser.is_type_of("sight::service::object_parser"));
        service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(789), object->value()[0]);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(874), object->value()[1]);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(41), object->value()[2]);
        CPPUNIT_ASSERT_EQUAL(std::int64_t(-78), object->value()[3]);
    }
}

} // namespace sight::app::parser::ut

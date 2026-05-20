/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::app::parser::Generic")
{
//------------------------------------------------------------------------------

    TEST_CASE("boolean")
    {
        auto object = std::make_shared<sight::data::boolean>();
        CHECK_EQ(false, object->value());
        sight::app::parser::boolean parser;
        CHECK(parser.is_a("sight::app::parser::boolean"));
        CHECK(parser.is_type_of("sight::app::parser::generic"));
        CHECK(parser.is_type_of("sight::service::object_parser"));

        boost::property_tree::ptree bad_ptree;
        bad_ptree.put("<xmlattr>.value", "89");
        sight::service::object_parser::objects_t sub_objects;
        CHECK_THROWS_AS(parser.parse(bad_ptree, object, sub_objects), sight::data::exception);

        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "true");
        parser.parse(ptree, object, sub_objects);
        CHECK_EQ(true, object->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE("color")
    {
        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "#0077FFA3");
        auto object = std::make_shared<sight::data::color>();

        sight::app::parser::color parser;
        CHECK(parser.is_a("sight::app::parser::color"));
        CHECK(parser.is_type_of("sight::app::parser::generic"));
        CHECK(parser.is_type_of("sight::service::object_parser"));
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CHECK_EQ(0.F, object->red());
        CHECK_EQ(119.F / 255.F, object->green());
        CHECK_EQ(1.F, object->blue());
        CHECK_EQ(163.F / 255.F, object->alpha());
    }

//------------------------------------------------------------------------------

    TEST_CASE("integer")
    {
        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "10");
        auto object = std::make_shared<sight::data::integer>();
        CHECK_EQ(std::int64_t(0), object->value());

        sight::app::parser::integer parser;
        CHECK(parser.is_a("sight::app::parser::integer"));
        CHECK(parser.is_type_of("sight::app::parser::generic"));
        CHECK(parser.is_type_of("sight::service::object_parser"));
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CHECK_EQ(std::int64_t(10), object->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE("real")
    {
        boost::property_tree::ptree ptree;
        ptree.put("value", "12.1234");
        auto object = std::make_shared<sight::data::real>();
        sight::app::parser::real parser;
        CHECK(parser.is_a("sight::app::parser::real"));
        CHECK(parser.is_type_of("sight::app::parser::generic"));
        CHECK(parser.is_type_of("sight::service::object_parser"));
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CHECK_EQ(12.1234, object->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE("string")
    {
        boost::property_tree::ptree ptree;
        ptree.put("<xmlattr>.value", "this is a string");
        auto object = std::make_shared<sight::data::string>();
        sight::app::parser::string parser;
        CHECK(parser.is_a("sight::app::parser::string"));
        CHECK(parser.is_type_of("sight::app::parser::generic"));
        CHECK(parser.is_type_of("sight::service::object_parser"));
        sight::service::object_parser::objects_t sub_objects;
        parser.parse(ptree, object, sub_objects);
        CHECK_EQ(std::string("this is a string"), object->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE("vec")
    {
        {
            boost::property_tree::ptree ptree;
            ptree.put("<xmlattr>.value", "12.12;45.98");
            auto object = std::make_shared<sight::data::dvec2>();
            sight::app::parser::dvec2 parser;
            CHECK(parser.is_a("sight::app::parser::dvec2"));
            CHECK(parser.is_type_of("sight::app::parser::generic"));
            CHECK(parser.is_type_of("sight::service::object_parser"));
            sight::service::object_parser::objects_t sub_objects;
            parser.parse(ptree, object, sub_objects);
            CHECK_EQ(12.12, object->value()[0]);
            CHECK_EQ(45.98, object->value()[1]);
        }
        {
            auto object = std::make_shared<sight::data::ivec2>();
            sight::app::parser::ivec2 parser;
            CHECK(parser.is_a("sight::app::parser::ivec2"));
            CHECK(parser.is_type_of("sight::app::parser::generic"));
            CHECK(parser.is_type_of("sight::service::object_parser"));

            sight::service::object_parser::objects_t sub_objects;
            boost::property_tree::ptree bad_ptree;
            bad_ptree.put("<xmlattr>.value", "12.12;45.98");
            CHECK_THROWS_AS(parser.parse(bad_ptree, object, sub_objects), boost::bad_lexical_cast);

            boost::property_tree::ptree ptree;
            ptree.put("value", "789;874");

            parser.parse(ptree, object, sub_objects);
            CHECK_EQ(std::int64_t(789), object->value()[0]);
            CHECK_EQ(std::int64_t(874), object->value()[1]);
        }
        {
            auto object = std::make_shared<sight::data::dvec3>();
            sight::app::parser::dvec3 parser;
            CHECK(parser.is_a("sight::app::parser::dvec3"));
            CHECK(parser.is_type_of("sight::app::parser::generic"));
            CHECK(parser.is_type_of("sight::service::object_parser"));

            sight::service::object_parser::objects_t sub_objects;
            boost::property_tree::ptree bad_ptree;
            bad_ptree.put("<xmlattr>.value", "12.12;45.98");
            CHECK_THROWS_AS(parser.parse(bad_ptree, object, sub_objects), sight::data::exception);

            boost::property_tree::ptree ptree;
            ptree.put("<xmlattr>.value", "-12.4;45.98;45");
            parser.parse(ptree, object, sub_objects);
            CHECK_EQ(-12.4, object->value()[0]);
            CHECK_EQ(45.98, object->value()[1]);
            CHECK_EQ(45., object->value()[2]);
        }
        {
            boost::property_tree::ptree ptree;
            ptree.put("<xmlattr>.value", "12;45;98");
            auto object = std::make_shared<sight::data::ivec3>();
            sight::app::parser::ivec3 parser;
            CHECK(parser.is_a("sight::app::parser::ivec3"));
            CHECK(parser.is_type_of("sight::app::parser::generic"));
            CHECK(parser.is_type_of("sight::service::object_parser"));
            sight::service::object_parser::objects_t sub_objects;
            parser.parse(ptree, object, sub_objects);
            CHECK_EQ(std::int64_t(12), object->value()[0]);
            CHECK_EQ(std::int64_t(45), object->value()[1]);
            CHECK_EQ(std::int64_t(98), object->value()[2]);
        }
        {
            boost::property_tree::ptree ptree;
            ptree.put("<xmlattr>.value", "12.12;45.98;45;-89.3");
            auto object = std::make_shared<sight::data::dvec4>();
            sight::app::parser::dvec4 parser;
            CHECK(parser.is_a("sight::app::parser::dvec4"));
            CHECK(parser.is_type_of("sight::app::parser::generic"));
            CHECK(parser.is_type_of("sight::service::object_parser"));
            sight::service::object_parser::objects_t sub_objects;
            parser.parse(ptree, object, sub_objects);
            CHECK_EQ(12.12, object->value()[0]);
            CHECK_EQ(45.98, object->value()[1]);
            CHECK_EQ(45., object->value()[2]);
            CHECK_EQ(-89.3, object->value()[3]);
        }
        {
            boost::property_tree::ptree ptree;
            ptree.put("<xmlattr>.value", "789; 874; 41 ; -78");
            auto object = std::make_shared<sight::data::ivec4>();
            sight::app::parser::ivec4 parser;
            CHECK(parser.is_a("sight::app::parser::ivec4"));
            CHECK(parser.is_type_of("sight::app::parser::generic"));
            CHECK(parser.is_type_of("sight::service::object_parser"));
            sight::service::object_parser::objects_t sub_objects;
            parser.parse(ptree, object, sub_objects);
            CHECK_EQ(std::int64_t(789), object->value()[0]);
            CHECK_EQ(std::int64_t(874), object->value()[1]);
            CHECK_EQ(std::int64_t(41), object->value()[2]);
            CHECK_EQ(std::int64_t(-78), object->value()[3]);
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE

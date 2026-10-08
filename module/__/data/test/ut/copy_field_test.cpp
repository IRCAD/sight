/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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
#include <data/mesh.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::copy_field")
{
//------------------------------------------------------------------------------

    TEST_CASE("copy_fields_to_inout")
    {
        auto srv = sight::service::add("sight::module::data::copy_field");

        // Create source object with fields
        auto source = std::make_shared<sight::data::mesh>();
        auto field1 = std::make_shared<sight::data::string>("field1_value");
        auto field2 = std::make_shared<sight::data::integer>(42);
        source->set_field("field1", field1);
        source->set_field("field2", field2);

        srv->set_input(source, "data.source");

        // Create target object
        auto target = std::make_shared<sight::data::mesh>();
        srv->set_inout(target, "data.target");

        boost::property_tree::ptree ptree;
        ptree.put("in", "");
        ptree.put("inout", "");
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        // Copy the fields
        CHECK_NOTHROW(srv->update().get());

        // Verify fields were copied
        CHECK_EQ(2, target->get_field_names().size());
        CHECK_NE(nullptr, target->get_field("field1"));
        CHECK_NE(nullptr, target->get_field("field2"));

        const auto copied_field1 = std::dynamic_pointer_cast<sight::data::string>(target->get_field("field1"));
        const auto copied_field2 = std::dynamic_pointer_cast<sight::data::integer>(target->get_field("field2"));
        CHECK_NE(nullptr, copied_field1);
        CHECK_NE(nullptr, copied_field2);

        // Verify they are the same objects (shallow copy)
        CHECK_EQ(field1->value(), copied_field1->value());
        CHECK_EQ(field2->value(), copied_field2->value());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy_fields_empty_source")
    {
        auto srv = sight::service::add("sight::module::data::copy_field");

        // Create empty source object
        auto source = std::make_shared<sight::data::mesh>();

        srv->set_input(source, "data.source");

        // Create target object with initial fields
        auto target        = std::make_shared<sight::data::mesh>();
        auto initial_field = std::make_shared<sight::data::string>("initial");
        target->set_field("initial", initial_field);

        srv->set_inout(target, "data.target");

        boost::property_tree::ptree ptree;
        ptree.put("in", "");
        ptree.put("inout", "");
        srv->set_config(ptree);
        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        // Copy (should clear existing fields)
        CHECK_NOTHROW(srv->update().get());

        // Verify fields were cleared
        CHECK_EQ(0, target->get_field_names().size());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::copy_field")

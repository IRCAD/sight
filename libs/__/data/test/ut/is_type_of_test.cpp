/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "is_type_of_test.hpp"

#include <data/boolean.hpp>
#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/transfer_function.hpp>

#include <utility>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::is_type_of_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void is_type_of_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void is_type_of_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void is_type_of_test::check_type()
{
    data::composite::sptr data_composite  = std::make_shared<data::composite>();
    data::boolean::sptr data_boolean      = std::make_shared<data::boolean>();
    data::real::sptr data_float           = std::make_shared<data::real>();
    data::integer::sptr data_integer      = std::make_shared<data::integer>();
    data::image::sptr data_image          = std::make_shared<data::image>();
    data::transfer_function::sptr data_tf = std::make_shared<data::transfer_function>();

    data::composite::is_type_of("sight::data::object");
    data::boolean::is_type_of("sight::data::object");
    data::real::is_type_of("sight::data::object");
    data::integer::is_type_of("sight::data::object");
    data::image::is_type_of("sight::data::object");
    data::transfer_function::is_type_of("sight::data::object");

    data::composite::is_type_of("sight::data::composite");
    data::boolean::is_type_of("sight::data::boolean");
    data::real::is_type_of("sight::data::real");
    data::integer::is_type_of("sight::data::integer");
    data::image::is_type_of("sight::data::image");
    data::transfer_function::is_type_of("sight::data::transfer_function");

    CPPUNIT_ASSERT(data_composite->is_a("sight::data::object"));
    CPPUNIT_ASSERT(data_boolean->is_a("sight::data::object"));
    CPPUNIT_ASSERT(data_float->is_a("sight::data::object"));
    CPPUNIT_ASSERT(data_integer->is_a("sight::data::object"));
    CPPUNIT_ASSERT(data_image->is_a("sight::data::object"));
    CPPUNIT_ASSERT(data_tf->is_a("sight::data::object"));

    CPPUNIT_ASSERT(data_composite->is_a("sight::data::composite"));
    CPPUNIT_ASSERT(data_boolean->is_a("sight::data::boolean"));
    CPPUNIT_ASSERT(data_float->is_a("sight::data::real"));
    CPPUNIT_ASSERT(data_integer->is_a("sight::data::integer"));
    CPPUNIT_ASSERT(data_image->is_a("sight::data::image"));
    CPPUNIT_ASSERT(data_tf->is_a("sight::data::transfer_function"));

    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_composite)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_boolean)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_float)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_integer)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_image)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_tf)->is_a("sight::data::object"));

    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_composite)->is_a("sight::data::composite"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_boolean)->is_a("sight::data::boolean"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_float)->is_a("sight::data::real"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_integer)->is_a("sight::data::integer"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_image)->is_a("sight::data::image"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(data_tf)->is_a("sight::data::transfer_function"));

    CPPUNIT_ASSERT(!data_composite->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!data_boolean->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!data_float->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!data_integer->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!data_image->is_a("sight::data::boolean"));
    CPPUNIT_ASSERT(!data_tf->is_a("sight::data::image"));
}

} // namespace sight::data::ut

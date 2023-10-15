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

void is_type_of_test::checkType()
{
    data::composite::sptr dataComposite  = std::make_shared<data::composite>();
    data::boolean::sptr dataBoolean      = std::make_shared<data::boolean>();
    data::real::sptr dataFloat           = std::make_shared<data::real>();
    data::integer::sptr dataInteger      = std::make_shared<data::integer>();
    data::image::sptr dataImage          = std::make_shared<data::image>();
    data::transfer_function::sptr dataTF = std::make_shared<data::transfer_function>();

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

    CPPUNIT_ASSERT(dataComposite->is_a("sight::data::object"));
    CPPUNIT_ASSERT(dataBoolean->is_a("sight::data::object"));
    CPPUNIT_ASSERT(dataFloat->is_a("sight::data::object"));
    CPPUNIT_ASSERT(dataInteger->is_a("sight::data::object"));
    CPPUNIT_ASSERT(dataImage->is_a("sight::data::object"));
    CPPUNIT_ASSERT(dataTF->is_a("sight::data::object"));

    CPPUNIT_ASSERT(dataComposite->is_a("sight::data::composite"));
    CPPUNIT_ASSERT(dataBoolean->is_a("sight::data::boolean"));
    CPPUNIT_ASSERT(dataFloat->is_a("sight::data::real"));
    CPPUNIT_ASSERT(dataInteger->is_a("sight::data::integer"));
    CPPUNIT_ASSERT(dataImage->is_a("sight::data::image"));
    CPPUNIT_ASSERT(dataTF->is_a("sight::data::transfer_function"));

    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataComposite)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataBoolean)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataFloat)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataInteger)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataImage)->is_a("sight::data::object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataTF)->is_a("sight::data::object"));

    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataComposite)->is_a("sight::data::composite"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataBoolean)->is_a("sight::data::boolean"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataFloat)->is_a("sight::data::real"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataInteger)->is_a("sight::data::integer"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataImage)->is_a("sight::data::image"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::object>(dataTF)->is_a("sight::data::transfer_function"));

    CPPUNIT_ASSERT(!dataComposite->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!dataBoolean->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!dataFloat->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!dataInteger->is_a("sight::data::image"));
    CPPUNIT_ASSERT(!dataImage->is_a("sight::data::boolean"));
    CPPUNIT_ASSERT(!dataTF->is_a("sight::data::image"));
}

} // namespace sight::data::ut

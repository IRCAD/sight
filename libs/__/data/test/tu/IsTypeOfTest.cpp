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

#include "IsTypeOfTest.hpp"

#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/TransferFunction.hpp>

#include <utility>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::IsTypeOfTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void IsTypeOfTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IsTypeOfTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IsTypeOfTest::checkType()
{
    data::Composite::sptr dataComposite = std::make_shared<data::Composite>();
    data::Boolean::sptr dataBoolean     = std::make_shared<data::Boolean>();
    data::Float::sptr dataFloat         = std::make_shared<data::Float>();
    data::Integer::sptr dataInteger     = std::make_shared<data::Integer>();
    data::Image::sptr dataImage         = std::make_shared<data::Image>();
    data::TransferFunction::sptr dataTF = std::make_shared<data::TransferFunction>();

    data::Composite::is_type_of("sight::data::Object");
    data::Boolean::is_type_of("sight::data::Object");
    data::Float::is_type_of("sight::data::Object");
    data::Integer::is_type_of("sight::data::Object");
    data::Image::is_type_of("sight::data::Object");
    data::TransferFunction::is_type_of("sight::data::Object");

    data::Composite::is_type_of("sight::data::Composite");
    data::Boolean::is_type_of("sight::data::Boolean");
    data::Float::is_type_of("sight::data::Float");
    data::Integer::is_type_of("sight::data::Integer");
    data::Image::is_type_of("sight::data::Image");
    data::TransferFunction::is_type_of("sight::data::TransferFunction");

    CPPUNIT_ASSERT(dataComposite->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(dataBoolean->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(dataFloat->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(dataInteger->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(dataImage->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(dataTF->is_a("sight::data::Object"));

    CPPUNIT_ASSERT(dataComposite->is_a("sight::data::Composite"));
    CPPUNIT_ASSERT(dataBoolean->is_a("sight::data::Boolean"));
    CPPUNIT_ASSERT(dataFloat->is_a("sight::data::Float"));
    CPPUNIT_ASSERT(dataInteger->is_a("sight::data::Integer"));
    CPPUNIT_ASSERT(dataImage->is_a("sight::data::Image"));
    CPPUNIT_ASSERT(dataTF->is_a("sight::data::TransferFunction"));

    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataComposite)->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataBoolean)->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataFloat)->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataInteger)->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataImage)->is_a("sight::data::Object"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataTF)->is_a("sight::data::Object"));

    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataComposite)->is_a("sight::data::Composite"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataBoolean)->is_a("sight::data::Boolean"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataFloat)->is_a("sight::data::Float"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataInteger)->is_a("sight::data::Integer"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataImage)->is_a("sight::data::Image"));
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::Object>(dataTF)->is_a("sight::data::TransferFunction"));

    CPPUNIT_ASSERT(!dataComposite->is_a("sight::data::Image"));
    CPPUNIT_ASSERT(!dataBoolean->is_a("sight::data::Image"));
    CPPUNIT_ASSERT(!dataFloat->is_a("sight::data::Image"));
    CPPUNIT_ASSERT(!dataInteger->is_a("sight::data::Image"));
    CPPUNIT_ASSERT(!dataImage->is_a("sight::data::Boolean"));
    CPPUNIT_ASSERT(!dataTF->is_a("sight::data::Image"));
}

} // namespace sight::data::ut

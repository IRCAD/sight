/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
    data::Composite::sptr dataComposite               = data::Composite::New();
    data::Boolean::sptr dataBoolean                   = data::Boolean::New();
    data::Float::sptr dataFloat                       = data::Float::New();
    data::Integer::sptr dataInteger                   = data::Integer::New();
    data::Image::sptr dataImage                       = data::Image::New();
    data::TransferFunction::sptr dataTransferFunction = data::TransferFunction::New();

    data::Composite::isTypeOf("sight::data::Object");
    data::Boolean::isTypeOf("sight::data::Object");
    data::Float::isTypeOf("sight::data::Object");
    data::Integer::isTypeOf("sight::data::Object");
    data::Image::isTypeOf("sight::data::Object");
    data::TransferFunction::isTypeOf("sight::data::Object");

    data::Composite::isTypeOf("sight::data::Composite");
    data::Boolean::isTypeOf("sight::data::Boolean");
    data::Float::isTypeOf("sight::data::Float");
    data::Integer::isTypeOf("sight::data::Integer");
    data::Image::isTypeOf("sight::data::Image");
    data::TransferFunction::isTypeOf("sight::data::TransferFunction");

    CPPUNIT_ASSERT(dataComposite->isA("sight::data::Object"));
    CPPUNIT_ASSERT(dataBoolean->isA("sight::data::Object"));
    CPPUNIT_ASSERT(dataFloat->isA("sight::data::Object"));
    CPPUNIT_ASSERT(dataInteger->isA("sight::data::Object"));
    CPPUNIT_ASSERT(dataImage->isA("sight::data::Object"));
    CPPUNIT_ASSERT(dataTransferFunction->isA("sight::data::Object"));

    CPPUNIT_ASSERT(dataComposite->isA("sight::data::Composite"));
    CPPUNIT_ASSERT(dataBoolean->isA("sight::data::Boolean"));
    CPPUNIT_ASSERT(dataFloat->isA("sight::data::Float"));
    CPPUNIT_ASSERT(dataInteger->isA("sight::data::Integer"));
    CPPUNIT_ASSERT(dataImage->isA("sight::data::Image"));
    CPPUNIT_ASSERT(dataTransferFunction->isA("sight::data::TransferFunction"));

    CPPUNIT_ASSERT(data::Object::dynamicCast(dataComposite)->isA("sight::data::Object"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataBoolean)->isA("sight::data::Object"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataFloat)->isA("sight::data::Object"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataInteger)->isA("sight::data::Object"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataImage)->isA("sight::data::Object"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataTransferFunction)->isA("sight::data::Object"));

    CPPUNIT_ASSERT(data::Object::dynamicCast(dataComposite)->isA("sight::data::Composite"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataBoolean)->isA("sight::data::Boolean"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataFloat)->isA("sight::data::Float"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataInteger)->isA("sight::data::Integer"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataImage)->isA("sight::data::Image"));
    CPPUNIT_ASSERT(data::Object::dynamicCast(dataTransferFunction)->isA("sight::data::TransferFunction"));

    CPPUNIT_ASSERT(!dataComposite->isA("sight::data::Image"));
    CPPUNIT_ASSERT(!dataBoolean->isA("sight::data::Image"));
    CPPUNIT_ASSERT(!dataFloat->isA("sight::data::Image"));
    CPPUNIT_ASSERT(!dataInteger->isA("sight::data::Image"));
    CPPUNIT_ASSERT(!dataImage->isA("sight::data::Boolean"));
    CPPUNIT_ASSERT(!dataTransferFunction->isA("sight::data::Image"));
}

} // namespace sight::data::ut

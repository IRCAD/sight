/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
CPPUNIT_TEST_SUITE_REGISTRATION( data::ut::IsTypeOfTest );

namespace sight::data
{
namespace ut
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

void IsTypeOfTest::methode1()
{
    data::Composite::sptr dataComposite               = data::Composite::New();
    data::Boolean::sptr dataBoolean                   = data::Boolean::New();
    data::Float::sptr dataFloat                       = data::Float::New();
    data::Integer::sptr dataInteger                   = data::Integer::New();
    data::Image::sptr dataImage                       = data::Image::New();
    data::TransferFunction::sptr dataTransferFunction = data::TransferFunction::New();

    data::Composite::isTypeOf( "data::Object" );
    data::Boolean::isTypeOf( "data::Object" );
    data::Float::isTypeOf( "data::Object" );
    data::Integer::isTypeOf( "data::Object" );
    data::Image::isTypeOf( "data::Object" );
    data::TransferFunction::isTypeOf( "data::Object" );

    data::Composite::isTypeOf( "data::Composite"         );
    data::Boolean::isTypeOf( "data::Boolean"           );
    data::Float::isTypeOf( "data::Float"             );
    data::Integer::isTypeOf( "data::Integer"           );
    data::Image::isTypeOf( "data::Image"             );
    data::TransferFunction::isTypeOf( "data::TransferFunction"  );

    CPPUNIT_ASSERT( dataComposite->isA("data::Object") );
    CPPUNIT_ASSERT( dataBoolean->isA("data::Object") );
    CPPUNIT_ASSERT( dataFloat->isA("data::Object") );
    CPPUNIT_ASSERT( dataInteger->isA("data::Object") );
    CPPUNIT_ASSERT( dataImage->isA("data::Object") );
    CPPUNIT_ASSERT( dataTransferFunction->isA("data::Object") );

    CPPUNIT_ASSERT( dataComposite->isA("data::Composite") );
    CPPUNIT_ASSERT( dataBoolean->isA("data::Boolean") );
    CPPUNIT_ASSERT( dataFloat->isA("data::Float") );
    CPPUNIT_ASSERT( dataInteger->isA("data::Integer") );
    CPPUNIT_ASSERT( dataImage->isA("data::Image") );
    CPPUNIT_ASSERT( dataTransferFunction->isA("data::TransferFunction") );

    CPPUNIT_ASSERT( data::Object::dynamicCast(dataComposite)->isA("data::Object") );
    CPPUNIT_ASSERT( data::Object::dynamicCast(dataBoolean)->isA("data::Object") );
    CPPUNIT_ASSERT( data::Object::dynamicCast(dataFloat)->isA("data::Object") );
    CPPUNIT_ASSERT( data::Object::dynamicCast(dataInteger)->isA("data::Object") );
    CPPUNIT_ASSERT( data::Object::dynamicCast(dataImage)->isA("data::Object") );
    CPPUNIT_ASSERT( data::Object::dynamicCast(dataTransferFunction)->isA("data::Object") );

    CPPUNIT_ASSERT( data::Object::dynamicCast( dataComposite         )->isA( "data::Composite"         ) );
    CPPUNIT_ASSERT( data::Object::dynamicCast( dataBoolean           )->isA( "data::Boolean"           ) );
    CPPUNIT_ASSERT( data::Object::dynamicCast( dataFloat             )->isA( "data::Float"             ) );
    CPPUNIT_ASSERT( data::Object::dynamicCast( dataInteger           )->isA( "data::Integer"           ) );
    CPPUNIT_ASSERT( data::Object::dynamicCast( dataImage             )->isA( "data::Image"             ) );
    CPPUNIT_ASSERT( data::Object::dynamicCast( dataTransferFunction  )->isA( "data::TransferFunction"  ) );

    CPPUNIT_ASSERT( !dataComposite->isA("data::Image") );
    CPPUNIT_ASSERT( !dataBoolean->isA("data::Image") );
    CPPUNIT_ASSERT( !dataFloat->isA("data::Image") );
    CPPUNIT_ASSERT( !dataInteger->isA("data::Image") );
    CPPUNIT_ASSERT( !dataImage->isA("data::Boolean") );
    CPPUNIT_ASSERT( !dataTransferFunction->isA("data::Image") );
}

} //namespace ut
} //namespace sight::data

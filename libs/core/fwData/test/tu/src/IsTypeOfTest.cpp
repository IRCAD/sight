/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <fwData/Composite.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

#include <utility>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::IsTypeOfTest );

namespace fwData
{
namespace ut
{

void IsTypeOfTest::setUp()
{
    // Set up context before running a test.

}
void IsTypeOfTest::tearDown()
{
    // Clean up after the test run.
}

void IsTypeOfTest::methode1()
{
    ::fwData::Composite::sptr dataComposite               = ::fwData::Composite::New();
    ::fwData::Boolean::sptr dataBoolean                   = ::fwData::Boolean::New();
    ::fwData::Float::sptr dataFloat                       = ::fwData::Float::New();
    ::fwData::Integer::sptr dataInteger                   = ::fwData::Integer::New();
    ::fwData::Image::sptr dataImage                       = ::fwData::Image::New();
    ::fwData::TransferFunction::sptr dataTransferFunction = ::fwData::TransferFunction::New();

    ::fwData::Composite::isTypeOf         ( "::fwData::Object" );
    ::fwData::Boolean::isTypeOf           ( "::fwData::Object" );
    ::fwData::Float::isTypeOf             ( "::fwData::Object" );
    ::fwData::Integer::isTypeOf           ( "::fwData::Object" );
    ::fwData::Image::isTypeOf             ( "::fwData::Object" );
    ::fwData::TransferFunction::isTypeOf  ( "::fwData::Object" );

    ::fwData::Composite::isTypeOf         ( "::fwData::Composite"         );
    ::fwData::Boolean::isTypeOf           ( "::fwData::Boolean"           );
    ::fwData::Float::isTypeOf             ( "::fwData::Float"             );
    ::fwData::Integer::isTypeOf           ( "::fwData::Integer"           );
    ::fwData::Image::isTypeOf             ( "::fwData::Image"             );
    ::fwData::TransferFunction::isTypeOf  ( "::fwData::TransferFunction"  );

    CPPUNIT_ASSERT( dataComposite->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataBoolean->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataFloat->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataInteger->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataImage->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataTransferFunction->isA("::fwData::Object") );

    CPPUNIT_ASSERT( dataComposite->isA("::fwData::Composite") );
    CPPUNIT_ASSERT( dataBoolean->isA("::fwData::Boolean") );
    CPPUNIT_ASSERT( dataFloat->isA("::fwData::Float") );
    CPPUNIT_ASSERT( dataInteger->isA("::fwData::Integer") );
    CPPUNIT_ASSERT( dataImage->isA("::fwData::Image") );
    CPPUNIT_ASSERT( dataTransferFunction->isA("::fwData::TransferFunction") );

    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataComposite)->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataBoolean)->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataFloat)->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataInteger)->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataImage)->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataTransferFunction)->isA("::fwData::Object") );

    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataComposite         )->isA( "::fwData::Composite"         ) );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataBoolean           )->isA( "::fwData::Boolean"           ) );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataFloat             )->isA( "::fwData::Float"             ) );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataInteger           )->isA( "::fwData::Integer"           ) );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataImage             )->isA( "::fwData::Image"             ) );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataTransferFunction  )->isA( "::fwData::TransferFunction"  ) );

    CPPUNIT_ASSERT( !dataComposite->isA("::fwData::Image") );
    CPPUNIT_ASSERT( !dataBoolean->isA("::fwData::Image") );
    CPPUNIT_ASSERT( !dataFloat->isA("::fwData::Image") );
    CPPUNIT_ASSERT( !dataInteger->isA("::fwData::Image") );
    CPPUNIT_ASSERT( !dataImage->isA("::fwData::Boolean") );
    CPPUNIT_ASSERT( !dataTransferFunction->isA("::fwData::Image") );
}

} //namespace ut
} //namespace fwData


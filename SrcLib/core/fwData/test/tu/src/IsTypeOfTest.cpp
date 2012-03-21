/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>
#include <boost/foreach.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Patient.hpp>


#include "IsTypeOfTest.hpp"


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
    ::fwData::Object::NewSptr            dataObject ;
    ::fwData::Composite::NewSptr         dataComposite ;
    ::fwData::Boolean::NewSptr           dataBoolean ;
    ::fwData::Float::NewSptr             dataFloat ;
    ::fwData::Integer::NewSptr           dataInteger ;
    ::fwData::Image::NewSptr             dataImage ;
    ::fwData::TransferFunction::NewSptr dataTransfertFunction ;
    ::fwData::Patient::NewSptr           dataPatient ;

    ::fwData::Object::isTypeOf            ( "::fwData::Object" );
    ::fwData::Composite::isTypeOf         ( "::fwData::Object" );
    ::fwData::Boolean::isTypeOf           ( "::fwData::Object" );
    ::fwData::Float::isTypeOf             ( "::fwData::Object" );
    ::fwData::Integer::isTypeOf           ( "::fwData::Object" );
    ::fwData::Image::isTypeOf             ( "::fwData::Object" );
    ::fwData::TransferFunction::isTypeOf ( "::fwData::Object" );
    ::fwData::Patient::isTypeOf           ( "::fwData::Object" );

    ::fwData::Composite::isTypeOf         ( "::fwData::Composite"         );
    ::fwData::Boolean::isTypeOf           ( "::fwData::Boolean"           );
    ::fwData::Float::isTypeOf             ( "::fwData::Float"             );
    ::fwData::Integer::isTypeOf           ( "::fwData::Integer"           );
    ::fwData::Image::isTypeOf             ( "::fwData::Image"             );
    ::fwData::TransferFunction::isTypeOf ( "::fwData::TransferFunction" );
    ::fwData::Patient::isTypeOf           ( "::fwData::Patient"           );

    CPPUNIT_ASSERT( dataObject           ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataComposite        ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataBoolean          ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataFloat            ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataInteger          ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataImage            ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataTransfertFunction->isA("::fwData::Object") );
    CPPUNIT_ASSERT( dataPatient          ->isA("::fwData::Object") );

    CPPUNIT_ASSERT( dataComposite        ->isA("::fwData::Composite") );
    CPPUNIT_ASSERT( dataBoolean          ->isA("::fwData::Boolean") );
    CPPUNIT_ASSERT( dataFloat            ->isA("::fwData::Float") );
    CPPUNIT_ASSERT( dataInteger          ->isA("::fwData::Integer") );
    CPPUNIT_ASSERT( dataImage            ->isA("::fwData::Image") );
    CPPUNIT_ASSERT( dataTransfertFunction->isA("::fwData::TransferFunction") );
    CPPUNIT_ASSERT( dataPatient          ->isA("::fwData::Patient") );

    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataComposite)        ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataBoolean)          ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataFloat)            ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataInteger)          ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataImage)            ->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataTransfertFunction)->isA("::fwData::Object") );
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast(dataPatient)          ->isA("::fwData::Object") );

    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataComposite         )->isA( "::fwData::Composite"         ) ) ;
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataBoolean           )->isA( "::fwData::Boolean"           ) ) ;
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataFloat             )->isA( "::fwData::Float"             ) ) ;
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataInteger           )->isA( "::fwData::Integer"           ) ) ;
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataImage             )->isA( "::fwData::Image"             ) ) ;
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataTransfertFunction )->isA( "::fwData::TransferFunction" ) ) ;
    CPPUNIT_ASSERT( ::fwData::Object::dynamicCast( dataPatient           )->isA( "::fwData::Patient"           ) ) ;

    CPPUNIT_ASSERT( ! dataObject           ->isA("::fwData::Image") );
    CPPUNIT_ASSERT( ! dataComposite        ->isA("::fwData::Image") );
    CPPUNIT_ASSERT( ! dataBoolean          ->isA("::fwData::Image") );
    CPPUNIT_ASSERT( ! dataFloat            ->isA("::fwData::Image") );
    CPPUNIT_ASSERT( ! dataInteger          ->isA("::fwData::Image") );
    CPPUNIT_ASSERT( ! dataImage            ->isA("::fwData::Boolean") );
    CPPUNIT_ASSERT( ! dataTransfertFunction->isA("::fwData::Image") );
    CPPUNIT_ASSERT( ! dataPatient          ->isA("::fwData::Image") );
}

} //namespace ut
} //namespace fwData


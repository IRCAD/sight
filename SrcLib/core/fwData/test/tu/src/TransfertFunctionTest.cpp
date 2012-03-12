/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwData/TransfertFunction.hpp>
#include <fwData/TransfertFunction_VERSION_II.hpp>
#include <fwData/String.hpp>
#include <fwData/Color.hpp>

#include "TransfertFunctionTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::TransfertFunctionTest );

namespace fwData
{
namespace ut
{

void TransfertFunctionTest::setUp()
{
    // Set up context before running a test.

}
void TransfertFunctionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void TransfertFunctionTest::constructorTest()
{
    // Expected value.
    double expectedLevel = 0.0;
    double expectedWindow = 100.0;
    std::string expectedName = "";
    TransfertFunction_VERSION_II::InterpolationMode expectedInterpolationMode = TransfertFunction_VERSION_II::LINEAR;
    bool expectedIsClamped = true;
    TransfertFunction_VERSION_II::TFColor expectedBackgroundColor = TransfertFunction_VERSION_II::TFColor();
    size_t expectedSize = 0;

    ::fwData::TransfertFunction_VERSION_II::NewSptr tf;

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->getLevel(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->getWindow(), 0.0);

    CPPUNIT_ASSERT_EQUAL(expectedName, tf->getName());
    CPPUNIT_ASSERT_EQUAL(expectedInterpolationMode, tf->getInterpolationMode());
    CPPUNIT_ASSERT_EQUAL(expectedIsClamped, tf->getIsClamped());
    CPPUNIT_ASSERT(expectedBackgroundColor==tf->getBackgroundColor());
    CPPUNIT_ASSERT_EQUAL(expectedSize, tf->getTFData().size());


}

//------------------------------------------------------------------------------
void TransfertFunctionTest::defaultTfTest()
{
    // Expected default value.
    double expectedLevel = 0.0;
    double expectedWindow = 100.0;

    std::string expectedName = TransfertFunction_VERSION_II::s_DEFAULT_TF_NAME;
    TransfertFunction_VERSION_II::InterpolationMode expectedInterpolationMode = TransfertFunction_VERSION_II::LINEAR;
    bool expectedIsClamped = true;
    TransfertFunction_VERSION_II::TFColor expectedBackgroundColor = TransfertFunction_VERSION_II::TFColor();
    size_t expectedSize = 2;

    ::fwData::TransfertFunction_VERSION_II::sptr tf = ::fwData::TransfertFunction_VERSION_II::createDefaultTF();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->getLevel(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->getWindow(), 0.0);

    CPPUNIT_ASSERT_EQUAL(expectedName, tf->getName());
    CPPUNIT_ASSERT_EQUAL(expectedInterpolationMode, tf->getInterpolationMode());
    CPPUNIT_ASSERT_EQUAL(expectedIsClamped, tf->getIsClamped());
    CPPUNIT_ASSERT(expectedBackgroundColor==tf->getBackgroundColor());
    CPPUNIT_ASSERT_EQUAL(expectedSize, tf->getTFData().size());

}

//------------------------------------------------------------------------------

void TransfertFunctionTest::classicGetSetTest()
{
    TransfertFunction_VERSION_II::TFColor expectedColor1(0.0, 0.0, 0.0, 0.0);
    TransfertFunction_VERSION_II::TFColor expectedColor2(1.0, 1.0, 1.0, 1.0);
    TransfertFunction_VERSION_II::TFValueType expectedKey1(0.0);
    TransfertFunction_VERSION_II::TFValueType expectedKey2(1.0);

    ::fwData::TransfertFunction_VERSION_II::sptr tf = ::fwData::TransfertFunction_VERSION_II::createDefaultTF();

    // Test getTFData()
    const TransfertFunction_VERSION_II::TFDataType& data = tf->getTFData();

    TransfertFunction_VERSION_II::TFDataType::const_iterator itr = data.begin();
    TransfertFunction_VERSION_II::TFValueType key1 =  itr->first;
    TransfertFunction_VERSION_II::TFValueType key2 = (++itr)->first;

    TransfertFunction_VERSION_II::TFDataType::const_iterator itrColor = data.begin();
    TransfertFunction_VERSION_II::TFColor color1 = itrColor->second;
    TransfertFunction_VERSION_II::TFColor color2 = (++itrColor)->second;

    CPPUNIT_ASSERT(expectedKey1 == key1);
    CPPUNIT_ASSERT(expectedKey2 == key2);

    CPPUNIT_ASSERT(expectedColor1 == color1);
    CPPUNIT_ASSERT(expectedColor2 == color2);

    // Test getTFColor()
    CPPUNIT_ASSERT(expectedColor1 == tf->getTFColor(key1));

    // Test getTFValues()
    TransfertFunction_VERSION_II::TFValueVectorType values = tf->getTFValues();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong key value", expectedKey1 , values[0], 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong key value", expectedKey2 , values[1], 0.0);

    // Test getTFColors()
    TransfertFunction_VERSION_II::TFColorVectorType colors = tf->getTFColors();

    CPPUNIT_ASSERT(expectedColor1 == colors[0]);
    CPPUNIT_ASSERT(expectedColor2 ==  colors[1]);

    // Test erase
    tf->eraseTFValue( 1.0 );
    CPPUNIT_ASSERT_EQUAL( (size_t)1, tf->getTFData().size() );
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong key value", expectedKey1 , tf->getTFValues()[0], 0.0);

    // Test clear()
    tf->clear();
    size_t expectedClearedSize = 0;
    CPPUNIT_ASSERT_EQUAL(expectedClearedSize, tf->getTFData().size());

}

//------------------------------------------------------------------------------

void TransfertFunctionTest::usingTfTest()
{
    ::fwData::TransfertFunction_VERSION_II::sptr tf = this->createTFColor();

    // -40.33 / -0.2 / 3 / 150
    CPPUNIT_ASSERT_EQUAL( -40.33, tf->getNearestValue( -140.33 ) );
    CPPUNIT_ASSERT_EQUAL( -40.33, tf->getNearestValue( -40.33 ) );
    CPPUNIT_ASSERT_EQUAL( -40.33, tf->getNearestValue( -25 ) );
    CPPUNIT_ASSERT_EQUAL( -0.2, tf->getNearestValue( -10.8 ) );
    CPPUNIT_ASSERT_EQUAL( -0.2, tf->getNearestValue( -0.2 ) );
    CPPUNIT_ASSERT_EQUAL( -0.2, tf->getNearestValue( 1.0 ) );
    CPPUNIT_ASSERT_EQUAL( 3.0, tf->getNearestValue( 50 ) );
    CPPUNIT_ASSERT_EQUAL( 3.0, tf->getNearestValue( 3 ) );
    CPPUNIT_ASSERT_EQUAL( 150.0, tf->getNearestValue( 150 ) );
    CPPUNIT_ASSERT_EQUAL( 150.0, tf->getNearestValue( 1000 ) );


}

//------------------------------------------------------------------------------

void TransfertFunctionTest::shallowAndDeepCopyTest()
{
    ::fwData::TransfertFunction_VERSION_II::sptr tf = this->createTFColor();
    this->checkTFColor(tf);

    ::fwData::TransfertFunction_VERSION_II::NewSptr deepCopyTf;
    deepCopyTf->deepCopy( tf );
    this->checkTFColor(deepCopyTf);

    ::fwData::TransfertFunction_VERSION_II::NewSptr shallowCopyTf;
    shallowCopyTf->shallowCopy( tf );
    this->checkTFColor(shallowCopyTf);
}


//------------------------------------------------------------------------------

::fwData::TransfertFunction_VERSION_II::sptr TransfertFunctionTest::createTFColor()
{
    ::fwData::TransfertFunction_VERSION_II::NewSptr tf;

    tf->setBackgroundColor( ::fwData::TransfertFunction_VERSION_II::TFColor( 1.0, 0.3, 0.6, 0.1) );
    tf->setInterpolationMode( ::fwData::TransfertFunction_VERSION_II::NEAREST );
    tf->setIsClamped( false );
    tf->setLevel( 900.6 );
    tf->setName( "TFColor" );
    tf->setWindow( -200.02 );

    tf->addTFColor( -40.33, ::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) );
    tf->addTFColor( 3,      ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) ); // Invert point 3 <=> -0.2, for tests
    tf->addTFColor( -0.2,   ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) );
    tf->addTFColor( 150,    ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) );

    ::fwData::String::NewSptr myString ("fieldStringValue");
    tf->setField_NEWAPI( "fieldStringKey", myString );

    return tf;
}

//------------------------------------------------------------------------------

void TransfertFunctionTest::checkTFColor( ::fwData::TransfertFunction_VERSION_II::sptr tf )
{

    CPPUNIT_ASSERT( ::fwData::TransfertFunction_VERSION_II::TFColor( 1.0, 0.3, 0.6, 0.1) == tf->getBackgroundColor() );
    CPPUNIT_ASSERT_EQUAL( ::fwData::TransfertFunction_VERSION_II::NEAREST, tf->getInterpolationMode() );
    CPPUNIT_ASSERT_EQUAL( false, tf->getIsClamped() );
    CPPUNIT_ASSERT_EQUAL( 900.6, tf->getLevel() );
    CPPUNIT_ASSERT_EQUAL( std::string("TFColor"), tf->getName() );
    CPPUNIT_ASSERT_EQUAL( -200.02, tf->getWindow() );

    CPPUNIT_ASSERT_EQUAL( (size_t)4, tf->getTFData().size() );
    CPPUNIT_ASSERT_EQUAL( -40.33, tf->getMinMaxTFValues().first );
    CPPUNIT_ASSERT_EQUAL( 150.0, tf->getMinMaxTFValues().second );

    const TransfertFunction_VERSION_II::TFValueVectorType & values = tf->getTFValues();
    CPPUNIT_ASSERT_EQUAL( values[0] , -40.33  );
    CPPUNIT_ASSERT_EQUAL( values[1] , -0.2    );
    CPPUNIT_ASSERT_EQUAL( values[2] , 3.0      );
    CPPUNIT_ASSERT_EQUAL( values[3] , 150.0     );

    CPPUNIT_ASSERT( ::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getTFColor( -40.33 ) );
    CPPUNIT_ASSERT( ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) == tf->getTFColor( -0.2   ) );
    CPPUNIT_ASSERT( ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getTFColor( 3      ) );
    CPPUNIT_ASSERT( ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getTFColor( 150    ) );



    CPPUNIT_ASSERT_EQUAL( std::string("fieldStringValue"), ::fwData::String::dynamicCast( tf->getField_NEWAPI( "fieldStringKey" ) )->value() );
}

//------------------------------------------------------------------------------

void TransfertFunctionTest::linearColorTest()
{
    ::fwData::TransfertFunction_VERSION_II::sptr tf = this->createTFColor();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2  => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3     => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150   => color : {0.1, 0.2, 0.3, 0.9}

    tf->setIsClamped(true);
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getLinearColor(-120) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getLinearColor(200) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getLinearColor(-40.33) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getLinearColor(150) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.6, 0.65) == tf->getLinearColor(((150.0-3.0)/2.0)+3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) == tf->getLinearColor(-0.2) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getLinearColor(3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.75, 0.525) == tf->getLinearColor(((150.0-3.0)/4.0)+3) );

    tf->setInterpolationMode(TransfertFunction_VERSION_II::LINEAR);
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(-120) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(200) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getInterpolatedColor(-40.33) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getInterpolatedColor(150) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.6, 0.65) == tf->getInterpolatedColor(((150.0-3.0)/2.0)+3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) == tf->getInterpolatedColor(-0.2) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getInterpolatedColor(3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.75, 0.525) == tf->getInterpolatedColor(((150.0-3.0)/4.0)+3) );

    tf->setIsClamped(false);
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getLinearColor(-120) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getLinearColor(200) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getLinearColor(-40.33) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getLinearColor(150) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.6, 0.65) == tf->getLinearColor(((150.0-3.0)/2.0)+3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) == tf->getLinearColor(-0.2) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getLinearColor(3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.75, 0.525) == tf->getLinearColor(((150.0-3.0)/4.0)+3) );


}

//------------------------------------------------------------------------------

void TransfertFunctionTest::nearestColorTest()
{
    ::fwData::TransfertFunction_VERSION_II::sptr tf = this->createTFColor();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2  => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3     => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150   => color : {0.1, 0.2, 0.3, 0.9}

    tf->setIsClamped(true);
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getNearestColor(-120) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getNearestColor(200) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getNearestColor(-40.33) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(150) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(((150.0-3.0)/2.0)+3 + 0.1) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) == tf->getNearestColor(-0.2) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getNearestColor(3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getNearestColor(((150.0-3.0)/4.0)+3) );

    tf->setInterpolationMode(TransfertFunction_VERSION_II::NEAREST);
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(-120) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(200) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getInterpolatedColor(-40.33) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getInterpolatedColor(150) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getInterpolatedColor(((150.0-3.0)/2.0)+3 + 0.1) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) == tf->getInterpolatedColor(-0.2) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getInterpolatedColor(3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getInterpolatedColor(((150.0-3.0)/4.0)+3) );

    tf->setIsClamped(false);
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getNearestColor(-120) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(200) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.9, 0.2, 0.3, 0.4) == tf->getNearestColor(-40.33) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(150) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(((150.0-3.0)/2.0)+3 + 0.1) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.9, 0.3, 0.4) == tf->getNearestColor(-0.2) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getNearestColor(3) );
    CPPUNIT_ASSERT(::fwData::TransfertFunction_VERSION_II::TFColor( 0.1, 0.2, 0.9, 0.4) == tf->getNearestColor(((150.0-3.0)/4.0)+3) );

}

} //namespace ut
} //namespace fwData


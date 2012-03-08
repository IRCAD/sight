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

void TransfertFunctionTest::methode1()
{
    //-----------test values
    const ::fwData::Color::sptr C1 = ::fwData::Color::New(0.5f, 0.4f ,0.6f, 0.1f);
    const ::fwData::Color::sptr C2 = ::fwData::Color::New();
    const int INDEX1 = 2;
    const int INDEX2 = 5;
    const std::string NAME = "tfName";
    const std::string ENCODING = "tfencoding";
    const boost::uint32_t SIZE = 2;

    ::fwData::TransfertFunction::NewSptr tf;

    tf->setCRefEncoding(ENCODING);
    tf->setCRefName(NAME);
    tf->getColor(INDEX1)->deepCopy( C1 );
    tf->getColor(INDEX2)->deepCopy( C2 );

    CPPUNIT_ASSERT_EQUAL(tf->getTransfertFunctionPointSize(), SIZE);
    CPPUNIT_ASSERT_EQUAL(tf->getName(), NAME);
    CPPUNIT_ASSERT_EQUAL(tf->getEncoding(), ENCODING);
    CPPUNIT_ASSERT_EQUAL(tf->getColor(INDEX1)->getRGBA()[0], C1->getRGBA()[0]);
    CPPUNIT_ASSERT_EQUAL(tf->getColor(INDEX1)->getRGBA()[1], C1->getRGBA()[1]);
}

//------------------------------------------------------------------------------

void TransfertFunctionTest::constructorTest()
{
    // Expected value.
    double expectedLevel = 0.0;
    double expectedWindow = 100.0;
    double expectedRefLevel = 0.5;
    double expectedRefWindow = 1.0;
    std::string expectedName = "";
    TransfertFunction_VERSION_II::InterpolationMode expectedInterpolationMode = TransfertFunction_VERSION_II::LINEAR;
    bool expectedIsClamped = true;
    TransfertFunction_VERSION_II::TFColor expectedBackgroundColor = TransfertFunction_VERSION_II::TFColor();
    size_t expectedSize = 0;

    ::fwData::TransfertFunction_VERSION_II::NewSptr tf;

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->getLevel(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->getWindow(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong reference level", expectedRefLevel, tf->getRrefLevel(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong reference window", expectedRefWindow, tf->getRrefWindow(), 0.0);
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
    double expectedRefLevel = 0.5;
    double expectedRefWindow = 1.0;
    std::string expectedName = TransfertFunction_VERSION_II::s_DEFAULT_TF_NAME;
    TransfertFunction_VERSION_II::InterpolationMode expectedInterpolationMode = TransfertFunction_VERSION_II::LINEAR;
    bool expectedIsClamped = true;
    TransfertFunction_VERSION_II::TFColor expectedBackgroundColor = TransfertFunction_VERSION_II::TFColor();
    size_t expectedSize = 2;

    ::fwData::TransfertFunction_VERSION_II::sptr tf = ::fwData::TransfertFunction_VERSION_II::createDefaultTF();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->getLevel(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->getWindow(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong reference level", expectedRefLevel, tf->getRrefLevel(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong reference window", expectedRefWindow, tf->getRrefWindow(), 0.0);
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

    // Test clear()
    tf->clear();
    size_t expectedClearedSize = 0;
    CPPUNIT_ASSERT_EQUAL(expectedClearedSize, tf->getTFData().size());

}

//------------------------------------------------------------------------------

void TransfertFunctionTest::usingTfTest()
{

}

} //namespace ut
} //namespace fwData


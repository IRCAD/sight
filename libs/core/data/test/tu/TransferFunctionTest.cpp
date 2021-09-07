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

#include "TransferFunctionTest.hpp"

#include <data/Color.hpp>
#include <data/String.hpp>
#include <data/TransferFunction.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::TransferFunctionTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void TransferFunctionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TransferFunctionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void TransferFunctionTest::constructorTest()
{
    // Expected value.
    const double expectedLevel                                          = 0.0;
    const double expectedWindow                                         = 100.0;
    const std::string expectedName                                      = "";
    const TransferFunction::InterpolationMode expectedInterpolationMode = TransferFunction::LINEAR;
    const bool expectedIsClamped                                        = true;
    const TransferFunction::TFColor expectedBackgroundColor             = TransferFunction::TFColor();
    const size_t expectedSize                                           = 0;

    data::TransferFunction::csptr tf = data::TransferFunction::New();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->getLevel(), 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->getWindow(), 0.0);

    CPPUNIT_ASSERT_EQUAL(expectedName, tf->getName());
    CPPUNIT_ASSERT_EQUAL(expectedInterpolationMode, tf->getInterpolationMode());
    CPPUNIT_ASSERT_EQUAL(expectedIsClamped, tf->getIsClamped());
    CPPUNIT_ASSERT(expectedBackgroundColor == tf->getBackgroundColor());
    CPPUNIT_ASSERT_EQUAL(expectedSize, tf->getTFData().size());
}

//------------------------------------------------------------------------------
void TransferFunctionTest::defaultTfTest()
{
    // Expected default value.
    double expectedLevel  = 0.0;
    double expectedWindow = 100.0;

    const std::string expectedName                                      = TransferFunction::s_DEFAULT_TF_NAME;
    const TransferFunction::InterpolationMode expectedInterpolationMode = TransferFunction::LINEAR;
    const bool expectedIsClamped                                        = false;
    const TransferFunction::TFColor expectedBackgroundColor             = TransferFunction::TFColor();
    const size_t expectedSize                                           = 2;

    data::TransferFunction::csptr tf = data::TransferFunction::createDefaultTF();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->getLevel(), 50.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->getWindow(), 500.0);

    CPPUNIT_ASSERT_EQUAL(expectedName, tf->getName());
    CPPUNIT_ASSERT_EQUAL(expectedInterpolationMode, tf->getInterpolationMode());
    CPPUNIT_ASSERT_EQUAL(expectedIsClamped, tf->getIsClamped());
    CPPUNIT_ASSERT(expectedBackgroundColor == tf->getBackgroundColor());
    CPPUNIT_ASSERT_EQUAL(expectedSize, tf->getTFData().size());
}

//------------------------------------------------------------------------------

void TransferFunctionTest::classicGetSetTest()
{
    const TransferFunction::TFColor expectedColor1(0.0, 0.0, 0.0, 0.0);
    const TransferFunction::TFColor expectedColor2(1.0, 1.0, 1.0, 1.0);
    const TransferFunction::TFValueType expectedKey1(0.0);
    const TransferFunction::TFValueType expectedKey2(1.0);

    data::TransferFunction::sptr tf = data::TransferFunction::createDefaultTF();

    // Test getTFData()
    const TransferFunction::TFDataType& data = tf->getTFData();

    TransferFunction::TFDataType::const_iterator itr = data.begin();
    const TransferFunction::TFValueType key1         = itr->first;
    const TransferFunction::TFValueType key2         = (++itr)->first;

    TransferFunction::TFDataType::const_iterator itrColor = data.begin();
    const TransferFunction::TFColor color1                = itrColor->second;
    const TransferFunction::TFColor color2                = (++itrColor)->second;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedKey1, key1, 1e-10);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedKey2, key2, 1e-10);

    CPPUNIT_ASSERT(expectedColor1 == color1);
    CPPUNIT_ASSERT(expectedColor2 == color2);

    // Test getTFColor()
    CPPUNIT_ASSERT(expectedColor1 == tf->getTFColor(key1));

    // Test getTFValues()
    const TransferFunction::TFValueVectorType values = tf->getTFValues();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong key value", expectedKey1, values[0], 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong key value", expectedKey2, values[1], 0.0);

    // Test getTFColors()
    const TransferFunction::TFColorVectorType colors = tf->getTFColors();

    CPPUNIT_ASSERT(expectedColor1 == colors[0]);
    CPPUNIT_ASSERT(expectedColor2 == colors[1]);

    // Test erase
    tf->eraseTFValue(1.0);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, tf->getTFData().size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong key value", expectedKey1, tf->getTFValues()[0], 0.0);

    // Test clear()
    tf->clear();
    const size_t expectedClearedSize = 0;
    CPPUNIT_ASSERT_EQUAL(expectedClearedSize, tf->getTFData().size());
}

//------------------------------------------------------------------------------

void TransferFunctionTest::usingTfTest()
{
    data::TransferFunction::csptr tf = this->createTFColor();

    // -40.33 / -0.2 / 3 / 150
    CPPUNIT_ASSERT_EQUAL(-40.33, tf->getNearestValue(-140.33));
    CPPUNIT_ASSERT_EQUAL(-40.33, tf->getNearestValue(-40.33));
    CPPUNIT_ASSERT_EQUAL(-40.33, tf->getNearestValue(-25));
    CPPUNIT_ASSERT_EQUAL(-0.2, tf->getNearestValue(-10.8));
    CPPUNIT_ASSERT_EQUAL(-0.2, tf->getNearestValue(-0.2));
    CPPUNIT_ASSERT_EQUAL(-0.2, tf->getNearestValue(1.0));
    CPPUNIT_ASSERT_EQUAL(3.0, tf->getNearestValue(50));
    CPPUNIT_ASSERT_EQUAL(3.0, tf->getNearestValue(3));
    CPPUNIT_ASSERT_EQUAL(150.0, tf->getNearestValue(150));
    CPPUNIT_ASSERT_EQUAL(150.0, tf->getNearestValue(1000));
}

//------------------------------------------------------------------------------

void TransferFunctionTest::shallowAndDeepCopyTest()
{
    const data::TransferFunction::sptr tf = this->createTFColor();
    this->checkTFColor(tf);

    const data::TransferFunction::sptr deepCopyTf = data::Object::copy(tf);
    this->checkTFColor(deepCopyTf);

    const data::TransferFunction::sptr shallowCopyTf = data::TransferFunction::New();
    shallowCopyTf->shallowCopy(tf);
    this->checkTFColor(shallowCopyTf);
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr TransferFunctionTest::createTFColor()
{
    data::TransferFunction::sptr tf = data::TransferFunction::New();

    tf->setBackgroundColor(data::TransferFunction::TFColor(1.0, 0.3, 0.6, 0.1));
    tf->setInterpolationMode(data::TransferFunction::NEAREST);
    tf->setIsClamped(false);
    tf->setLevel(900.6);
    tf->setName("TFColor");
    tf->setWindow(-200.02);

    tf->addTFColor(-40.33, data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4));
    tf->addTFColor(3, data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4)); // Invert point 3 <=> -0.2, for
    // tests
    tf->addTFColor(-0.2, data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4));
    tf->addTFColor(150, data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9));

    data::String::sptr myString = data::String::New("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

void TransferFunctionTest::checkTFColor(data::TransferFunction::sptr tf)
{
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(1.0, 0.3, 0.6, 0.1) == tf->getBackgroundColor());
    CPPUNIT_ASSERT_EQUAL(data::TransferFunction::NEAREST, tf->getInterpolationMode());
    CPPUNIT_ASSERT_EQUAL(false, tf->getIsClamped());
    CPPUNIT_ASSERT_EQUAL(900.6, tf->getLevel());
    CPPUNIT_ASSERT_EQUAL(std::string("TFColor"), tf->getName());
    CPPUNIT_ASSERT_EQUAL(-200.02, tf->getWindow());

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tf->getTFData().size());
    CPPUNIT_ASSERT_EQUAL(-40.33, tf->getMinMaxTFValues().first);
    CPPUNIT_ASSERT_EQUAL(150.0, tf->getMinMaxTFValues().second);

    const TransferFunction::TFValueVectorType& values = tf->getTFValues();
    CPPUNIT_ASSERT_EQUAL(values[0], -40.33);
    CPPUNIT_ASSERT_EQUAL(values[1], -0.2);
    CPPUNIT_ASSERT_EQUAL(values[2], 3.0);
    CPPUNIT_ASSERT_EQUAL(values[3], 150.0);

    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getTFColor(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4) == tf->getTFColor(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4) == tf->getTFColor(3));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getTFColor(150));

    CPPUNIT_ASSERT_EQUAL(
        std::string("fieldStringValue"),
        data::String::dynamicCast(tf->getField("fieldStringKey"))->value()
    );
}

//------------------------------------------------------------------------------

void TransferFunctionTest::setTFDataTest()
{
    data::TransferFunction::sptr tf = this->createTFColor();

    tf->clear();
    data::TransferFunction::TFDataType tfData;
    tfData[-40.33] = data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4);
    tfData[3]      = data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4);
    tfData[-0.2]   = data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4);
    tfData[150]    = data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9);
    tf->setTFData(tfData);

    this->checkTFColor(tf);
}

//------------------------------------------------------------------------------

void TransferFunctionTest::linearColorTest()
{
    data::TransferFunction::sptr tf = this->createTFColor();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2  => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3     => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150   => color : {0.1, 0.2, 0.3, 0.9}

    tf->setIsClamped(true);
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getLinearColor(-120));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getLinearColor(200));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getLinearColor(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getLinearColor(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.6,
            0.65
        ) == tf->getLinearColor(((150.0 - 3.0) / 2.0) + 3)
    );
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4) == tf->getLinearColor(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4) == tf->getLinearColor(3));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(0.1, 0.2, 0.75, 0.525) == tf->getLinearColor(
            ((150.0 - 3.0) / 4.0) + 3
        )
    );

    tf->setInterpolationMode(TransferFunction::LINEAR);
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(-120));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(200));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getInterpolatedColor(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getInterpolatedColor(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.6,
            0.65
        ) == tf->getInterpolatedColor(((150.0 - 3.0) / 2.0) + 3)
    );
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4) == tf->getInterpolatedColor(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4) == tf->getInterpolatedColor(3));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.75,
            0.525
        ) == tf->getInterpolatedColor(((150.0 - 3.0) / 4.0) + 3)
    );

    tf->setIsClamped(false);
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getLinearColor(-120));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getLinearColor(200));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getLinearColor(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getLinearColor(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.6,
            0.65
        ) == tf->getLinearColor(((150.0 - 3.0) / 2.0) + 3)
    );
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4) == tf->getLinearColor(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4) == tf->getLinearColor(3));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(0.1, 0.2, 0.75, 0.525) == tf->getLinearColor(
            ((150.0 - 3.0) / 4.0) + 3
        )
    );
}

//------------------------------------------------------------------------------

void TransferFunctionTest::nearestColorTest()
{
    data::TransferFunction::sptr tf = this->createTFColor();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2  => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3     => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150   => color : {0.1, 0.2, 0.3, 0.9}

    tf->setIsClamped(true);
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getNearestColor(-120));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getNearestColor(200));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getNearestColor(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.3,
            0.9
        ) == tf->getNearestColor(((150.0 - 3.0) / 2.0) + 3 + 0.1)
    );
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4) == tf->getNearestColor(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4) == tf->getNearestColor(3));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.9,
            0.4
        ) == tf->getNearestColor(((150.0 - 3.0) / 4.0) + 3)
    );

    tf->setInterpolationMode(TransferFunction::NEAREST);
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(-120));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.0, 0.0, 0.0, 0.0) == tf->getInterpolatedColor(200));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getInterpolatedColor(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getInterpolatedColor(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.3,
            0.9
        ) == tf->getInterpolatedColor(((150.0 - 3.0) / 2.0) + 3 + 0.1)
    );
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4) == tf->getInterpolatedColor(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4) == tf->getInterpolatedColor(3));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.9,
            0.4
        ) == tf->getInterpolatedColor(((150.0 - 3.0) / 4.0) + 3)
    );

    tf->setIsClamped(false);
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getNearestColor(-120));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(200));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.9, 0.2, 0.3, 0.4) == tf->getNearestColor(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.3, 0.9) == tf->getNearestColor(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.3,
            0.9
        ) == tf->getNearestColor(((150.0 - 3.0) / 2.0) + 3 + 0.1)
    );
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.9, 0.3, 0.4) == tf->getNearestColor(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::TFColor(0.1, 0.2, 0.9, 0.4) == tf->getNearestColor(3));
    CPPUNIT_ASSERT(
        data::TransferFunction::TFColor(
            0.1,
            0.2,
            0.9,
            0.4
        ) == tf->getNearestColor(((150.0 - 3.0) / 4.0) + 3)
    );
}

} //namespace ut

} //namespace sight::data

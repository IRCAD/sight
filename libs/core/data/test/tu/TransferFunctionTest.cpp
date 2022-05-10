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

#include "TransferFunctionTest.hpp"

#include <data/Color.hpp>
#include <data/String.hpp>
#include <data/TransferFunction.hpp>

#include <glm/common.hpp>
#include <glm/gtc/epsilon.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::TransferFunctionTest);

static const double s_EPSILON = 1e-5;

// There might be some uncertainty when sampling, so we need to include an epsilon when testing equality
#define ASSERT_COLOR_EQUALS(c1, c2) \
    CPPUNIT_ASSERT(glm::all(glm::epsilonEqual(c1, c2, s_EPSILON)));

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
    const TransferFunction::InterpolationMode expectedInterpolationMode = TransferFunction::InterpolationMode::LINEAR;
    const bool expectedIsClamped                                        = true;
    const TransferFunction::color_t expectedBackgroundColor             = TransferFunction::color_t();
    const std::size_t expectedSize                                      = 0;

    data::TransferFunction::csptr tf = data::TransferFunction::New();

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->level());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->window());

    CPPUNIT_ASSERT_EQUAL(expectedName, tf->name());
    CPPUNIT_ASSERT_EQUAL(expectedInterpolationMode, tf->interpolationMode());
    CPPUNIT_ASSERT_EQUAL(expectedIsClamped, tf->clamped());
    CPPUNIT_ASSERT(expectedBackgroundColor == tf->backgroundColor());
    CPPUNIT_ASSERT_EQUAL(expectedSize, tf->size());
}

//------------------------------------------------------------------------------
void TransferFunctionTest::defaultTfTest()
{
    // Expected default value.
    double expectedLevel  = 50.0;
    double expectedWindow = 500.0;

    const std::string expectedName                                      = TransferFunction::s_DEFAULT_TF_NAME;
    const TransferFunction::InterpolationMode expectedInterpolationMode = TransferFunction::InterpolationMode::LINEAR;
    const bool expectedIsClamped                                        = false;
    const TransferFunction::color_t expectedBackgroundColor             = TransferFunction::color_t();
    const std::size_t expectedSize                                      = 2;

    data::TransferFunction::csptr tf = data::TransferFunction::createDefaultTF();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", expectedLevel, tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", expectedWindow, tf->window(), s_EPSILON);

    CPPUNIT_ASSERT_EQUAL(expectedName, tf->name());
    CPPUNIT_ASSERT_EQUAL(expectedInterpolationMode, tf->interpolationMode());
    CPPUNIT_ASSERT_EQUAL(expectedIsClamped, tf->clamped());
    CPPUNIT_ASSERT(expectedBackgroundColor == tf->backgroundColor());
    CPPUNIT_ASSERT_EQUAL(expectedSize, tf->size());
}

//------------------------------------------------------------------------------

void TransferFunctionTest::classicGetSetTest()
{
    const TransferFunction::color_t expectedColor1(0.0, 0.0, 0.0, 0.0);
    const TransferFunction::color_t expectedColor2(1.0, 1.0, 1.0, 1.0);
    const TransferFunction::value_t expectedKey1(-200.0);
    const TransferFunction::value_t expectedKey2(300.0);

    data::TransferFunction::sptr tf = data::TransferFunction::createDefaultTF();

    // Test getTFData()
    TransferFunction::data_t::const_iterator itr = tf->cbegin();
    const TransferFunction::value_t key1         = itr->first;
    const TransferFunction::value_t key2         = (++itr)->first;

    TransferFunction::data_t::const_iterator itrColor = tf->cbegin();
    const TransferFunction::color_t color1            = itrColor->second;
    const TransferFunction::color_t color2            = (++itrColor)->second;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedKey1, key1, 1e-10);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedKey2, key2, 1e-10);

    CPPUNIT_ASSERT(expectedColor1 == color1);
    CPPUNIT_ASSERT(expectedColor2 == color2);
    CPPUNIT_ASSERT(expectedColor1 == tf->find(key1)->second);

    // Test erase
    tf->erase(300.0);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 1, tf->size());

    // Test clear()
    tf->clear();
    const std::size_t expectedClearedSize = 0;
    CPPUNIT_ASSERT_EQUAL(expectedClearedSize, tf->size());
}

//------------------------------------------------------------------------------

void TransferFunctionTest::shallowAndDeepCopyTest()
{
    const data::TransferFunction::sptr tf = this->createTFColor();
    tf->setLevel(900.6);
    tf->setWindow(-200.02);

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

    tf->setBackgroundColor(data::TransferFunction::color_t(1.0, 0.3, 0.6, 0.1));
    tf->setInterpolationMode(data::TransferFunction::InterpolationMode::NEAREST);
    tf->setClamped(false);
    tf->setWindowMinMax({-40.33, 150.});

    tf->setName("color_t");
    tf->insert({-40.33, data::TransferFunction::color_t(0.9, 0.2, 0.3, 0.4)});
    tf->insert({3, data::TransferFunction::color_t(0.1, 0.2, 0.9, 0.4)}); // Invert point 3 <=> -0.2, for
    // tests
    tf->insert({-0.2, data::TransferFunction::color_t(0.1, 0.9, 0.3, 0.4)});
    tf->insert({150, data::TransferFunction::color_t(0.1, 0.2, 0.3, 0.9)});

    data::String::sptr myString = data::String::New("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

void TransferFunctionTest::checkTFColor(data::TransferFunction::sptr tf)
{
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 0.3, 0.6, 0.1) == tf->backgroundColor());
    CPPUNIT_ASSERT_EQUAL(data::TransferFunction::InterpolationMode::NEAREST, tf->interpolationMode());
    CPPUNIT_ASSERT_EQUAL(false, tf->clamped());
    CPPUNIT_ASSERT_EQUAL(900.6, tf->level());
    CPPUNIT_ASSERT_EQUAL(std::string("color_t"), tf->name());
    CPPUNIT_ASSERT_EQUAL(-200.02, tf->window());

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), tf->size());
    CPPUNIT_ASSERT_EQUAL(-40.33, tf->minMax().first);
    CPPUNIT_ASSERT_EQUAL(150., tf->minMax().second);

    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.9, 0.2, 0.3, 0.4) == tf->find(-40.33)->second);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.1, 0.9, 0.3, 0.4) == tf->find(-0.2)->second);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.1, 0.2, 0.9, 0.4) == tf->find(3)->second);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.1, 0.2, 0.3, 0.9) == tf->find(150)->second);

    CPPUNIT_ASSERT_EQUAL(
        std::string("fieldStringValue"),
        data::String::dynamicCast(tf->getField("fieldStringKey"))->value()
    );
}

//------------------------------------------------------------------------------

void TransferFunctionTest::setTFDataTest()
{
    data::TransferFunction::sptr tf = this->createTFColor();
    tf->setLevel(900.6);
    tf->setWindow(-200.02);

    tf->clear();
    data::TransferFunction::data_t tfData;
    tfData[-40.33] = data::TransferFunction::color_t(0.9, 0.2, 0.3, 0.4);
    tfData[3]      = data::TransferFunction::color_t(0.1, 0.2, 0.9, 0.4);
    tfData[-0.2]   = data::TransferFunction::color_t(0.1, 0.9, 0.3, 0.4);
    tfData[150]    = data::TransferFunction::color_t(0.1, 0.2, 0.3, 0.9);
    std::copy(tfData.begin(), tfData.end(), inserter(*tf));

    this->checkTFColor(tf);
}

//------------------------------------------------------------------------------

void TransferFunctionTest::linearColorTest()
{
    data::TransferFunction::sptr tf = this->createTFColor();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2   => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3      => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150    => color : {0.1, 0.2, 0.3, 0.9}

    tf->setClamped(true);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sampleLinear(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sampleLinear(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), tf->sampleLinear(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), tf->sampleLinear(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), tf->sampleLinear(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), tf->sampleLinear(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), tf->sampleLinear(3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .75, .525), tf->sampleLinear(((150. - 3.) / 4.) + 3));

    tf->setInterpolationMode(TransferFunction::InterpolationMode::LINEAR);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sample(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sample(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), tf->sample(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), tf->sample(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), tf->sample(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), tf->sample(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), tf->sample(3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .75, .525), tf->sample(((150. - 3.) / 4.) + 3));

    tf->setClamped(false);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), tf->sampleLinear(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), tf->sampleLinear(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), tf->sampleLinear(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), tf->sampleLinear(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), tf->sampleLinear(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), tf->sampleLinear(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), tf->sampleLinear(3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .75, .525), tf->sampleLinear(((150. - 3.) / 4.) + 3));
}

//------------------------------------------------------------------------------

void TransferFunctionTest::nearestColorTest()
{
    data::TransferFunction::sptr tf = this->createTFColor();

    // Value = -40.33 => color : {0.9, 0.2, 0.3, 0.4}
    // Value = -0.2  => color : {0.1, 0.9, 0.3, 0.4}
    // Value = 3     => color : {0.1, 0.2, 0.9, 0.4}
    // Value = 150   => color : {0.1, 0.2, 0.3, 0.9}

    tf->setClamped(true);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sampleNearest(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sampleNearest(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == tf->sampleNearest(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sampleNearest(150));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sampleNearest(((150. - 3.) / 2.) + 3 + .1));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == tf->sampleNearest(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sampleNearest(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sampleNearest(((150. - 3.) / 4.) + 3));

    tf->setInterpolationMode(TransferFunction::InterpolationMode::NEAREST);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sample(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sample(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == tf->sample(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sample(150));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sample(((150. - 3.) / 2.) + 3 + 0.1));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == tf->sample(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sample(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sample(((150. - 3.) / 4.) + 3));

    tf->setClamped(false);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == tf->sampleNearest(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sampleNearest(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == tf->sampleNearest(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sampleNearest(150));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sampleNearest(((150. - 3.) / 2.) + 3 + .1));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == tf->sampleNearest(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sampleNearest(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sampleNearest(((150. - 3.) / 4.) + 3));
}

//------------------------------------------------------------------------------

void TransferFunctionTest::mapValuesTest()
{
    auto tf = data::TransferFunction::New();

    tf->setClamped(false);
    tf->setWindowMinMax({-200, 300.});
    tf->insert({0., {0., 0., 0., 0.}});
    tf->insert({1., {1., 1., 1., 1.}});

    CPPUNIT_ASSERT_EQUAL(-200., tf->mapValueToWindow(0.));
    CPPUNIT_ASSERT_EQUAL(50., tf->mapValueToWindow(0.5));
    CPPUNIT_ASSERT_EQUAL(300., tf->mapValueToWindow(1.));

    CPPUNIT_ASSERT_EQUAL(0., tf->mapValueFromWindow(-200.));
    CPPUNIT_ASSERT_EQUAL(.5, tf->mapValueFromWindow(50.));
    CPPUNIT_ASSERT_EQUAL(1., tf->mapValueFromWindow(300.));

    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sampleNearest(-200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sampleNearest(0));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 1.0, 1.0) == tf->sampleNearest(55));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 1.0, 1.0) == tf->sampleNearest(100));

    // Insert a point that changes the maximum value
    tf->insert({2., {1., 1., 0., 1.}});

    CPPUNIT_ASSERT_EQUAL(-200., tf->mapValueToWindow(0.));
    CPPUNIT_ASSERT_EQUAL(-75., tf->mapValueToWindow(0.5));
    CPPUNIT_ASSERT_EQUAL(50., tf->mapValueToWindow(1.));
    CPPUNIT_ASSERT_EQUAL(300., tf->mapValueToWindow(2.));

    CPPUNIT_ASSERT_EQUAL(0., tf->mapValueFromWindow(-200.));
    CPPUNIT_ASSERT_EQUAL(.5, tf->mapValueFromWindow(-75.));
    CPPUNIT_ASSERT_EQUAL(1., tf->mapValueFromWindow(50.));
    CPPUNIT_ASSERT_EQUAL(2., tf->mapValueFromWindow(300.));

    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sampleNearest(-200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 1.0, 1.0) == tf->sampleNearest(0));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, .5, 1.0) == tf->sampleLinear(175));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 0., 1.0) == tf->sampleNearest(300.));
}

//------------------------------------------------------------------------------

void TransferFunctionTest::piecewiseFunctionTest()
{
    auto tf = data::TransferFunction::New();

    auto& pieces = tf->pieces();

    pieces.emplace_back(data::TransferFunctionData::New());

    pieces[0]->setClamped(false);
    pieces[0]->setWindowMinMax({-10, 0.});
    pieces[0]->insert({-10.0, {0.0, 0.0, 0.0, 0.0}}); // 1
    pieces[0]->insert({-5.0, {1.0, 0.3, 0.0, 1.0}});  // 2
    pieces[0]->insert({0.0, {0.0, 0.0, 1.0, 0.4}});   // 3

    CPPUNIT_ASSERT(tf->empty());

    tf->mergePieces();

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), tf->size());
    CPPUNIT_ASSERT_EQUAL(-5., tf->level());
    CPPUNIT_ASSERT_EQUAL(10., tf->window());

    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 0.0, 0.0), tf->find(-10.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.3, 0.0, 1.0), tf->find(-5.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 1.0, 0.4), tf->find(0)->second);

    pieces[0]->setClamped(true);

    tf->mergePieces();

    // The clamp add two boundary points at +/-0.5
    CPPUNIT_ASSERT_EQUAL(std::size_t(5), tf->size());
    CPPUNIT_ASSERT_EQUAL(-5., tf->level());
    CPPUNIT_ASSERT_EQUAL(11.0, tf->window());

    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 0.0, 0.0), tf->find(-10.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.3, 0.0, 1.0), tf->find(-5.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 1.0, 0.4), tf->find(0)->second);

    /// Additional values for the clamp
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 0.0, 0.0), tf->begin()->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 0.0, 0.0), tf->rbegin()->second);

    pieces.emplace_back(data::TransferFunctionData::New());

    pieces[1]->setClamped(false);
    pieces[1]->setWindowMinMax({0, 100.});
    pieces[1]->insert({0.0, {0.0, 0.6, 0.0, 0.5}});   // 4
    pieces[1]->insert({50.0, {1.0, 1.0, 0.0, 1.0}});  // 5
    pieces[1]->insert({100.0, {1.0, 0.0, 1.0, 0.5}}); // 6

    tf->mergePieces();
    CPPUNIT_ASSERT_EQUAL(std::size_t(7), tf->size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(44.75, tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(110.5, tf->window(), s_EPSILON);

    // Yields 1 and 4 because the second tf is unclamped
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.6, 0.0, 0.5), tf->find(-10.)->second);
    // Yields 2 and 4 because the second tf is unclamped
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.6, 0.0, 1.0), tf->find(-5.)->second);
    // Yields 3 and exactly 4
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.6, 0.8, 0.5), tf->find(0.)->second);
    // Yields exactly 5
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 1.0, 0.0, 1.0), tf->find(50.)->second);
    // Yields exactly 6
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.0, 1.0, 0.5), tf->find(100.)->second);

    // Test windowing
    tf->setLevel(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200., tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(110.5, tf->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(150.25, pieces[0]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10., pieces[0]->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(205.25, pieces[1]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100., pieces[1]->window(), s_EPSILON);

    tf->setWindow(55);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200., tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55., tf->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(150.25, pieces[0]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.97737, pieces[0]->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(205.25, pieces[1]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(49.77375, pieces[1]->window(), s_EPSILON);
}

} //namespace ut

} //namespace sight::data

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

namespace sight::data::ut
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
    data::TransferFunction::sptr tf = data::TransferFunction::New();

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong level ", 0.0, tf->level());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong window", 2.0, tf->window());

    CPPUNIT_ASSERT_EQUAL(std::string(), tf->name());
    CPPUNIT_ASSERT(TransferFunction::color_t() == tf->backgroundColor());

    auto tfData = tf->pieces().emplace_back(data::TransferFunctionPiece::New());

    CPPUNIT_ASSERT_EQUAL(TransferFunction::InterpolationMode::LINEAR, tfData->interpolationMode());
    CPPUNIT_ASSERT_EQUAL(true, tfData->clamped());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), tfData->size());
}

//------------------------------------------------------------------------------
void TransferFunctionTest::defaultTfTest()
{
    data::TransferFunction::csptr tf = data::TransferFunction::createDefaultTF();

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", 50.0, tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", 500.0, tf->window(), s_EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong level ", 50.0, tf->pieces()[0]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Wrong window", 500.0, tf->pieces()[0]->window(), s_EPSILON);

    CPPUNIT_ASSERT_EQUAL(TransferFunction::s_DEFAULT_TF_NAME, tf->name());
    CPPUNIT_ASSERT(TransferFunction::color_t() == tf->backgroundColor());

    const auto firstPiece = tf->pieces().front();

    CPPUNIT_ASSERT_EQUAL(TransferFunction::InterpolationMode::LINEAR, firstPiece->interpolationMode());
    CPPUNIT_ASSERT_EQUAL(false, firstPiece->clamped());
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), firstPiece->size());
}

//------------------------------------------------------------------------------

void TransferFunctionTest::classicGetSetTest()
{
    data::TransferFunction::sptr tf = data::TransferFunction::createDefaultTF();

    // Test getTFData()
    const auto tfData                    = tf->pieces().front();
    auto itr                             = tfData->cbegin();
    const TransferFunction::value_t key1 = itr->first;
    const TransferFunction::value_t key2 = (++itr)->first;

    auto itrColor                          = tfData->cbegin();
    const TransferFunction::color_t color1 = itrColor->second;
    const TransferFunction::color_t color2 = (++itrColor)->second;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, key1, 1e-10);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, key2, 1e-10);

    const TransferFunction::color_t expectedColor1(0.0, 0.0, 0.0, 0.0);
    const TransferFunction::color_t expectedColor2(1.0, 1.0, 1.0, 1.0);
    CPPUNIT_ASSERT(expectedColor1 == color1);
    CPPUNIT_ASSERT(expectedColor2 == color2);
    CPPUNIT_ASSERT(expectedColor1 == tfData->find(key1)->second);

    // Test erase
    tfData->erase(1.0);
    CPPUNIT_ASSERT_EQUAL((std::size_t) 1, tfData->size());

    // Test clear()
    tfData->clear();
    const std::size_t expectedClearedSize = 0;
    CPPUNIT_ASSERT_EQUAL(expectedClearedSize, tfData->size());
}

//------------------------------------------------------------------------------

void TransferFunctionTest::shallowAndDeepCopyTest()
{
    const data::TransferFunction::sptr tf = sight::data::ut::TransferFunctionTest::createTFColor();
    tf->setLevel(900.6);
    tf->setWindow(-200.02);

    sight::data::ut::TransferFunctionTest::checkTFColor(tf);

    const data::TransferFunction::sptr shallowCopyTf = data::TransferFunction::New();
    shallowCopyTf->shallowCopy(tf);
    sight::data::ut::TransferFunctionTest::checkTFColor(shallowCopyTf);
    CPPUNIT_ASSERT_EQUAL(tf->pieces()[0], shallowCopyTf->pieces()[0]);

    const data::TransferFunction::sptr deepCopyTf = data::Object::copy(tf);
    sight::data::ut::TransferFunctionTest::checkTFColor(deepCopyTf);
    // Pointer addresses must be different
    CPPUNIT_ASSERT(tf->pieces()[0] != deepCopyTf->pieces()[0]);
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr TransferFunctionTest::createTFColor()
{
    data::TransferFunction::sptr tf = data::TransferFunction::New();

    tf->setBackgroundColor(data::TransferFunction::color_t(1.0, 0.3, 0.6, 0.1));
    tf->setName("color_t");
    tf->setWindowMinMax({-40.33, 150.});

    auto tfData = tf->pieces().emplace_back(data::TransferFunctionPiece::New());
    tfData->setClamped(false);
    tfData->setInterpolationMode(data::TransferFunction::InterpolationMode::NEAREST);
    tfData->setWindowMinMax({-40.33, 150.});

    tfData->insert({-40.33, data::TransferFunction::color_t(0.9, 0.2, 0.3, 0.4)});
    tfData->insert({3, data::TransferFunction::color_t(0.1, 0.2, 0.9, 0.4)}); // Invert point 3 <=> -0.2, for
    // tests
    tfData->insert({-0.2, data::TransferFunction::color_t(0.1, 0.9, 0.3, 0.4)});
    tfData->insert({150, data::TransferFunction::color_t(0.1, 0.2, 0.3, 0.9)});

    data::String::sptr myString = data::String::New("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

void TransferFunctionTest::checkTFColor(data::TransferFunction::sptr tf)
{
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 0.3, 0.6, 0.1) == tf->backgroundColor());
    CPPUNIT_ASSERT_EQUAL(std::string("color_t"), tf->name());

    CPPUNIT_ASSERT_EQUAL(-200.02, tf->window());
    CPPUNIT_ASSERT_EQUAL(900.6, tf->level());

    const auto tfData = tf->pieces().front();
    CPPUNIT_ASSERT_EQUAL(false, tfData->clamped());
    CPPUNIT_ASSERT_EQUAL(data::TransferFunction::InterpolationMode::NEAREST, tfData->interpolationMode());

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), tfData->size());
    CPPUNIT_ASSERT_EQUAL(-40.33, tfData->minMax().first);
    CPPUNIT_ASSERT_EQUAL(150., tfData->minMax().second);

    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.9, 0.2, 0.3, 0.4) == tfData->find(-40.33)->second);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.1, 0.9, 0.3, 0.4) == tfData->find(-0.2)->second);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.1, 0.2, 0.9, 0.4) == tfData->find(3)->second);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(0.1, 0.2, 0.3, 0.9) == tfData->find(150)->second);

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

    auto piece = tf->pieces()[0];
    data::TransferFunction::data_t tfData;
    tfData[-40.33] = data::TransferFunction::color_t(0.9, 0.2, 0.3, 0.4);
    tfData[3]      = data::TransferFunction::color_t(0.1, 0.2, 0.9, 0.4);
    tfData[-0.2]   = data::TransferFunction::color_t(0.1, 0.9, 0.3, 0.4);
    tfData[150]    = data::TransferFunction::color_t(0.1, 0.2, 0.3, 0.9);
    std::copy(tfData.begin(), tfData.end(), inserter(*piece));

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
    auto piece = tf->pieces()[0];

    piece->setClamped(true);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), piece->sampleLinear(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), piece->sampleLinear(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), piece->sampleLinear(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), piece->sampleLinear(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), piece->sampleLinear(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), piece->sampleLinear(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), piece->sampleLinear(3));
    ASSERT_COLOR_EQUALS(
        data::TransferFunction::color_t(.1, .2, .75, .525),
        piece->sampleLinear(((150. - 3.) / 4.) + 3)
    );

    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sampleLinear(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sampleLinear(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), tf->sampleLinear(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), tf->sampleLinear(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), tf->sampleLinear(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), tf->sampleLinear(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), tf->sampleLinear(3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .75, .525), tf->sampleLinear(((150. - 3.) / 4.) + 3));

    piece->setInterpolationMode(TransferFunction::InterpolationMode::LINEAR);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), piece->sample(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), piece->sample(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), piece->sample(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), piece->sample(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), piece->sample(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), piece->sample(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), piece->sample(3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .75, .525), piece->sample(((150. - 3.) / 4.) + 3));

    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sample(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.0, .0, .0, .0), tf->sample(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), tf->sample(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), tf->sample(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), tf->sample(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), tf->sample(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), tf->sample(3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .75, .525), tf->sample(((150. - 3.) / 4.) + 3));

    piece->setClamped(false);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), piece->sampleLinear(-120));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), piece->sampleLinear(200));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.9, .2, .3, .4), piece->sampleLinear(-40.33));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .3, .9), piece->sampleLinear(150));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .6, .65), piece->sampleLinear(((150. - 3.) / 2.) + 3));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .9, .3, .4), piece->sampleLinear(-0.2));
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(.1, .2, .9, .4), piece->sampleLinear(3));
    ASSERT_COLOR_EQUALS(
        data::TransferFunction::color_t(.1, .2, .75, .525),
        piece->sampleLinear(((150. - 3.) / 4.) + 3)
    );

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

    auto piece = tf->pieces()[0];

    piece->setClamped(true);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == piece->sampleNearest(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == piece->sampleNearest(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == piece->sampleNearest(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == piece->sampleNearest(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::color_t(
            .1,
            .2,
            .3,
            .9
        ) == piece->sampleNearest(((150. - 3.) / 2.) + 3 + .1)
    );
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == piece->sampleNearest(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == piece->sampleNearest(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == piece->sampleNearest(((150. - 3.) / 4.) + 3));

    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sampleNearest(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sampleNearest(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == tf->sampleNearest(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sampleNearest(150));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sampleNearest(((150. - 3.) / 2.) + 3 + .1));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == tf->sampleNearest(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sampleNearest(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sampleNearest(((150. - 3.) / 4.) + 3));

    piece->setInterpolationMode(TransferFunction::InterpolationMode::NEAREST);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == piece->sample(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == piece->sample(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == piece->sample(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == piece->sample(150));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == piece->sample(((150. - 3.) / 2.) + 3 + 0.1));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == piece->sample(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == piece->sample(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == piece->sample(((150. - 3.) / 4.) + 3));

    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sample(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tf->sample(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == tf->sample(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sample(150));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == tf->sample(((150. - 3.) / 2.) + 3 + 0.1));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == tf->sample(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sample(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == tf->sample(((150. - 3.) / 4.) + 3));

    piece->setClamped(false);
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == piece->sampleNearest(-120));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == piece->sampleNearest(200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.9, .2, .3, .4) == piece->sampleNearest(-40.33));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .3, .9) == piece->sampleNearest(150));
    CPPUNIT_ASSERT(
        data::TransferFunction::color_t(
            .1,
            .2,
            .3,
            .9
        ) == piece->sampleNearest(((150. - 3.) / 2.) + 3 + .1)
    );
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .9, .3, .4) == piece->sampleNearest(-0.2));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == piece->sampleNearest(3));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.1, .2, .9, .4) == piece->sampleNearest(((150. - 3.) / 4.) + 3));

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
    const auto tfData = data::TransferFunctionPiece::New();

    tfData->setClamped(false);
    tfData->setWindowMinMax({-200, 300.});
    tfData->insert({0., {0., 0., 0., 0.}});
    tfData->insert({1., {1., 1., 1., 1.}});

    CPPUNIT_ASSERT_EQUAL(-200., tfData->mapValueToWindow(0.));
    CPPUNIT_ASSERT_EQUAL(50., tfData->mapValueToWindow(0.5));
    CPPUNIT_ASSERT_EQUAL(300., tfData->mapValueToWindow(1.));

    CPPUNIT_ASSERT_EQUAL(0., tfData->mapValueFromWindow(-200.));
    CPPUNIT_ASSERT_EQUAL(.5, tfData->mapValueFromWindow(50.));
    CPPUNIT_ASSERT_EQUAL(1., tfData->mapValueFromWindow(300.));

    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tfData->sampleNearest(-200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tfData->sampleNearest(0));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 1.0, 1.0) == tfData->sampleNearest(55));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 1.0, 1.0) == tfData->sampleNearest(100));

    // Insert a point that changes the maximum value
    tfData->insert({2., {1., 1., 0., 1.}});

    CPPUNIT_ASSERT_EQUAL(-200., tfData->mapValueToWindow(0.));
    CPPUNIT_ASSERT_EQUAL(-75., tfData->mapValueToWindow(0.5));
    CPPUNIT_ASSERT_EQUAL(50., tfData->mapValueToWindow(1.));
    CPPUNIT_ASSERT_EQUAL(300., tfData->mapValueToWindow(2.));

    CPPUNIT_ASSERT_EQUAL(0., tfData->mapValueFromWindow(-200.));
    CPPUNIT_ASSERT_EQUAL(.5, tfData->mapValueFromWindow(-75.));
    CPPUNIT_ASSERT_EQUAL(1., tfData->mapValueFromWindow(50.));
    CPPUNIT_ASSERT_EQUAL(2., tfData->mapValueFromWindow(300.));

    CPPUNIT_ASSERT(data::TransferFunction::color_t(.0, .0, .0, .0) == tfData->sampleNearest(-200));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 1.0, 1.0) == tfData->sampleNearest(0));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, .5, 1.0) == tfData->sampleLinear(175));
    CPPUNIT_ASSERT(data::TransferFunction::color_t(1.0, 1.0, 0., 1.0) == tfData->sampleNearest(300.));
}

//------------------------------------------------------------------------------

void TransferFunctionTest::piecewiseFunctionTest()
{
    auto tf = data::TransferFunction::New();

    auto piece_0 = data::TransferFunctionPiece::New();

    CPPUNIT_ASSERT(tf->empty());

    auto& pieces = tf->pieces();
    pieces.emplace_back(piece_0);

    piece_0->setClamped(false);
    piece_0->setWindowMinMax({-10, 0.});
    piece_0->insert({-10.0, {0.0, 0.0, 0.0, 0.0}}); // 1
    piece_0->insert({-5.0, {1.0, 0.3, 0.0, 1.0}});  // 2
    piece_0->insert({0.0, {0.0, 0.0, 1.0, 0.4}});   // 3

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), piece_0->size());
    CPPUNIT_ASSERT_EQUAL(-5., piece_0->level());
    CPPUNIT_ASSERT_EQUAL(10., piece_0->window());

    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 0.0, 0.0), piece_0->find(-10.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.3, 0.0, 1.0), piece_0->find(-5.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 1.0, 0.4), piece_0->find(0)->second);

    piece_0->setClamped(true);

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), piece_0->size());
    CPPUNIT_ASSERT_EQUAL(-5., piece_0->level());
    CPPUNIT_ASSERT_EQUAL(10.0, piece_0->window());

    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 0.0, 0.0), piece_0->find(-10.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.3, 0.0, 1.0), piece_0->find(-5.)->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 1.0, 0.4), piece_0->find(0)->second);

    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 0.0, 0.0), piece_0->begin()->second);
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.0, 1.0, 0.4), piece_0->rbegin()->second);

    auto piece_1 = data::TransferFunctionPiece::New();
    pieces.emplace_back(piece_1);

    piece_1->setClamped(false);
    piece_1->setWindowMinMax({0, 100.});
    piece_1->insert({0.0, {0.0, 0.6, 0.0, 0.5}});   // 4
    piece_1->insert({50.0, {1.0, 1.0, 0.0, 1.0}});  // 5
    piece_1->insert({100.0, {1.0, 0.0, 1.0, 0.5}}); // 6

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), piece_1->size());

    tf->fitWindow();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(45.0, tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(110.0, tf->window(), s_EPSILON);

    // Yields 1 and 4 because the second tf is unclamped
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.6, 0.0, 0.5), tf->sample(-10.));
    // Yields 2 and 4 because the second tf is unclamped
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.6, 0.0, 1.0), tf->sample(-5.));
    // Yields 3 and exactly 4
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(0.0, 0.6, 0.8, 0.5), tf->sample(0.));
    // Yields exactly 5
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 1.0, 0.0, 1.0), tf->sample(50.));
    // Yields exactly 6
    ASSERT_COLOR_EQUALS(data::TransferFunction::color_t(1.0, 0.0, 1.0, 0.5), tf->sample(100.));

    // Test windowing
    tf->setLevel(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200.0, tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(110.0, tf->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(150.0, pieces[0]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, pieces[0]->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(205.0, pieces[1]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, pieces[1]->window(), s_EPSILON);

    tf->setWindow(55);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200.0, tf->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(55.0, tf->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(150.0, pieces[0]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, pieces[0]->window(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(205.0, pieces[1]->level(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0, pieces[1]->window(), s_EPSILON);
}

//------------------------------------------------------------------------------

void TransferFunctionTest::equalityTest()
{
    auto function1 = data::TransferFunction::New();
    auto function2 = data::TransferFunction::New();

    CPPUNIT_ASSERT(*function1 == *function2 && !(*function1 != *function2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    function1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Transfer functions should be different when using " #op " on the first one", \
        *function1 != *function2 && !(*function1 == *function2) \
    ); \
    function2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Transfer functions should be equal when using " #op " on both", \
        *function1 == *function2 && !(*function1 != *function2) \
    );

    // TransferFunction::setWindow and TransferFunction::setLevel aren't tested here, as the behavior is
    // counterintuitive: The equality of the TransferFunction isn't changed by setWindow et setLevel if the list of
    // TransferFunctionPiece is empty. Whether this is the correct behavior or not is still to be determined.

    TEST(setName("3"));
    TEST(setBackgroundColor({4, 5, 6, 7}));
    TEST(pieces().push_back(data::TransferFunctionPiece::New()));

    #undef TEST
}

} // namespace sight::data::ut

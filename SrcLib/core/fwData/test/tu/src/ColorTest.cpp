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

#include "fwData/Color.hpp"
#include "ColorTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ColorTest );

void ColorTest::setUp()
{
    // Set up context before running a test.

}
void ColorTest::tearDown()
{
    // Clean up after the test run.
}

void ColorTest::methode1()
{
    //-----------test values
    const float R = 0.2f;
    const float G = 0.8f;
    const float B = 0.5f;
    const float A = 0.6f;

    //::fwData::Color::sptr color(new ::fwData::Color(R, G, B, A));

    //CPPUNIT_ASSERT_EQUAL(color->red(), R);
    //CPPUNIT_ASSERT_EQUAL(color->green(), G);
    //CPPUNIT_ASSERT_EQUAL(color->blue(), B);
    //CPPUNIT_ASSERT_EQUAL(color->alpha(), A);
}

void ColorTest::methode2()
{
    //-----------test values
    //const float R = 0.2f;
    //const float G = 0.8f;
    //const float B = 0.5f;
    //const float A = 0.6f;

    //::fwData::Color::NewSptr color;

    //::fwData::Color::ColorArray array;
    //array[0] = R;
    //array[1] = G;
    //array[2] = B;
    //array[3] = A;

    //color->setCRefRGBA(array);

    //CPPUNIT_ASSERT_EQUAL(color->getRGBA()[0], R);
    //CPPUNIT_ASSERT_EQUAL(color->getRGBA()[1], G);
    //CPPUNIT_ASSERT_EQUAL(color->getRGBA()[2], B);
    //CPPUNIT_ASSERT_EQUAL(color->getRGBA()[3], A);
}



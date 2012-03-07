/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/random/Generator.hpp>

#include "RandomTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::RandomTest );

namespace fwTools
{
namespace ut
{

void RandomTest::setUp()
{
    // Set up context before running a test.
}

void RandomTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void RandomTest::generatorTest()
{
    float minF=10.f, maxF=20.f;
    float randF = ::fwTools::random::getValue(minF, maxF);
    CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", randF>=minF && randF<=maxF);

    int minI=10, maxI=12;
    int randI = ::fwTools::random::getValue(minI, maxI);
    CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", randI>=minI && randI<=maxI);
}

//------------------------------------------------------------------------------

void RandomTest::randomListTest()
{
    float minF=10.f, maxF=20.f;
    std::vector<float> randF(10);
    ::fwTools::random::fillContainer(minF, maxF, randF);
    for(size_t i = 0; i <randF.size(); ++i)
    {
        CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", randF[i]>=minF && randF[i]<=maxF);
    }

    int minI=10, maxI=20;
    std::vector<int> randI(10);
    ::fwTools::random::fillContainer(minI, maxI, randI);
    for(size_t i = 0; i <randF.size(); ++i)
    {
        CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", randI[i]>=minI && randI[i]<=maxI);
    }
}

//------------------------------------------------------------------------------

void RandomTest::seedTest()
{
    ::boost::uint32_t seedVal = 42;

    float minF=10.f, maxF=20.f;
    float randF1 = ::fwTools::random::getValue(minF, maxF, seedVal);
    CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", randF1>=minF && randF1<=maxF);
    float randF2 = ::fwTools::random::getValue(minF, maxF, seedVal);
    CPPUNIT_ASSERT_EQUAL(randF1, randF2);

    int minI=10, maxI=20;
    std::vector<int> randI1(10);
    std::vector<int> randI2(10);
    ::fwTools::random::fillContainer(minI, maxI, randI1, seedVal);
    ::fwTools::random::fillContainer(minI, maxI, randI2, seedVal);
    for(size_t i = 0; i <randI1.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(randI1[i], randI2[i]);
    }
}

} // namespace ut
} // namespace fwTools


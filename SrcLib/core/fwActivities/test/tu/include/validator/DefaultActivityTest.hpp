/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_UT_VALIDATOR_DEFAULTACTIVITYTEST_HPP__
#define __FWACTIVITIES_UT_VALIDATOR_DEFAULTACTIVITYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwActivities/registry/Activities.hpp>

#include <fwCore/macros.hpp>

#include <fwRuntime/Bundle.hpp>

namespace fwActivities
{

namespace ut
{

class DefaultActivityTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DefaultActivityTest );

CPPUNIT_TEST(requirementsTest);
CPPUNIT_TEST(parametersTest);
CPPUNIT_TEST(objectTest);
CPPUNIT_TEST(validatorTest);
CPPUNIT_TEST(bigActivityTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void parametersTest();
    void requirementsTest();
    void objectTest();
    void validatorTest();
    void bigActivityTest();

protected:
    SPTR( ::fwRuntime::Bundle ) m_bundle;
};

} //namespace ut
} //namespace fwActivities

#endif //__FWACTIVITIES_UT_VALIDATOR_DEFAULTACTIVITYTEST_HPP__


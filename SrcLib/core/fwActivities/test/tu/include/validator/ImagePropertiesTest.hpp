/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_UT_VALIDATOR_IMAGEPROPERTIESTEST_HPP__
#define __FWACTIVITIES_UT_VALIDATOR_IMAGEPROPERTIESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwActivities
{

namespace ut
{

class ImagePropertiesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImagePropertiesTest );

CPPUNIT_TEST(propertiesTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void propertiesTest();

};

} //namespace ut
} //namespace fwActivities

#endif //__FWACTIVITIES_UT_VALIDATOR_IMAGEPROPERTIESTEST_HPP__


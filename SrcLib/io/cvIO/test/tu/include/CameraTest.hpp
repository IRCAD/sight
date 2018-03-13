/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace cvIO
{

namespace ut
{

/**
 * @brief Intrinsic camera calibration conversion tests.
 */
class CameraTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE( CameraTest );
    CPPUNIT_TEST(copyToCv);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void copyToCv();
};

}

} // namespace cvIO

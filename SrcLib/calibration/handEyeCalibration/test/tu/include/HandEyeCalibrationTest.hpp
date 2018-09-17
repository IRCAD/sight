/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace handEyeCalibration
{
namespace ut
{
/**
 * @brief HandEyeCalibrationTest tests the handEyeCalibration API
 */
class HandEyeCalibrationTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( HandEyeCalibrationTest );
    CPPUNIT_TEST( checkCalibration);
    CPPUNIT_TEST( checkMultipleCalibration );
    CPPUNIT_TEST( checkApi );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void checkCalibration();
    void checkMultipleCalibration();

    void checkApi();
};

} // namespace ut
} // namespace handEyeCalibration

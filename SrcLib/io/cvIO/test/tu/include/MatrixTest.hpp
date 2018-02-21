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
 * @brief Tests the ::cvIO::Matrix functions
 */
class MatrixTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE( MatrixTest );
    CPPUNIT_TEST(copyFromCvFloat);
    CPPUNIT_TEST(copyToCvFloat);
    CPPUNIT_TEST(copyFromCvDouble);
    CPPUNIT_TEST(copyToCvDouble);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    ///Tests the copy from/to ::cv::Matx44f (float)
    void copyFromCvFloat();
    void copyToCvFloat();
    ///Tests the copy from/to ::cv::Matx44d (double)
    void copyFromCvDouble();
    void copyToCvDouble();

};

}

} // namespace cvIO

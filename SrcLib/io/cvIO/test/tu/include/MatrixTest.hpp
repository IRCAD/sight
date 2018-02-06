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
 * @brief The MatrixTest class
 */
class MatrixTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE( MatrixTest );
    CPPUNIT_TEST(copyFromCv);
    CPPUNIT_TEST(copyToCv);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void copyFromCv();
    void copyToCv();
};

}

} // namespace cvIO

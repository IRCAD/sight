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
 * @brief Point list conversion tests.
 */
class PointListTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE( PointListTest );
    CPPUNIT_TEST(copyFromCv);
    CPPUNIT_TEST(copyToCv);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void copyFromCv();
    void copyToCv();
};

}

} // namespace cvIO

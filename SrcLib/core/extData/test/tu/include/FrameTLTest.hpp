/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_UT_FRAMETLTEST_HPP__
#define __EXTDATA_UT_FRAMETLTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace extData
{
namespace ut
{

class FrameTLTest : public CPPUNIT_NS::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( FrameTLTest );
    CPPUNIT_TEST( pushTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void pushTest();
};

} //namespace ut
} //namespace extData
#endif // __EXTDATA_UT_FRAMETLTEST_HPP__

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_UT_RAWBUFFERTLTEST_HPP__
#define __ARDATA_UT_RAWBUFFERTLTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace arData
{
namespace ut
{

class RawBufferTLTest : public CPPUNIT_NS::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( RawBufferTLTest );
    CPPUNIT_TEST( pushTest );
    CPPUNIT_TEST( getObjectTest );
    CPPUNIT_TEST( setObjectTest );
    CPPUNIT_TEST( modifyTimeTest );
    CPPUNIT_TEST( copyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void pushTest();
    void getObjectTest();
    void setObjectTest();
    void modifyTimeTest();
    void copyTest();
};

} //namespace ut
} //namespace arData
#endif // __ARDATA_UT_RAWBUFFERTLTEST_HPP__

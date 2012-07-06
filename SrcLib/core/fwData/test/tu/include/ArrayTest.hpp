/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_ARRAYTEST_HPP__
#define __FWDATA_UT_ARRAYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class ArrayTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ArrayTest );
    CPPUNIT_TEST( allocation );
    CPPUNIT_TEST( resize );
    CPPUNIT_TEST( reallocate );
    CPPUNIT_TEST( copy );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void allocation();
    void resize();
    void reallocate();
    void copy();
};
} //namespace ut
} //namespace fwData
#endif // __FWDATA_UT_ARRAYTEST_HPP__

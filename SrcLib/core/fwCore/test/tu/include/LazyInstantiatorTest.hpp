/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_UT_LAZYINSTANTIATORTEST_HPP__
#define __FWCORE_UT_LAZYINSTANTIATORTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCore
{
namespace ut
{

class LazyInstantiatorTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( LazyInstantiatorTest );
    CPPUNIT_TEST( lazyTest );
    CPPUNIT_TEST( threadSafeTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void lazyTest();
    void threadSafeTest();

};

} //namespace ut
} //namespace fwCore
#endif __FWCORE_UT_LAZYINSTANTIATORTEST_HPP__

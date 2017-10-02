/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_UT_RUNTIMETEST_HPP__
#define __FWRUNTIME_UT_RUNTIMETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwRuntime
{
namespace ut
{

/**
 * @brief   Test Runtime : read the bundles in CommonLib
 */
class RuntimeTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( RuntimeTest );
CPPUNIT_TEST( testRuntime );
CPPUNIT_TEST( testOperations );
CPPUNIT_TEST_SUITE_END();

public:
    RuntimeTest();

    // interface
    void setUp();
    void tearDown();

    void testRuntime();
    void testOperations();
};

} // namespace ut
} // namespace fwTools

#endif //__FWRUNTIME_UT_RUNTIMETEST_HPP__

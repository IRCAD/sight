/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_UT_IODICOMEXTTEST_HPP__
#define __IODICOMEXT_UT_IODICOMEXTTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioDicomExt
{
namespace ut
{

/**
 * @brief Test PacsConfigurationInitializer controller
 */
class IoDicomExtTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( IoDicomExtTest );
CPPUNIT_TEST( pacsConfigurationInitializer );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void pacsConfigurationInitializer();
};

} //namespace ut
} //namespace ioDicomExt

#endif //__IODICOMEXT_UT_IODICOMEXTTEST_HPP__

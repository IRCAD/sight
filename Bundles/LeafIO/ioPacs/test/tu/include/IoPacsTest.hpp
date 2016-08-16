/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_UT_IOPACSTEST_HPP__
#define __IOPACS_UT_IOPACSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioPacs
{
namespace ut
{

/**
 * @brief Test PacsConfigurationInitializer controller
 */
class IoPacsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( IoPacsTest );
CPPUNIT_TEST( pacsConfigurationInitializer );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void pacsConfigurationInitializer();
};

} //namespace ut
} //namespace ioPacs

#endif //__IOPACS_UT_IOPACSTEST_HPP__

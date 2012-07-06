/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_UT_ECONFIGURATIONELEMENTTEST_HPP_
#define __FWRUNTIME_UT_ECONFIGURATIONELEMENTTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

namespace fwRuntime
{
namespace ut
{

/**
 * @brief   Test the creation of EConfigurationElement
 */
class EConfigurationElementTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( EConfigurationElementTest );
    CPPUNIT_TEST( testConfiguration );
    CPPUNIT_TEST_SUITE_END();

protected:

public:
    // interface
    void setUp();
    void tearDown();

    void testConfiguration();
};

} // namespace ut
} // namespace fwTools

#endif //__FWRUNTIME_UT_ECONFIGURATIONELEMENTTEST_HPP_

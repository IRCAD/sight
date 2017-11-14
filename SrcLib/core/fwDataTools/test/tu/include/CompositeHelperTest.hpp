/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_COMPOSITEHELPERTEST_HPP__
#define __FWDATATOOLS_UT_COMPOSITEHELPERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

/**
 * @brief   Test ::fwDataTools::helper::Composite methods.
 */
class CompositeHelperTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( CompositeHelperTest );
CPPUNIT_TEST( testHelper );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testHelper();
};

} //namespace ut
} //namespace fwDataTools

#endif // __FWDATATOOLS_UT_COMPOSITEHELPERTEST_HPP__

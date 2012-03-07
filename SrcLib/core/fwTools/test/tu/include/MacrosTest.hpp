/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_UT_MACROSTEST_HPP__
#define __FWTOOLS_UT_MACROSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwTools
{
namespace ut
{

class MacrosTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( MacrosTest );
    CPPUNIT_TEST( factoryMacroTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void factoryMacroTest();
};

} // namespace ut
} // namespace fwTools

#endif //__FWTOOLS_UT_MACROSTEST_HPP__

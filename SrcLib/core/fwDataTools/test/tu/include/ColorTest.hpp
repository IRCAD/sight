/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_COLORTEST_HPP__
#define __FWDATATOOLS_UT_COLORTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

class ColorTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( ColorTest );
    CPPUNIT_TEST( hexaStringToRGBA );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void hexaStringToRGBA();
};
} //namespace ut
} //namespace fwDataTools

#endif //__FWDATATOOLS_UT_COLORTEST_HPP__

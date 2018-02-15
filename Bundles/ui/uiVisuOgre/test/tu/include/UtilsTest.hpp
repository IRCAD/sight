/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_UT_UTILSTEST_HPP__
#define __UIVISUOGRE_UT_UTILSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace uiVisuOgre
{
namespace ut
{

class UtilsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( UtilsTest );
CPPUNIT_TEST( convertOgreColorToQColor );
CPPUNIT_TEST( convertQColorToOgreColor );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void convertOgreColorToQColor();
    void convertQColorToOgreColor();
};

} //namespace ut
} //namespace uiVisuOgre

#endif // __UIVISUOGRE_UT_UTILSTEST_HPP__

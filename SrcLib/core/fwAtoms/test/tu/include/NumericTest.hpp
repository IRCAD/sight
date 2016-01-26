/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_UT_NUMERICTEST_HPP__
#define __FWATOMS_UT_NUMERICTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwAtoms
{
namespace ut
{

class NumericTest :  public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( NumericTest );
CPPUNIT_TEST( emptyTest );
CPPUNIT_TEST( intTest );
CPPUNIT_TEST( floatingTest );
CPPUNIT_TEST( mixedTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void emptyTest();
    void intTest();
    void floatingTest();
    void mixedTest();

};

}  // namespace ut
}  // namespace fwAtoms



#endif // __FWATOMS_UT_NUMERICTEST_HPP__


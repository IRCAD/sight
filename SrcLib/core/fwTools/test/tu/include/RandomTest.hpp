/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_UT_RANDOMTEST_HPP__
#define __FWTOOLS_UT_RANDOMTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwTools
{
namespace ut
{

class RandomTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( RandomTest );
    CPPUNIT_TEST( generatorTest );
    CPPUNIT_TEST( randomListTest );
    CPPUNIT_TEST( seedTest );
    CPPUNIT_TEST_SUITE_END();

protected:

public:
    // interface
    void setUp();
    void tearDown();

    void generatorTest();
    void randomListTest();
    void seedTest();
};

} // namespace ut
} // namespace fwTools

#endif // __FWTOOLS_UT_RANDOMTEST_HPP__

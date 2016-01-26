/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_UT_NUMERICROUNDCASTTEST_HPP__
#define __FWTOOLS_UT_NUMERICROUNDCASTTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwTools
{
namespace ut
{

class NumericRoundCastTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( NumericRoundCastTest );
CPPUNIT_TEST( castFloatToIntTest );
CPPUNIT_TEST( castDoubleToIntTest );
CPPUNIT_TEST( castIntToFloatTest );
CPPUNIT_TEST( castDoubleToFloatTest );
CPPUNIT_TEST( castIntToIntTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void castFloatToIntTest();
    void castDoubleToIntTest();
    void castIntToFloatTest();
    void castDoubleToFloatTest();
    void castIntToIntTest();
};

} // namespace ut
} // namespace fwTools

#endif // __FWTOOLS_UT_NUMERICROUNDCASTTEST_HPP__

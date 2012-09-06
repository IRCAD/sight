/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETADATA_UT_NUMERIC_HPP__
#define __FWMETADATA_UT_NUMERIC_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMetaData
{
namespace ut
{

class NumericTest :  public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( NumericTest );
    CPPUNIT_TEST( intConversion );
    CPPUNIT_TEST( uintConversion );
    CPPUNIT_TEST( int64Conversion );
    CPPUNIT_TEST( uint64Conversion );
    CPPUNIT_TEST( floatConversion );
    CPPUNIT_TEST( doubleConversion );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void intConversion();
    void uintConversion();
    void int64Conversion();
    void uint64Conversion();
    void floatConversion();
    void doubleConversion();

};


}  // namespace ut
}  // namespace fwMetaData



#endif // __FWMETADATA_UT_NUMERIC_HPP__


/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_UT_GENERICTLTEST_HPP__
#define __EXTDATA_UT_GENERICTLTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace extData
{
namespace ut
{

class GenericTLTest : public CPPUNIT_NS::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( GenericTLTest );
    CPPUNIT_TEST( pushTest );
    CPPUNIT_TEST( pushClassTest );
    CPPUNIT_TEST( copyTest );
    CPPUNIT_TEST( iteratorTest );
    CPPUNIT_TEST( objectValid );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void pushTest();
    void pushClassTest();
    void copyTest();
    void iteratorTest();
    void objectValid();
};

} //namespace ut
} //namespace extData
#endif // __EXTDATA_UT_GENERICTLTEST_HPP__

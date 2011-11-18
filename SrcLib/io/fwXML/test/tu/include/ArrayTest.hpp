/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_TEST_TU_ARRAYTEST_HPP_
#define _FWXML_TEST_TU_ARRAYTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

class ArrayTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ArrayTest );
    CPPUNIT_TEST( testArray );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testArray();

private:

    void compareBuffer(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2);

    ::fwData::Array::sptr generateArray();

};

#endif // _FWXML_TEST_TU_ARRAYTEST_HPP_

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_GRAPHTEST_HPP__
#define __FWDATA_UT_GRAPHTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class GraphTest : public CPPUNIT_NS::TestFixture {

    CPPUNIT_TEST_SUITE( GraphTest );
    CPPUNIT_TEST( normalCase );
    CPPUNIT_TEST( limitCase1 );
    CPPUNIT_TEST( limitCase2 );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void normalCase();
    void limitCase1();
    void limitCase2();
};

} //namespace ut
} //namespace fwData
#endif //__FWDATA_UT_GRAPHTEST_HPP__

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_UT_DEMANGLERTEST_HPP__
#define __FWCORE_UT_DEMANGLERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <sstream>

namespace fwCore
{
namespace ut
{

class DemanglerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DemanglerTest );
CPPUNIT_TEST( classNameTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void classNameTest();
};

} //namespace ut
} //namespace fwCore

#endif //__FWCORE_UT_DEMANGLERTEST_HPP__

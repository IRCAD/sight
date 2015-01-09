/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_UT_ZIPTEST_HPP__
#define __FWZIP_UT_ZIPTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwZip
{
namespace ut
{

class ZipTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ZipTest );
    CPPUNIT_TEST( commentTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void commentTest();
};

} // namespace ut
} // namespace fwZip

#endif //__FWZIP_UT_ZIPTEST_HPP__

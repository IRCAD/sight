/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_UT_BYTESIZE_HPP__
#define __FWTOOLS_UT_BYTESIZE_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwTools
{
namespace ut
{

class ByteSizeTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ByteSizeTest );
    CPPUNIT_TEST( byteSizeTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void byteSizeTest();
};

} // namespace ut
} // namespace fwTools

#endif //__FWTOOLS_UT_BYTESIZE_HPP__


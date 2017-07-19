/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWITKIO_UT_TRANSFORMTEST_HPP__
#define __FWITKIO_UT_TRANSFORMTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwItkIO
{
namespace ut
{

class TransformTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( TransformTest );
CPPUNIT_TEST( testConversion );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void testConversion();

};

} //namespace ut
} //namespace fwItkIO

#endif // __FWITKIO_UT_TRANSFORMTEST_HPP__

/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDATA_UT_IMAGETEST_HPP__
#define __FWDATA_UT_IMAGETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class ImageTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( ImageTest );
    CPPUNIT_TEST( testGetterSetter );
    CPPUNIT_TEST( testAllocation );
    CPPUNIT_TEST( testReallocation );
    CPPUNIT_TEST( testPixelType );
    CPPUNIT_TEST( testSetGetPixel );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testGetterSetter();
    void testAllocation();
    void testReallocation();
    void testPixelType();
    void testSetGetPixel();
};

} //namespace ut
} //namespace fwData
#endif //__FWDATA_UT_IMAGETEST_HPP__

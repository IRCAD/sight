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

#ifndef __FWDATA_UT_MESHTEST_HPP__
#define __FWDATA_UT_MESHTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class MeshTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( MeshTest );
    CPPUNIT_TEST( allocation );
    CPPUNIT_TEST( insertion );
    CPPUNIT_TEST( colorsNormals );
    CPPUNIT_TEST( texCoords );
    CPPUNIT_TEST( addingArray );
    CPPUNIT_TEST( copy );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void allocation();
    void insertion();
    void colorsNormals();
    void texCoords();
    void addingArray();
    void copy();
};
} //namespace ut
} //namespace fwData
#endif // __FWDATA_UT_MESHTEST_HPP__

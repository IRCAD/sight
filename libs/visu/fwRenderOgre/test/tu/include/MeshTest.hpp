/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#ifndef __FWRENDEROGRE_UT_MESHTEST_HPP__
#define __FWRENDEROGRE_UT_MESHTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwRenderOgre
{
namespace helper
{

namespace ut
{

class MeshTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MeshTest );
CPPUNIT_TEST( copyColors );
CPPUNIT_TEST_SUITE_END();

public:
    MeshTest();
    virtual ~MeshTest();
    // interface
    void setUp();
    void tearDown();

    void copyColors();

private:
};

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_UT_MESHTEST_HPP__

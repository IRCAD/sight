/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __ARSTRUCTURALPATCH_UT_ARDATA_CAMERA_V3TOV2TEST_HPP__
#define __ARSTRUCTURALPATCH_UT_ARDATA_CAMERA_V3TOV2TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace arStructuralPatch
{
namespace ut
{
namespace arData
{
namespace Camera
{

/**
 * @brief Test patch to convert fwData::Reconstruction from version 3 to version 2.
 */
class V3ToV2Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( V3ToV2Test );
CPPUNIT_TEST( applyPatchTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTest();
};

} //namespace Camera
} //namespace arData
} //namespace ut
} //namespace arStructuralPatch

#endif //__ARSTRUCTURALPATCH_UT_ARDATA_CAMERA_V3TOV2TEST_HPP__

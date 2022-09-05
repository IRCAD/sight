/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::filter::mesh::ut
{

/**
 * @brief Test apply the threshold value on the image.
 */
class SVTKMesherTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SVTKMesherTest);
CPPUNIT_TEST(generateMesh);
CPPUNIT_TEST(generateMeshWithMinReduction);
CPPUNIT_TEST(noMeshGenerated);
CPPUNIT_TEST(updateThresholdTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    static void generateMesh();
    static void generateMeshWithMinReduction();
    static void noMeshGenerated();
    static void updateThresholdTest();
};

} // namespace sight::module::filter::mesh::ut

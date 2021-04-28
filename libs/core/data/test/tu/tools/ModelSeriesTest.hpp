/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

namespace sight::data::tools
{
namespace ut
{

class ModelSeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ModelSeriesTest );
CPPUNIT_TEST(addReconstruction);
CPPUNIT_TEST(addMesh);
CPPUNIT_TEST(createReconstructionFromMesh);
CPPUNIT_TEST_SUITE_END();

public:
    /**
     * @name Override
     * @{ */
    void setUp();
    void tearDown();
    /**  @} */

    void addReconstruction();
    void createReconstructionFromMesh();
    void addMesh();
};

} // namespace ut

} // namespace sight::data::tools
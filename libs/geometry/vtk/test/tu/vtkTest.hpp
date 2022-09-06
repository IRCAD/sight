/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include <core/tools/random/Generator.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <algorithm>
#include <cmath>

namespace sight::geometry::vtk::ut
{

class vtkTest : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(vtkTest);
    CPPUNIT_TEST(computeCenterOfMass);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void computeCenterOfMass();

    //------------------------------------------------------------------------------

    template<class T>
    static T random(const T& a, const T& b)
    {
        return static_cast<T>(core::tools::random::safeRand()) / static_cast<T>(RAND_MAX) * (b - a) + a;
    }
};

} // namespace sight::geometry::vtk::ut

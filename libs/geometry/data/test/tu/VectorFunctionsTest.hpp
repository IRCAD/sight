/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::geometry::data::ut
{

class VectorFunctionsTest : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(VectorFunctionsTest);
    CPPUNIT_TEST(checkNormalize);
    CPPUNIT_TEST(checkDot);
    CPPUNIT_TEST(checkCross);
    CPPUNIT_TEST(checkNegate);
    CPPUNIT_TEST(checkVecLength);
    CPPUNIT_TEST(checkOperators);
    CPPUNIT_TEST_SUITE_END();

protected:

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void checkDot();
    static void checkCross();
    static void checkNormalize();
    static void checkNegate();
    static void checkVecLength();
    static void checkOperators();
};

} // namespace sight::geometry::data::ut

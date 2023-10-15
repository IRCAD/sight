/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

namespace sight::service::ut
{

class slots_signals_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(slots_signals_test);
    CPPUNIT_TEST(basicTest);
    CPPUNIT_TEST(comObjectServiceTest);
    CPPUNIT_TEST(comServiceToServiceTest);
    CPPUNIT_TEST(blockConnectionTest);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void basicTest();
    static void comObjectServiceTest();
    static void comServiceToServiceTest();
    static void blockConnectionTest();
};

} // namespace sight::service::ut

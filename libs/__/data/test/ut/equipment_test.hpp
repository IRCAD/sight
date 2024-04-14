/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <data/series.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class equipment_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(equipment_test);
CPPUNIT_TEST(institution_name_test);
CPPUNIT_TEST(equality_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void institution_name_test();
    static void equality_test();

private:

    series::sptr m_equipment;
};

} // namespace sight::data::ut

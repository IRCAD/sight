/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <core/object.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::core::tools::ut
{

class uuidtest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(uuidtest);
CPPUNIT_TEST(object_uuidtest);
CPPUNIT_TEST(conccurent_access_on_uuid_map_test);
CPPUNIT_TEST(conccurent_access_on_same_obj_uuidtest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void object_uuidtest();

    static void conccurent_access_on_uuid_map_test();
    void conccurent_access_on_same_obj_uuidtest();

private:

    static void run_uuidcreation();
    void run_access_to_object_uuid();

    core::object::sptr m_object;
    std::string m_uuid;
};

} // namespace sight::core::tools::ut

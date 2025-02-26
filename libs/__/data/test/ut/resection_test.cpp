/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "resection_test.hpp"

#include <data/reconstruction.hpp>
#include <data/resection.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::resection_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void resection_test::equality_test()
{
    auto resection1 = std::make_shared<data::resection>();
    auto resection2 = std::make_shared<data::resection>();

    CPPUNIT_ASSERT(*resection1 == *resection2 && !(*resection1 != *resection2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            resection1->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Resections should be different when using " #op " on the first one", \
                *resection1 != *resection2 && !(*resection1 == *resection2) \
            ); \
            resection2->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Resections should be equal when using " #op " on both", \
                *resection1 == *resection2 && !(*resection1 != *resection2) \
            );

    auto plane_list = std::make_shared<data::plane_list>();
    plane_list->set_planes({std::make_shared<data::plane>()});
    TEST(set_plane_list(plane_list));
    TEST(set_inputs({std::make_shared<data::reconstruction>()}));
    TEST(set_outputs({std::make_shared<data::reconstruction>()}));
    TEST(set_is_safe_part(false));
    TEST(set_name("1"));
    TEST(set_is_visible(false));
    TEST(set_is_valid(true));

    #undef TEST
}

} // namespace sight::data::ut

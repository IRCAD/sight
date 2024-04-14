/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "structure_traits_test.hpp"

#include <data/structure_traits.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::structure_traits_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void structure_traits_test::equality_test()
{
    auto struct1 = std::make_shared<data::structure_traits>();
    auto struct2 = std::make_shared<data::structure_traits>();

    CPPUNIT_ASSERT(*struct1 == *struct2 && !(*struct1 != *struct2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    struct1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Structures should be different when using " #op " on the first one", \
        *struct1 != *struct2 && !(*struct1 == *struct2) \
    ); \
    struct2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Structures should be equal when using " #op " on both", \
        *struct1 == *struct2 && !(*struct1 != *struct2) \
    );

    TEST(set_type("1"));
    TEST(set_categories({data::structure_traits::body}));
    TEST(set_class(data::structure_traits::tool));
    TEST(set_native_exp("2"));
    TEST(set_native_geometric_exp("3"));
    TEST(set_attachment_type("4"));
    TEST(set_color(std::make_shared<data::color>(5.F, 6.F, 7.F)));
    TEST(set_anatomic_region("8"));
    TEST(set_property_category("9"));
    TEST(set_property_type("10"));

    #undef TEST
}

} // namespace sight::data::ut

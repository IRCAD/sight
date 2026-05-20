#include "utest_data/generator/series_set.hpp"
#include <doctest/doctest.h>

/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/series.hpp>

TEST_SUITE("sight::data::equipment")
{
//------------------------------------------------------------------------------

    TEST_CASE("institution_name")
    {
        const auto equipment               = std::make_shared<sight::data::series>();
        const std::string institution_name = "IRCAD";
        CHECK(equipment);
        equipment->set_institution_name(institution_name);

        CHECK_EQ(institution_name, equipment->get_institution_name());
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto equipment1 = std::make_shared<sight::data::series>();
        auto equipment2 = std::make_shared<sight::data::series>();

        CHECK(*equipment1 == *equipment2);

        equipment1->set_institution_name("IRCAD");
        CHECK(*equipment1 != *equipment2);
        equipment2->set_institution_name(equipment1->get_institution_name());
        CHECK(*equipment1 == *equipment2);

        // Test also deepcopy, just for fun
        auto dicom_series3 = std::make_shared<sight::data::series>();
        dicom_series3->deep_copy(equipment1);
        CHECK(*equipment1 == *dicom_series3);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::equipment")

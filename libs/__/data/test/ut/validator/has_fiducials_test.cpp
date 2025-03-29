/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "has_fiducials_test.hpp"

#include <data/image_series.hpp>
#include <data/validator/base.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::validator::ut::has_fiducials_test);

namespace sight::data::validator::ut
{

namespace factory = sight::data::validator::factory;

//------------------------------------------------------------------------------

void has_fiducials_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void has_fiducials_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void has_fiducials_test::image_series()
{
    auto validator = factory::make("sight::data::validator::has_fiducials");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::data::validator::return_t validation;
    data::image_series::sptr image_series = std::make_shared<data::image_series>();

    {
        validation = obj_validator->validate(image_series);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }
    {
        // Adds a dummy fiducial set
        data::fiducials_series::fiducial_set fiducial_set;
        data::fiducials_series::fiducial fiducial;
        fiducial.shape_type           = data::fiducials_series::shape::point;
        fiducial.fiducial_description = "6";
        fiducial.fiducial_identifier  = "7";

        fiducial.fiducial_uid = "14";
        fiducial.contour_data = {{15, 16, 17}};
        fiducial_set.fiducial_sequence.push_back(fiducial);

        fiducial_set.group_name = "18";
        fiducial_set.color      = {{19, 20, 21, 22}};
        fiducial_set.size       = 23.F;
        fiducial_set.shape      = data::fiducials_series::private_shape::cube;
        fiducial_set.visibility = true;

        image_series->get_fiducials()->set_fiducial_sets({fiducial_set});
        validation = obj_validator->validate(image_series);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::validator::ut

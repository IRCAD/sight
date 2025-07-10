/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#include "contain_one_skin_test.hpp"

#include <data/map.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/validator/base.hpp>
#include <data/vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::validator::ut::contain_one_skin_test);

namespace sight::data::validator::ut
{

namespace factory = sight::data::validator::factory;
using sight::data::validator::base;
using sight::data::validator::base;

//------------------------------------------------------------------------------

void contain_one_skin_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void contain_one_skin_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void contain_one_skin_test::test_validator()
{
    auto validator = factory::make("sight::data::validator::model_series::contain_one_skin");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::data::validator::return_t validation;

    data::model_series::sptr model_series = std::make_shared<data::model_series>();
    data::reconstruction::sptr rec1       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec2       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec3       = std::make_shared<data::reconstruction>();
    data::model_series::reconstruction_vector_t vec_rec;

    {
        validation = obj_validator->validate(rec1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Validator on ohter data should NOT be valid", false, validation.first);
    }
    {
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty model series should NOT be valid", false, validation.first);
    }
    {
        vec_rec.push_back(rec1);
        model_series->set_reconstruction_db(vec_rec);
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with one non-Skin reconstruction should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec1->set_structure_type("Skin");
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with one Skin reconstruction should be valid",
            true,
            validation.first
        );
    }
    {
        vec_rec.push_back(rec2);
        model_series->set_reconstruction_db(vec_rec);
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with two reconstructions (only one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        vec_rec.push_back(rec3);
        model_series->set_reconstruction_db(vec_rec);
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (only one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec2->set_structure_type("Skin");
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (two Skin) should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec1->set_structure_type("Bones");
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (only one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec2->set_structure_type("Tumor");
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (no Skin) should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void contain_one_skin_test::test_validator_with_vector()
{
    auto validator = factory::make("sight::data::validator::model_series::contain_one_skin");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::data::validator::return_t validation;

    data::vector::sptr vector = std::make_shared<data::vector>();

    data::model_series::sptr model_series1 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series2 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series3 = std::make_shared<data::model_series>();
    data::reconstruction::sptr rec11       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec12       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec21       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec22       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec31       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec32       = std::make_shared<data::reconstruction>();
    data::model_series::reconstruction_vector_t vec_rec1;
    data::model_series::reconstruction_vector_t vec_rec2;
    data::model_series::reconstruction_vector_t vec_rec3;

    vec_rec1.push_back(rec11);
    vec_rec1.push_back(rec12);
    vec_rec2.push_back(rec21);
    vec_rec2.push_back(rec22);
    vec_rec3.push_back(rec31);
    vec_rec3.push_back(rec32);

    model_series1->set_reconstruction_db(vec_rec1);
    model_series2->set_reconstruction_db(vec_rec2);
    model_series3->set_reconstruction_db(vec_rec3);

    {
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector series should be valid", true, validation.first);
    }
    {
        vector->push_back(model_series1);
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (without Skin) should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec11->set_structure_type("Skin");
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (with one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec12->set_structure_type("Skin");
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (with two Skin) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec12->set_structure_type("Bones");
        vector->push_back(model_series2);
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (one without Skin) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec21->set_structure_type("Skin");
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (each with one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec22->set_structure_type("Skin");
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (one with one Skin and one with two skin) should NOT "
            "be valid",
            false,
            validation.first
        );
    }
    {
        rec22->set_structure_type("Bone");
        vector->push_back(model_series3);
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (two with one Skin and one without skin) should NOT "
            "be valid",
            false,
            validation.first
        );
    }
    {
        rec32->set_structure_type("Skin");
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (each with one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec21->set_structure_type("Tumor");
        validation = obj_validator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (two with one Skin and one without skin) should NOT "
            "be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void contain_one_skin_test::test_validator_with_map()
{
    auto validator = factory::make("sight::data::validator::model_series::contain_one_skin");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::data::validator::return_t validation;

    data::map::sptr map = std::make_shared<data::map>();

    data::model_series::sptr model_series1 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series2 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series3 = std::make_shared<data::model_series>();
    data::reconstruction::sptr rec11       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec12       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec21       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec22       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec31       = std::make_shared<data::reconstruction>();
    data::reconstruction::sptr rec32       = std::make_shared<data::reconstruction>();
    data::model_series::reconstruction_vector_t vec_rec1;
    data::model_series::reconstruction_vector_t vec_rec2;
    data::model_series::reconstruction_vector_t vec_rec3;

    vec_rec1.push_back(rec11);
    vec_rec1.push_back(rec12);
    vec_rec2.push_back(rec21);
    vec_rec2.push_back(rec22);
    vec_rec3.push_back(rec31);
    vec_rec3.push_back(rec32);

    model_series1->set_reconstruction_db(vec_rec1);
    model_series2->set_reconstruction_db(vec_rec2);
    model_series3->set_reconstruction_db(vec_rec3);

    {
        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Map series should be valid", true, validation.first);
    }
    {
        (*map)["model1"] = model_series1;
        validation       = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (without Skin) should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec11->set_structure_type("Skin");
        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with one ModelSeries (with one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec12->set_structure_type("Skin");
        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with one ModelSeries (with two Skin) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec12->set_structure_type("Bones");
        (*map)["model2"] = model_series2;
        validation       = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with two ModelSeries (one without Skin) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec21->set_structure_type("Skin");
        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with two ModelSeries (each with one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec22->set_structure_type("Skin");
        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with two ModelSeries (one with one Skin and one with two skin) should "
            " NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec22->set_structure_type("Bone");
        (*map)["model3"] = model_series3;
        validation       = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with three ModelSeries (two with one Skin and one without skin) should "
            "NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec32->set_structure_type("Skin");
        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with three ModelSeries (each with one Skin) should be valid",
            true,
            validation.first
        );
    }
    {
        rec21->set_structure_type("Tumor");
        validation = obj_validator->validate(map);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Map with three ModelSeries (two with one Skin and one without skin) should "
            "NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::validator::ut

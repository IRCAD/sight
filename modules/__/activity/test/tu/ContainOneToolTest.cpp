/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ContainOneToolTest.hpp"

#include <activity/validator/base.hpp>
#include <activity/validator/object.hpp>

#include <data/Composite.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::activity::validator::ut::ContainOneToolTest);

namespace sight::module::activity::validator::ut
{

namespace factory = sight::activity::validator::factory;
using sight::activity::validator::object;
using sight::activity::validator::base;

//------------------------------------------------------------------------------

void ContainOneToolTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ContainOneToolTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ContainOneToolTest::testValidator()
{
    auto validator = factory::make("sight::module::activity::validator::ModelSeries::ContainOneTool");
    CPPUNIT_ASSERT(validator);

    auto objValidator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(objValidator);

    sight::activity::validator::return_t validation;

    data::ModelSeries::sptr modelSeries = std::make_shared<data::ModelSeries>();
    data::Reconstruction::sptr rec1     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec2     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec3     = std::make_shared<data::Reconstruction>();
    data::ModelSeries::ReconstructionVectorType vect_rec;

    {
        validation = objValidator->validate(rec1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Validator on ohter data should NOT be valid", false, validation.first);
    }
    {
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty model series should NOT be valid", false, validation.first);
    }
    {
        vect_rec.push_back(rec1);
        modelSeries->setReconstructionDB(vect_rec);
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with one non-Tool reconstruction should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec1->setStructureType("Tool");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with one Tool reconstruction should be valid",
            true,
            validation.first
        );
    }
    {
        vect_rec.push_back(rec2);
        modelSeries->setReconstructionDB(vect_rec);
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with two reconstructions (only one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        vect_rec.push_back(rec3);
        modelSeries->setReconstructionDB(vect_rec);
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (only one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec2->setStructureType("Tool");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (two Tool) should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec1->setStructureType("Bones");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (only one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec2->setStructureType("Tumor");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with three reconstructions (no Tool) should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void ContainOneToolTest::testValidatorWithVector()
{
    auto validator = factory::make("sight::module::activity::validator::ModelSeries::ContainOneTool");
    CPPUNIT_ASSERT(validator);

    auto objValidator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(objValidator);

    sight::activity::validator::return_t validation;

    data::Vector::sptr vector = std::make_shared<data::Vector>();

    data::ModelSeries::sptr modelSeries1 = std::make_shared<data::ModelSeries>();
    data::ModelSeries::sptr modelSeries2 = std::make_shared<data::ModelSeries>();
    data::ModelSeries::sptr modelSeries3 = std::make_shared<data::ModelSeries>();
    data::Reconstruction::sptr rec11     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec12     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec21     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec22     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec31     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec32     = std::make_shared<data::Reconstruction>();
    data::ModelSeries::ReconstructionVectorType vec_rec1;
    data::ModelSeries::ReconstructionVectorType vec_rec2;
    data::ModelSeries::ReconstructionVectorType vec_rec3;

    vec_rec1.push_back(rec11);
    vec_rec1.push_back(rec12);
    vec_rec2.push_back(rec21);
    vec_rec2.push_back(rec22);
    vec_rec3.push_back(rec31);
    vec_rec3.push_back(rec32);

    modelSeries1->setReconstructionDB(vec_rec1);
    modelSeries2->setReconstructionDB(vec_rec2);
    modelSeries3->setReconstructionDB(vec_rec3);

    {
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector series should be valid", true, validation.first);
    }
    {
        vector->push_back(modelSeries1);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (without Tool) should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec11->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (with one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec12->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (with two Tool) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec12->setStructureType("Bones");
        vector->push_back(modelSeries2);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (one without Tool) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (each with one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec22->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (one with one Tool and one with two skin) should NOT "
            "be valid",
            false,
            validation.first
        );
    }
    {
        rec22->setStructureType("Bone");
        vector->push_back(modelSeries3);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (two with one Tool and one without skin) should NOT "
            "be valid",
            false,
            validation.first
        );
    }
    {
        rec32->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (each with one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tumor");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (two with one Tool and one without skin) should NOT "
            "be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void ContainOneToolTest::testValidatorWithComposite()
{
    auto validator = factory::make("sight::module::activity::validator::ModelSeries::ContainOneTool");
    CPPUNIT_ASSERT(validator);

    auto objValidator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(objValidator);

    sight::activity::validator::return_t validation;

    data::Composite::sptr composite = std::make_shared<data::Composite>();

    data::ModelSeries::sptr modelSeries1 = std::make_shared<data::ModelSeries>();
    data::ModelSeries::sptr modelSeries2 = std::make_shared<data::ModelSeries>();
    data::ModelSeries::sptr modelSeries3 = std::make_shared<data::ModelSeries>();
    data::Reconstruction::sptr rec11     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec12     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec21     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec22     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec31     = std::make_shared<data::Reconstruction>();
    data::Reconstruction::sptr rec32     = std::make_shared<data::Reconstruction>();
    data::ModelSeries::ReconstructionVectorType vec_rec1;
    data::ModelSeries::ReconstructionVectorType vec_rec2;
    data::ModelSeries::ReconstructionVectorType vec_rec3;

    vec_rec1.push_back(rec11);
    vec_rec1.push_back(rec12);
    vec_rec2.push_back(rec21);
    vec_rec2.push_back(rec22);
    vec_rec3.push_back(rec31);
    vec_rec3.push_back(rec32);

    modelSeries1->setReconstructionDB(vec_rec1);
    modelSeries2->setReconstructionDB(vec_rec2);
    modelSeries3->setReconstructionDB(vec_rec3);

    {
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Composite series should be valid", true, validation.first);
    }
    {
        (*composite)["model1"] = modelSeries1;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (without Tool) should NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec11->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with one ModelSeries (with one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec12->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with one ModelSeries (with two Tool) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec12->setStructureType("Bones");
        (*composite)["model2"] = modelSeries2;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with two ModelSeries (one without Tool) should be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with two ModelSeries (each with one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec22->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with two ModelSeries (one with one Tool and one with two skin) should "
            " NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec22->setStructureType("Bone");
        (*composite)["model3"] = modelSeries3;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with three ModelSeries (two with one Tool and one without skin) should "
            "NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec32->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with three ModelSeries (each with one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tumor");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with three ModelSeries (two with one Tool and one without skin) should "
            "NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::activity::validator::ut

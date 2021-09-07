/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <activity/IObjectValidator.hpp>
#include <activity/IValidator.hpp>

#include <data/Composite.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::module::activity::validator::ut::ContainOneToolTest);

namespace sight::module::activity::validator
{

namespace ut
{

namespace factory = sight::activity::validator::factory;
using sight::activity::IObjectValidator;
using sight::activity::IValidator;

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
    auto validator = factory::New("::sight::module::activity::validator::ModelSeries::ContainOneTool");
    CPPUNIT_ASSERT(validator);

    auto objValidator = IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    IValidator::ValidationType validation;

    data::ModelSeries::sptr modelSeries = data::ModelSeries::New();
    data::Reconstruction::sptr rec1     = data::Reconstruction::New();
    data::Reconstruction::sptr rec2     = data::Reconstruction::New();
    data::Reconstruction::sptr rec3     = data::Reconstruction::New();
    data::ModelSeries::ReconstructionVectorType vecReconst;

    {
        validation = objValidator->validate(rec1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Validator on ohter data should NOT be valid", false, validation.first);
    }
    {
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty model series should NOT be valid", false, validation.first);
    }
    {
        vecReconst.push_back(rec1);
        modelSeries->setReconstructionDB(vecReconst);
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
        vecReconst.push_back(rec2);
        modelSeries->setReconstructionDB(vecReconst);
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "ModelSeries with two reconstructions (only one Tool) should be valid",
            true,
            validation.first
        );
    }
    {
        vecReconst.push_back(rec3);
        modelSeries->setReconstructionDB(vecReconst);
        validation = objValidator->validate(modelSeries);
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
    auto validator = factory::New("::sight::module::activity::validator::ModelSeries::ContainOneTool");
    CPPUNIT_ASSERT(validator);

    auto objValidator = IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    IValidator::ValidationType validation;

    data::Vector::sptr vector = data::Vector::New();

    data::ModelSeries::sptr modelSeries1 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();
    data::Reconstruction::sptr rec11     = data::Reconstruction::New();
    data::Reconstruction::sptr rec12     = data::Reconstruction::New();
    data::Reconstruction::sptr rec21     = data::Reconstruction::New();
    data::Reconstruction::sptr rec22     = data::Reconstruction::New();
    data::Reconstruction::sptr rec31     = data::Reconstruction::New();
    data::Reconstruction::sptr rec32     = data::Reconstruction::New();
    data::ModelSeries::ReconstructionVectorType vecReconst1;
    data::ModelSeries::ReconstructionVectorType vecReconst2;
    data::ModelSeries::ReconstructionVectorType vecReconst3;

    vecReconst1.push_back(rec11);
    vecReconst1.push_back(rec12);
    vecReconst2.push_back(rec21);
    vecReconst2.push_back(rec22);
    vecReconst3.push_back(rec31);
    vecReconst3.push_back(rec32);

    modelSeries1->setReconstructionDB(vecReconst1);
    modelSeries2->setReconstructionDB(vecReconst2);
    modelSeries3->setReconstructionDB(vecReconst3);

    {
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector series should be valid", true, validation.first);
    }
    {
        vector->getContainer().push_back(modelSeries1);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (without Tool) shoud NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec11->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (with one Tool) shoud be valid",
            true,
            validation.first
        );
    }
    {
        rec12->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (with two Tool) shoud be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec12->setStructureType("Bones");
        vector->getContainer().push_back(modelSeries2);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (one without Tool) shoud be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (each with one Tool) shoud be valid",
            true,
            validation.first
        );
    }
    {
        rec22->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with two ModelSeries (one with one Tool and one with two skin) shoud NOT "
            "be valid",
            false,
            validation.first
        );
    }
    {
        rec22->setStructureType("Bone");
        vector->getContainer().push_back(modelSeries3);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (two with one Tool and one without skin) shoud NOT "
            "be valid",
            false,
            validation.first
        );
    }
    {
        rec32->setStructureType("Tool");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (each with one Tool) shoud be valid",
            true,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tumor");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with three ModelSeries (two with one Tool and one without skin) shoud NOT "
            "be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void ContainOneToolTest::testValidatorWithComposite()
{
    auto validator = factory::New("::sight::module::activity::validator::ModelSeries::ContainOneTool");
    CPPUNIT_ASSERT(validator);

    auto objValidator = IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    IValidator::ValidationType validation;

    data::Composite::sptr composite = data::Composite::New();

    data::ModelSeries::sptr modelSeries1 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();
    data::Reconstruction::sptr rec11     = data::Reconstruction::New();
    data::Reconstruction::sptr rec12     = data::Reconstruction::New();
    data::Reconstruction::sptr rec21     = data::Reconstruction::New();
    data::Reconstruction::sptr rec22     = data::Reconstruction::New();
    data::Reconstruction::sptr rec31     = data::Reconstruction::New();
    data::Reconstruction::sptr rec32     = data::Reconstruction::New();
    data::ModelSeries::ReconstructionVectorType vecReconst1;
    data::ModelSeries::ReconstructionVectorType vecReconst2;
    data::ModelSeries::ReconstructionVectorType vecReconst3;

    vecReconst1.push_back(rec11);
    vecReconst1.push_back(rec12);
    vecReconst2.push_back(rec21);
    vecReconst2.push_back(rec22);
    vecReconst3.push_back(rec31);
    vecReconst3.push_back(rec32);

    modelSeries1->setReconstructionDB(vecReconst1);
    modelSeries2->setReconstructionDB(vecReconst2);
    modelSeries3->setReconstructionDB(vecReconst3);

    {
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Composite series should be valid", true, validation.first);
    }
    {
        (*composite)["model1"] = modelSeries1;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Vector with one ModelSeries (without Tool) shoud NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec11->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with one ModelSeries (with one Tool) shoud be valid",
            true,
            validation.first
        );
    }
    {
        rec12->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with one ModelSeries (with two Tool) shoud be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec12->setStructureType("Bones");
        (*composite)["model2"] = modelSeries2;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with two ModelSeries (one without Tool) shoud be NOT valid",
            false,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with two ModelSeries (each with one Tool) shoud be valid",
            true,
            validation.first
        );
    }
    {
        rec22->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with two ModelSeries (one with one Tool and one with two skin) shoud "
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
            "Composite with three ModelSeries (two with one Tool and one without skin) shoud "
            "NOT be valid",
            false,
            validation.first
        );
    }
    {
        rec32->setStructureType("Tool");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with three ModelSeries (each with one Tool) shoud be valid",
            true,
            validation.first
        );
    }
    {
        rec21->setStructureType("Tumor");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Composite with three ModelSeries (two with one Tool and one without skin) shoud "
            "NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::module::activity::validator

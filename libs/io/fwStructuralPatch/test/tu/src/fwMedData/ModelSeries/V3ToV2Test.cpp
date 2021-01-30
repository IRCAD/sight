/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwMedData/ModelSeries/V3ToV2Test.hpp"

#include <fwStructuralPatch/fwMedData/ModelSeries/V3ToV2.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwMedData::ModelSeries::V3ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace ModelSeries
{

//------------------------------------------------------------------------------

void V3ToV2Test::setUp()
{
}

//------------------------------------------------------------------------------

void V3ToV2Test::tearDown()
{
}

//------------------------------------------------------------------------------

void V3ToV2Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr modelSeriesV3 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr modelSeriesV2;

    ::fwAtomsPatch::helper::setClassname(modelSeriesV3, "::fwMedData::ModelSeries");
    ::fwAtomsPatch::helper::setVersion(modelSeriesV3, "3");

    ::fwAtomsPatch::helper::Object helper(modelSeriesV3);

    helper.addAttribute("patient", ::fwAtoms::Object::sptr());
    helper.addAttribute("study", ::fwAtoms::Object::sptr());
    helper.addAttribute("equipment", ::fwAtoms::Object::sptr());
    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    helper.addAttribute("dicom_reference", ::fwAtoms::Object::sptr());
    helper.addAttribute("reconstruction_db", ::fwAtoms::Object::sptr());

    helper.addAttribute("number", ::fwAtoms::String::New(""));
    helper.addAttribute("laterality", ::fwAtoms::String::New(""));
    helper.addAttribute("protocolName", ::fwAtoms::String::New(""));
    helper.addAttribute("body_part_examined", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_position", ::fwAtoms::String::New(""));
    helper.addAttribute("anatomical_orientation_type", ::fwAtoms::String::New(""));
    helper.addAttribute("performded_procedure_step_id", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_date", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_time", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_date", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_time", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_step_description", ::fwAtoms::String::New(""));
    helper.addAttribute("performed_procedure_comments", ::fwAtoms::String::New(""));

    {
        modelSeriesV2 = ::fwAtoms::Object::dynamicCast(modelSeriesV3->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[modelSeriesV3] = modelSeriesV2;

        ::fwStructuralPatch::fwMedData::ModelSeries::V3ToV2::sptr v3ToV2Patch;
        v3ToV2Patch = ::fwStructuralPatch::fwMedData::ModelSeries::V3ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v3ToV2Patch->apply(modelSeriesV3, modelSeriesV2, newVersions));

        CPPUNIT_ASSERT(modelSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), ::fwAtomsPatch::helper::getVersion(modelSeriesV2));
        const auto attributes = modelSeriesV2->getAttributes();
        CPPUNIT_ASSERT(attributes.find("number") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("laterality") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("protocolName") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("body_part_examined") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_position") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("anatomical_orientation_type") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("performded_procedure_step_id") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_start_date") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_start_time") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_end_date") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_end_time") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_description") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_comments") == attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace ModelSeries
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch

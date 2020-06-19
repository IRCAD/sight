/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "fwMedData/Study/V2ToV1Test.hpp"

#include <fwStructuralPatch/fwMedData/Study/V2ToV1.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwMedData::Study::V2ToV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace Study
{

//------------------------------------------------------------------------------

void V2ToV1Test::setUp()
{
}

//------------------------------------------------------------------------------

void V2ToV1Test::tearDown()
{
}

//------------------------------------------------------------------------------

void V2ToV1Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr StudyV2 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr StudyV1;

    ::fwAtomsPatch::helper::setClassname(StudyV2, "::fwMedData::Study");
    ::fwAtomsPatch::helper::setVersion(StudyV2, "2");

    ::fwAtomsPatch::helper::Object helper(StudyV2);

    helper.addAttribute("patient", ::fwAtoms::Object::sptr());
    helper.addAttribute("study", ::fwAtoms::Object::sptr());
    helper.addAttribute("equipment", ::fwAtoms::Object::sptr());
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));

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
    helper.addAttribute("study_id", ::fwAtoms::String::New(""));
    helper.addAttribute("consulting_physician_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("patient_size", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_weight", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_body_mass_index", ::fwAtoms::String::New(""));

    {
        StudyV1 = ::fwAtoms::Object::dynamicCast(StudyV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[StudyV2] = StudyV1;

        ::fwStructuralPatch::fwMedData::Study::V2ToV1::sptr v2ToV1Patch;
        v2ToV1Patch = ::fwStructuralPatch::fwMedData::Study::V2ToV1::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV1Patch->apply(StudyV2, StudyV1, newVersions));

        CPPUNIT_ASSERT(StudyV1);
        CPPUNIT_ASSERT_EQUAL(std::string("1"), ::fwAtomsPatch::helper::getVersion(StudyV1));
        const auto attributes = StudyV1->getAttributes();
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
        CPPUNIT_ASSERT(attributes.find("study_id") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("consulting_physician_name") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_size") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_weight") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_body_mass_index") == attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace Study
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch

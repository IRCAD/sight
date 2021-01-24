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

#include "data/Study/V1ToV2Test.hpp"

#include <fwStructuralPatch/data/Study/V1ToV2.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Study::V1ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace Study
{

//------------------------------------------------------------------------------

void V1ToV2Test::setUp()
{
}

//------------------------------------------------------------------------------

void V1ToV2Test::tearDown()
{
}

//------------------------------------------------------------------------------

void V1ToV2Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr StudyV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr StudyV2;

    ::fwAtomsPatch::helper::setClassname(StudyV1, "data::Study");
    ::fwAtomsPatch::helper::setVersion(StudyV1, "1");

    ::fwAtomsPatch::helper::Object helper(StudyV1);

    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("referring_physician_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_age", ::fwAtoms::String::New(""));

    {
        StudyV2 = ::fwAtoms::Object::dynamicCast(StudyV1->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[StudyV1] = StudyV2;

        ::fwStructuralPatchdata::Study::V1ToV2::sptr v1ToV2Patch;
        v1ToV2Patch = ::fwStructuralPatchdata::Study::V1ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v1ToV2Patch->apply(StudyV1, StudyV2, newVersions));

        CPPUNIT_ASSERT(StudyV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), ::fwAtomsPatch::helper::getVersion(StudyV2));
        const auto attributes = StudyV2->getAttributes();
        CPPUNIT_ASSERT(attributes.find("number") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("laterality") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("protocolName") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("body_part_examined") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_position") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("anatomical_orientation_type") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performded_procedure_step_id") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_start_date") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_start_time") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_end_date") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_end_time") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_description") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_comments") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("study_id") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("consulting_physician_name") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_size") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_weight") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_body_mass_index") != attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace Study
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

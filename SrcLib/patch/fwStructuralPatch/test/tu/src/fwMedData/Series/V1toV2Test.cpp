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

#include "fwMedData/Series/V1ToV2Test.hpp"

#include <fwStructuralPatch/fwMedData/Series/V1ToV2.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwMedData::Series::V1ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace Series
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
    ::fwAtoms::Object::sptr seriesV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr seriesV2;

    ::fwAtomsPatch::helper::setClassname(seriesV1, "::fwMedData::Series");
    ::fwAtomsPatch::helper::setVersion(seriesV1, "1");

    ::fwAtomsPatch::helper::Object helper(seriesV1);

    helper.addAttribute("patient", ::fwAtoms::Object::sptr());
    helper.addAttribute("study", ::fwAtoms::Object::sptr());
    helper.addAttribute("equipment", ::fwAtoms::Object::sptr());
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));

    {
        seriesV2 = ::fwAtoms::Object::dynamicCast(seriesV1->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[seriesV1] = seriesV2;

        ::fwStructuralPatch::fwMedData::Series::V1ToV2::sptr v1ToV2Patch;
        v1ToV2Patch = ::fwStructuralPatch::fwMedData::Series::V1ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v1ToV2Patch->apply(seriesV1, seriesV2, newVersions));

        CPPUNIT_ASSERT(seriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), ::fwAtomsPatch::helper::getVersion(seriesV2));
        const auto attributes = seriesV2->getAttributes();
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
    }
}

//------------------------------------------------------------------------------
} //namespace Series
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch

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

#include "fwMedData/NavigationSeries/V2ToV1Test.hpp"

#include <fwStructuralPatch/fwMedData/NavigationSeries/V2ToV1.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwMedData::NavigationSeries::V2ToV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace NavigationSeries
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
    ::fwAtoms::Object::sptr NavigationSeriesV2 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr NavigationSeriesV1;

    ::fwAtomsPatch::helper::setClassname(NavigationSeriesV2, "::fwMedData::NavigationSeries");
    ::fwAtomsPatch::helper::setVersion(NavigationSeriesV2, "2");

    ::fwAtomsPatch::helper::Object helper(NavigationSeriesV2);

    helper.addAttribute("patient", ::fwAtoms::Object::sptr());
    helper.addAttribute("study", ::fwAtoms::Object::sptr());
    helper.addAttribute("equipment", ::fwAtoms::Object::sptr());
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    helper.addAttribute("position", ::fwAtoms::Object::sptr());
    helper.addAttribute("move", ::fwAtoms::Object::sptr());
    helper.addAttribute("look_at", ::fwAtoms::Object::sptr());

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
        NavigationSeriesV1 = ::fwAtoms::Object::dynamicCast(NavigationSeriesV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[NavigationSeriesV2] = NavigationSeriesV1;

        ::fwStructuralPatch::fwMedData::NavigationSeries::V2ToV1::sptr v2ToV1Patch;
        v2ToV1Patch = ::fwStructuralPatch::fwMedData::NavigationSeries::V2ToV1::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV1Patch->apply(NavigationSeriesV2, NavigationSeriesV1, newVersions));

        CPPUNIT_ASSERT(NavigationSeriesV1);
        CPPUNIT_ASSERT_EQUAL(std::string("1"), ::fwAtomsPatch::helper::getVersion(NavigationSeriesV1));
        const auto attributes = NavigationSeriesV1->getAttributes();
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
} //namespace NavigationSeries
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch

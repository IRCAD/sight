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

#include "data/NavigationSeries/V2ToV1Test.hpp"

#include <fwStructuralPatch/data/NavigationSeries/V2ToV1.hpp>

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::NavigationSeries::V2ToV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
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
    atoms::Object::sptr NavigationSeriesV2 = atoms::Object::New();
    atoms::Object::sptr NavigationSeriesV1;

    ::fwAtomsPatch::helper::setClassname(NavigationSeriesV2, "::sight::data::NavigationSeries");
    ::fwAtomsPatch::helper::setVersion(NavigationSeriesV2, "2");

    ::fwAtomsPatch::helper::Object helper(NavigationSeriesV2);

    helper.addAttribute("patient", atoms::Object::sptr());
    helper.addAttribute("study", atoms::Object::sptr());
    helper.addAttribute("equipment", atoms::Object::sptr());
    helper.addAttribute("modality", atoms::String::New(""));
    helper.addAttribute("instance_uid", atoms::String::New(""));
    helper.addAttribute("date", atoms::String::New(""));
    helper.addAttribute("time", atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", atoms::Sequence::New());
    helper.addAttribute("description", atoms::String::New(""));
    helper.addAttribute("position", atoms::Object::sptr());
    helper.addAttribute("move", atoms::Object::sptr());
    helper.addAttribute("look_at", atoms::Object::sptr());

    helper.addAttribute("number", atoms::String::New(""));
    helper.addAttribute("laterality", atoms::String::New(""));
    helper.addAttribute("protocolName", atoms::String::New(""));
    helper.addAttribute("body_part_examined", atoms::String::New(""));
    helper.addAttribute("patient_position", atoms::String::New(""));
    helper.addAttribute("anatomical_orientation_type", atoms::String::New(""));
    helper.addAttribute("performded_procedure_step_id", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_date", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_time", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_date", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_time", atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_description", atoms::String::New(""));
    helper.addAttribute("performed_procedure_comments", atoms::String::New(""));

    {
        NavigationSeriesV1 = atoms::Object::dynamicCast(NavigationSeriesV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[NavigationSeriesV2] = NavigationSeriesV1;

        ::fwStructuralPatchdata::NavigationSeries::V2ToV1::sptr v2ToV1Patch;
        v2ToV1Patch = ::fwStructuralPatchdata::NavigationSeries::V2ToV1::New();
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
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

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

#include "data/ActivitySeries/V1ToV2Test.hpp"

#include <fwStructuralPatch/data/ActivitySeries/V1ToV2.hpp>

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::ActivitySeries::V1ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace ActivitySeries
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
    atoms::Object::sptr activitySeriesV1 = atoms::Object::New();
    atoms::Object::sptr activitySeriesV2;

    ::fwAtomsPatch::helper::setClassname(activitySeriesV1, "::sight::data::ActivitySeries");
    ::fwAtomsPatch::helper::setVersion(activitySeriesV1, "1");

    ::fwAtomsPatch::helper::Object helper(activitySeriesV1);

    helper.addAttribute("patient", atoms::Object::sptr());
    helper.addAttribute("study", atoms::Object::sptr());
    helper.addAttribute("equipment", atoms::Object::sptr());
    helper.addAttribute("modality", atoms::String::New(""));
    helper.addAttribute("instance_uid", atoms::String::New(""));
    helper.addAttribute("date", atoms::String::New(""));
    helper.addAttribute("time", atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", atoms::Sequence::New());
    helper.addAttribute("description", atoms::String::New(""));
    helper.addAttribute("activity_config_id", atoms::String::New(""));
    helper.addAttribute("data", atoms::Object::New());

    {
        activitySeriesV2 = atoms::Object::dynamicCast(activitySeriesV1->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[activitySeriesV1] = activitySeriesV2;

        ::fwStructuralPatchdata::ActivitySeries::V1ToV2::sptr v1ToV2Patch;
        v1ToV2Patch = ::fwStructuralPatchdata::ActivitySeries::V1ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v1ToV2Patch->apply(activitySeriesV1, activitySeriesV2, newVersions));

        CPPUNIT_ASSERT(activitySeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), ::fwAtomsPatch::helper::getVersion(activitySeriesV2));
        const auto attributes = activitySeriesV2->getAttributes();
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
} //namespace ActivitySeries
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

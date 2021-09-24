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

#include "V1ToV2Test.hpp"

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/NavigationSeries/V1ToV2.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::patch::structural::ut::data::NavigationSeries::V1ToV2Test);

namespace sight::io::patch::structural
{

namespace ut
{

namespace data
{

namespace NavigationSeries
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
    sight::atoms::Object::sptr NavigationSeriesV1 = sight::atoms::Object::New();
    sight::atoms::Object::sptr NavigationSeriesV2;

    io::atoms::patch::helper::setClassname(NavigationSeriesV1, "sight::data::NavigationSeries");
    io::atoms::patch::helper::setVersion(NavigationSeriesV1, "1");

    io::atoms::patch::helper::Object helper(NavigationSeriesV1);

    helper.addAttribute("patient", sight::atoms::Object::sptr());
    helper.addAttribute("study", sight::atoms::Object::sptr());
    helper.addAttribute("equipment", sight::atoms::Object::sptr());
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("instance_uid", sight::atoms::String::New(""));
    helper.addAttribute("date", sight::atoms::String::New(""));
    helper.addAttribute("time", sight::atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New(""));
    helper.addAttribute("position", sight::atoms::Object::sptr());
    helper.addAttribute("move", sight::atoms::Object::sptr());
    helper.addAttribute("look_at", sight::atoms::Object::sptr());

    {
        NavigationSeriesV2 = sight::atoms::Object::dynamicCast(NavigationSeriesV1->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[NavigationSeriesV1] = NavigationSeriesV2;

        patch::structural::data::NavigationSeries::V1ToV2::sptr v1ToV2Patch;
        v1ToV2Patch = patch::structural::data::NavigationSeries::V1ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v1ToV2Patch->apply(NavigationSeriesV1, NavigationSeriesV2, newVersions));

        CPPUNIT_ASSERT(NavigationSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), io::atoms::patch::helper::getVersion(NavigationSeriesV2));
        const auto attributes = NavigationSeriesV2->getAttributes();
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

} //namespace NavigationSeries

} //namespace data

} //namespace ut

} //namespace sight::io::patch::structural

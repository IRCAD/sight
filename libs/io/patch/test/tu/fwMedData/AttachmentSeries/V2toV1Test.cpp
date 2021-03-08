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

#include "V2ToV1Test.hpp"

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/AttachmentSeries/V2ToV1.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::data::AttachmentSeries::V2ToV1Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
{
namespace AttachmentSeries
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
    sight::atoms::Object::sptr attachmentSeriesV2 = sight::atoms::Object::New();
    sight::atoms::Object::sptr attachmentSeriesV1;

    io::atoms::patch::helper::setClassname(attachmentSeriesV2, "::sight::data::AttachmentSeries");
    io::atoms::patch::helper::setVersion(attachmentSeriesV2, "2");

    io::atoms::patch::helper::Object helper(attachmentSeriesV2);

    helper.addAttribute("patient", sight::atoms::Object::sptr());
    helper.addAttribute("study", sight::atoms::Object::sptr());
    helper.addAttribute("equipment", sight::atoms::Object::sptr());
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("instance_uid", sight::atoms::String::New(""));
    helper.addAttribute("date", sight::atoms::String::New(""));
    helper.addAttribute("time", sight::atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New(""));

    helper.addAttribute("number", sight::atoms::String::New(""));
    helper.addAttribute("laterality", sight::atoms::String::New(""));
    helper.addAttribute("protocolName", sight::atoms::String::New(""));
    helper.addAttribute("body_part_examined", sight::atoms::String::New(""));
    helper.addAttribute("patient_position", sight::atoms::String::New(""));
    helper.addAttribute("anatomical_orientation_type", sight::atoms::String::New(""));
    helper.addAttribute("performded_procedure_step_id", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_date", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_start_time", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_date", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_end_time", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_step_description", sight::atoms::String::New(""));
    helper.addAttribute("performed_procedure_comments", sight::atoms::String::New(""));

    {
        attachmentSeriesV1 = sight::atoms::Object::dynamicCast(attachmentSeriesV2->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[attachmentSeriesV2] = attachmentSeriesV1;

        patch::structural::data::AttachmentSeries::V2ToV1::sptr v2ToV1Patch;
        v2ToV1Patch = patch::structural::data::AttachmentSeries::V2ToV1::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV1Patch->apply(attachmentSeriesV2, attachmentSeriesV1, newVersions));

        CPPUNIT_ASSERT(attachmentSeriesV1);
        CPPUNIT_ASSERT_EQUAL(std::string("1"), io::atoms::patch::helper::getVersion(attachmentSeriesV1));
        const auto attributes = attachmentSeriesV1->getAttributes();
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
} //namespace AttachmentSeries
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

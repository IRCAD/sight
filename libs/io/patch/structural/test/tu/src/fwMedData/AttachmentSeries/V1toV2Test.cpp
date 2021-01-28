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

#include "data/AttachmentSeries/V1ToV2Test.hpp"

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/AttachmentSeries/V1ToV2.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::utdata::AttachmentSeries::V1ToV2Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
{
namespace AttachmentSeries
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
    sight::atoms::Object::sptr attachmentSeriesV1 = sight::atoms::Object::New();
    sight::atoms::Object::sptr attachmentSeriesV2;

    io::atoms::patch::helper::setClassname(attachmentSeriesV1, "::sight::data::AttachmentSeries");
    io::atoms::patch::helper::setVersion(attachmentSeriesV1, "1");

    io::atoms::patch::helper::Object helper(attachmentSeriesV1);

    helper.addAttribute("patient", sight::atoms::Object::sptr());
    helper.addAttribute("study", sight::atoms::Object::sptr());
    helper.addAttribute("equipment", sight::atoms::Object::sptr());
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("instance_uid", sight::atoms::String::New(""));
    helper.addAttribute("date", sight::atoms::String::New(""));
    helper.addAttribute("time", sight::atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New(""));

    {
        attachmentSeriesV2 = sight::atoms::Object::dynamicCast(attachmentSeriesV1->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[attachmentSeriesV1] = attachmentSeriesV2;

        patch::structural::data::AttachmentSeries::V1ToV2::sptr v1ToV2Patch;
        v1ToV2Patch = patch::structural::data::AttachmentSeries::V1ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v1ToV2Patch->apply(attachmentSeriesV1, attachmentSeriesV2, newVersions));

        CPPUNIT_ASSERT(attachmentSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), io::atoms::patch::helper::getVersion(attachmentSeriesV2));
        const auto attributes = attachmentSeriesV2->getAttributes();
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
} //namespace AttachmentSeries
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

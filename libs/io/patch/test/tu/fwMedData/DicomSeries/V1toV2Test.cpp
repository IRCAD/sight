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
#include <io/patch/structural/data/DicomSeries/V1ToV2.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::patch::structural::ut::data::DicomSeries::V1ToV2Test);

namespace sight::io::patch::structural
{

namespace ut
{

namespace data
{

namespace DicomSeries
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
    sight::atoms::Object::sptr dicomSeriesV1 = sight::atoms::Object::New();
    sight::atoms::Object::sptr dicomSeriesV2;

    io::atoms::patch::helper::setClassname(dicomSeriesV1, "sight::data::DicomSeries");
    io::atoms::patch::helper::setVersion(dicomSeriesV1, "1");

    io::atoms::patch::helper::Object helper(dicomSeriesV1);

    helper.addAttribute("patient", sight::atoms::Object::sptr());
    helper.addAttribute("study", sight::atoms::Object::sptr());
    helper.addAttribute("equipment", sight::atoms::Object::sptr());
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("instance_uid", sight::atoms::String::New(""));
    helper.addAttribute("date", sight::atoms::String::New(""));
    helper.addAttribute("time", sight::atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New(""));

    helper.addAttribute("number_of_instances", sight::atoms::String::New(""));
    helper.addAttribute("dicom_container", sight::atoms::String::New(""));
    helper.addAttribute("sop_class_uids", sight::atoms::String::New(""));
    helper.addAttribute("computed_tag_values", sight::atoms::String::New(""));
    helper.addAttribute("first_instance_number", sight::atoms::String::New(""));

    {
        dicomSeriesV2 = sight::atoms::Object::dynamicCast(dicomSeriesV1->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[dicomSeriesV1] = dicomSeriesV2;

        patch::structural::data::DicomSeries::V1ToV2::sptr v1ToV2Patch;
        v1ToV2Patch = patch::structural::data::DicomSeries::V1ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v1ToV2Patch->apply(dicomSeriesV1, dicomSeriesV2, newVersions));

        CPPUNIT_ASSERT(dicomSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), io::atoms::patch::helper::getVersion(dicomSeriesV2));
        const auto attributes = dicomSeriesV2->getAttributes();
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

} //namespace DicomSeries

} //namespace data

} //namespace ut

} //namespace sight::io::patch::structural

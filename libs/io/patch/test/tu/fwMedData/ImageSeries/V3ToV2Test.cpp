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

#include "V3ToV2Test.hpp"

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/ImageSeries/V3ToV2.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::patch::structural::ut::data::ImageSeries::V3ToV2Test);

namespace sight::io::patch::structural
{

namespace ut
{

namespace data
{

namespace ImageSeries
{

//------------------------------------------------------------------------------

void V3ToV2Test::setUp()
{
}

//------------------------------------------------------------------------------

void V3ToV2Test::tearDown()
{
}

//------------------------------------------------------------------------------

void V3ToV2Test::applyPatchTest()
{
    sight::atoms::Object::sptr imgSeriesV3 = sight::atoms::Object::New();
    sight::atoms::Object::sptr imgSeriesV2;

    io::atoms::patch::helper::setClassname(imgSeriesV3, "::sight::data::ImageSeries");
    io::atoms::patch::helper::setVersion(imgSeriesV3, "3");

    io::atoms::patch::helper::Object helper(imgSeriesV3);

    helper.addAttribute("patient", sight::atoms::Object::sptr());
    helper.addAttribute("study", sight::atoms::Object::sptr());
    helper.addAttribute("equipment", sight::atoms::Object::sptr());
    helper.addAttribute("instance_uid", sight::atoms::String::New(""));
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("date", sight::atoms::String::New(""));
    helper.addAttribute("time", sight::atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New(""));
    helper.addAttribute("image", sight::atoms::Object::sptr());
    helper.addAttribute("dicom_reference", sight::atoms::Object::sptr());

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
    helper.addAttribute("contrast_bolus_agent", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_route", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_volume", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_start_time", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_stop_time", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_total_dose", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_rate", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_duration", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient", sight::atoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient_concentration", sight::atoms::String::New(""));
    helper.addAttribute("acquisition_date", sight::atoms::String::New(""));
    helper.addAttribute("acquisition_time", sight::atoms::String::New(""));

    {
        imgSeriesV2 = sight::atoms::Object::dynamicCast(imgSeriesV3->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[imgSeriesV3] = imgSeriesV2;

        patch::structural::data::ImageSeries::V3ToV2::sptr v3ToV2Patch;
        v3ToV2Patch = patch::structural::data::ImageSeries::V3ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v3ToV2Patch->apply(imgSeriesV3, imgSeriesV2, newVersions));

        CPPUNIT_ASSERT(imgSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), io::atoms::patch::helper::getVersion(imgSeriesV2));
        const auto attributes = imgSeriesV2->getAttributes();
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
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_agent") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_route") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_volume") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_start_time") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_stop_time") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_total_dose") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_flow_rate") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_flow_duration") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_ingredient") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_ingredient_concentration") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("acquisition_date") == attributes.end());
        CPPUNIT_ASSERT(attributes.find("acquisition_time") == attributes.end());
    }
}

//------------------------------------------------------------------------------

} //namespace ImageSeries

} //namespace data

} //namespace ut

} //namespace sight::io::patch::structural

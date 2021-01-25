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

#include "data/ImageSeries/V3ToV2Test.hpp"

#include <fwStructuralPatch/data/ImageSeries/V3ToV2.hpp>

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::ImageSeries::V3ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
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
    atoms::Object::sptr imgSeriesV3 = atoms::Object::New();
    atoms::Object::sptr imgSeriesV2;

    ::fwAtomsPatch::helper::setClassname(imgSeriesV3, "::sight::data::ImageSeries");
    ::fwAtomsPatch::helper::setVersion(imgSeriesV3, "3");

    ::fwAtomsPatch::helper::Object helper(imgSeriesV3);

    helper.addAttribute("patient", atoms::Object::sptr());
    helper.addAttribute("study", atoms::Object::sptr());
    helper.addAttribute("equipment", atoms::Object::sptr());
    helper.addAttribute("instance_uid", atoms::String::New(""));
    helper.addAttribute("modality", atoms::String::New(""));
    helper.addAttribute("date", atoms::String::New(""));
    helper.addAttribute("time", atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", atoms::Sequence::New());
    helper.addAttribute("description", atoms::String::New(""));
    helper.addAttribute("image", atoms::Object::sptr());
    helper.addAttribute("dicom_reference", atoms::Object::sptr());

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
    helper.addAttribute("contrast_bolus_agent", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_route", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_volume", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_start_time", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_stop_time", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_total_dose", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_rate", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_flow_duration", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient", atoms::String::New(""));
    helper.addAttribute("contrast_bolus_ingredient_concentration", atoms::String::New(""));
    helper.addAttribute("acquisition_date", atoms::String::New(""));
    helper.addAttribute("acquisition_time", atoms::String::New(""));

    {
        imgSeriesV2 = atoms::Object::dynamicCast(imgSeriesV3->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[imgSeriesV3] = imgSeriesV2;

        ::fwStructuralPatchdata::ImageSeries::V3ToV2::sptr v3ToV2Patch;
        v3ToV2Patch = ::fwStructuralPatchdata::ImageSeries::V3ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v3ToV2Patch->apply(imgSeriesV3, imgSeriesV2, newVersions));

        CPPUNIT_ASSERT(imgSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), ::fwAtomsPatch::helper::getVersion(imgSeriesV2));
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
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

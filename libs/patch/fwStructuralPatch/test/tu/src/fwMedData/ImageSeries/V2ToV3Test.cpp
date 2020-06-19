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

#include "fwMedData/ImageSeries/V2ToV3Test.hpp"

#include <fwStructuralPatch/fwMedData/ImageSeries/V2ToV3.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwMedData::ImageSeries::V2ToV3Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace ImageSeries
{

//------------------------------------------------------------------------------

void V2ToV3Test::setUp()
{
}

//------------------------------------------------------------------------------

void V2ToV3Test::tearDown()
{
}

//------------------------------------------------------------------------------

void V2ToV3Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr imgSeriesV2 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr imgSeriesV3;

    ::fwAtomsPatch::helper::setClassname(imgSeriesV2, "::fwMedData::ImageSeries");
    ::fwAtomsPatch::helper::setVersion(imgSeriesV2, "2");

    ::fwAtomsPatch::helper::Object helper(imgSeriesV2);

    helper.addAttribute("patient", ::fwAtoms::Object::sptr());
    helper.addAttribute("study", ::fwAtoms::Object::sptr());
    helper.addAttribute("equipment", ::fwAtoms::Object::sptr());
    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    helper.addAttribute("image", ::fwAtoms::Object::sptr());
    helper.addAttribute("dicom_reference", ::fwAtoms::Object::sptr());

    {
        imgSeriesV3 = ::fwAtoms::Object::dynamicCast(imgSeriesV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[imgSeriesV2] = imgSeriesV3;

        ::fwStructuralPatch::fwMedData::ImageSeries::V2ToV3::sptr v2ToV3Patch;
        v2ToV3Patch = ::fwStructuralPatch::fwMedData::ImageSeries::V2ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV3Patch->apply(imgSeriesV2, imgSeriesV3, newVersions));

        CPPUNIT_ASSERT(imgSeriesV3);
        CPPUNIT_ASSERT_EQUAL(std::string("3"), ::fwAtomsPatch::helper::getVersion(imgSeriesV3));
        const auto attributes = imgSeriesV3->getAttributes();
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
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_agent") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_route") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_volume") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_start_time") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_stop_time") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_total_dose") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_flow_rate") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_flow_duration") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_ingredient") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("contrast_bolus_ingredient_concentration") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("acquisition_date") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("acquisition_time") != attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace ImageSeries
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch

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

#include "arData/CameraSeries/V3ToV2Test.hpp"

#include <arStructuralPatch/arData/CameraSeries/V3ToV2.hpp>

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::arStructuralPatch::ut::arData::CameraSeries::V3ToV2Test );

namespace arStructuralPatch
{
namespace ut
{
namespace arData
{
namespace CameraSeries
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
    atoms::Object::sptr CameraSeriesV3 = atoms::Object::New();
    atoms::Object::sptr CameraSeriesV2;

    ::fwAtomsPatch::helper::setClassname(CameraSeriesV3, "::arData::CameraSeries");
    ::fwAtomsPatch::helper::setVersion(CameraSeriesV3, "3");

    ::fwAtomsPatch::helper::Object helper(CameraSeriesV3);

    helper.addAttribute("patient", atoms::Object::sptr());
    helper.addAttribute("study", atoms::Object::sptr());
    helper.addAttribute("equipment", atoms::Object::sptr());
    helper.addAttribute("instance_uid", atoms::String::New(""));
    helper.addAttribute("modality", atoms::String::New(""));
    helper.addAttribute("date", atoms::String::New(""));
    helper.addAttribute("time", atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", atoms::Sequence::New());
    helper.addAttribute("description", atoms::String::New(""));
    helper.addAttribute("dicom_reference", atoms::Object::sptr());
    helper.addAttribute("reconstruction_db", atoms::Object::sptr());
    helper.addAttribute("cameras", atoms::Object::sptr());
    helper.addAttribute("extrinsic_matrices", atoms::Object::sptr());

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
        CameraSeriesV2 = atoms::Object::dynamicCast(CameraSeriesV3->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[CameraSeriesV3] = CameraSeriesV2;

        ::arStructuralPatch::arData::CameraSeries::V3ToV2::sptr v3ToV2Patch;
        v3ToV2Patch = ::arStructuralPatch::arData::CameraSeries::V3ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v3ToV2Patch->apply(CameraSeriesV3, CameraSeriesV2, newVersions));

        CPPUNIT_ASSERT(CameraSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), ::fwAtomsPatch::helper::getVersion(CameraSeriesV2));
        const auto attributes = CameraSeriesV2->getAttributes();
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
} //namespace CameraSeries
} //namespace arData
} //namespace ut
} //namespace arStructuralPatch

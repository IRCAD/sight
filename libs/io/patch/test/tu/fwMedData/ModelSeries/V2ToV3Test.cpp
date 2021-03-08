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

#include "V2ToV3Test.hpp"

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/ModelSeries/V2ToV3.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::data::ModelSeries::V2ToV3Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
{
namespace ModelSeries
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
    sight::atoms::Object::sptr modelSeriesV2 = sight::atoms::Object::New();
    sight::atoms::Object::sptr modelSeriesV3;

    io::atoms::patch::helper::setClassname(modelSeriesV2, "::sight::data::ModelSeries");
    io::atoms::patch::helper::setVersion(modelSeriesV2, "2");

    io::atoms::patch::helper::Object helper(modelSeriesV2);

    helper.addAttribute("patient", sight::atoms::Object::sptr());
    helper.addAttribute("study", sight::atoms::Object::sptr());
    helper.addAttribute("equipment", sight::atoms::Object::sptr());
    helper.addAttribute("instance_uid", sight::atoms::String::New(""));
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("date", sight::atoms::String::New(""));
    helper.addAttribute("time", sight::atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New(""));
    helper.addAttribute("dicom_reference", sight::atoms::Object::sptr());
    helper.addAttribute("reconstruction_db", sight::atoms::Object::sptr());

    {
        modelSeriesV3 = sight::atoms::Object::dynamicCast(modelSeriesV2->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[modelSeriesV2] = modelSeriesV3;

        patch::structural::data::ModelSeries::V2ToV3::sptr v2ToV3Patch;
        v2ToV3Patch = patch::structural::data::ModelSeries::V2ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV3Patch->apply(modelSeriesV2, modelSeriesV3, newVersions));

        CPPUNIT_ASSERT(modelSeriesV3);
        CPPUNIT_ASSERT_EQUAL(std::string("3"), io::atoms::patch::helper::getVersion(modelSeriesV3));
        const auto attributes = modelSeriesV3->getAttributes();
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
} //namespace ModelSeries
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

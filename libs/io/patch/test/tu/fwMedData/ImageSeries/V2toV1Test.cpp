/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include <atoms/Base.hpp>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/ImageSeries/V2ToV1.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::data::ImageSeries::V2ToV1Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
{
namespace ImageSeries
{

//------------------------------------------------------------------------------

void V2ToV1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V2ToV1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V2ToV1Test::applyPatchTest()
{
    sight::atoms::Object::sptr imgSeriesV2 = sight::atoms::Object::New();
    sight::atoms::Object::sptr imgSeriesV1;

    io::atoms::patch::helper::setClassname(imgSeriesV2, "::sight::data::ImageSeries");
    io::atoms::patch::helper::setVersion(imgSeriesV2, "2");

    io::atoms::patch::helper::Object helper(imgSeriesV2);

    helper.addAttribute("image", sight::atoms::Object::sptr());

    helper.addAttribute("patient", sight::atoms::Object::sptr());
    helper.addAttribute("study", sight::atoms::Object::sptr());
    helper.addAttribute("equipment", sight::atoms::Object::sptr());
    helper.addAttribute("instance_uid", sight::atoms::String::New("UID"));
    helper.addAttribute("modality", sight::atoms::String::New("OT"));
    helper.addAttribute("date", sight::atoms::String::New("0"));
    helper.addAttribute("time", sight::atoms::String::New("0"));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New("Description"));

    // Check if attribute "dicom_reference" is removed in ImageSeries V1
    {
        imgSeriesV1 = sight::atoms::Object::dynamicCast(imgSeriesV2->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[imgSeriesV2] = imgSeriesV1;

        patch::structural::data::ImageSeries::V2ToV1::sptr v2ToV1Patch;
        v2ToV1Patch = patch::structural::data::ImageSeries::V2ToV1::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV1Patch->apply(imgSeriesV2, imgSeriesV1, newVersions));

        CPPUNIT_ASSERT(imgSeriesV1);
        CPPUNIT_ASSERT_EQUAL(std::string("1"), io::atoms::patch::helper::getVersion(imgSeriesV1));
        const auto attributes = imgSeriesV1->getAttributes();
        CPPUNIT_ASSERT(attributes.find("dicom_reference") == attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace ImageSeries
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

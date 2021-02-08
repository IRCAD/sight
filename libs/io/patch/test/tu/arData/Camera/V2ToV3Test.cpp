/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <atoms/Base.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Camera/V2ToV3.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::data::Camera::V2ToV3Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
{
namespace Camera
{

//------------------------------------------------------------------------------

void V2ToV3Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V2ToV3Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V2ToV3Test::applyPatchTest()
{
    sight::atoms::Object::sptr camObjV2 = sight::atoms::Object::New();
    sight::atoms::Object::sptr camObjV3;

    io::atoms::patch::helper::setClassname(camObjV2, "data::Camera");
    io::atoms::patch::helper::setVersion(camObjV2, "2");

    io::atoms::patch::helper::Object helper(camObjV2);

    helper.addAttribute("camera_id", sight::atoms::String::New("file"));
    helper.addAttribute("description", sight::atoms::String::New("Videos/myVideo.mp4"));
    sight::atoms::Sequence::sptr intrinsicParam = sight::atoms::Sequence::New();
    intrinsicParam->push_back(sight::atoms::Numeric::New(2596.78));
    intrinsicParam->push_back(sight::atoms::Numeric::New(2554.72));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1020.74));
    intrinsicParam->push_back(sight::atoms::Numeric::New(500.189));
    helper.addAttribute("intrinsic", intrinsicParam);
    helper.addAttribute("width", sight::atoms::Numeric::New(1920));
    helper.addAttribute("height", sight::atoms::Numeric::New(1080));
    helper.addAttribute("camera_source", sight::atoms::String::New("FILE"));
    helper.addAttribute("video_file", sight::atoms::String::New("Videos/myVideo.mp4"));
    helper.addAttribute("stream_url", sight::atoms::String::New(""));
    helper.addAttribute("max_framerate", sight::atoms::String::New("30"));
    helper.addAttribute("pixel_format", sight::atoms::String::New("RGBA32"));

    camObjV3 = sight::atoms::Object::dynamicCast(camObjV2->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV2] = camObjV3;

    auto patch = patch::structural::data::Camera::V2ToV3::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(camObjV2, camObjV3, newVersions));

    CPPUNIT_ASSERT(camObjV3);
    CPPUNIT_ASSERT(camObjV3->getAttribute("width"));
    sight::atoms::Sequence::sptr intrinsicParam2 =
        sight::atoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
    CPPUNIT_ASSERT_EQUAL(size_t(4), intrinsicParam2->size());
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), camObjV3->getAttribute("width")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("height"));
    CPPUNIT_ASSERT_EQUAL(std::string("1080"), camObjV3->getAttribute("height")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("max_framerate"));
    CPPUNIT_ASSERT_EQUAL(std::string("30"), camObjV3->getAttribute("max_framerate")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("video_file"));
    CPPUNIT_ASSERT_EQUAL(std::string("Videos/myVideo.mp4"), camObjV3->getAttribute("video_file")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("stream_url"));
    CPPUNIT_ASSERT(camObjV3->getAttribute("stream_url")->getString().empty());
    CPPUNIT_ASSERT(camObjV3->getAttribute("camera_source"));
    CPPUNIT_ASSERT_EQUAL(std::string("FILE"), camObjV3->getAttribute("camera_source")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("pixel_format"));
    CPPUNIT_ASSERT_EQUAL(std::string("RGBA32"), camObjV3->getAttribute("pixel_format")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("scale"));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), camObjV3->getAttribute("scale")->getString());
}

//------------------------------------------------------------------------------

} //namespace Camera
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

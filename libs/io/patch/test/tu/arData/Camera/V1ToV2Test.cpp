/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <atoms/Base.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Camera/V1ToV2.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::patch::structural::ut::data::Camera::V1ToV2Test);

namespace sight::io::patch::structural
{

namespace ut
{

namespace data
{

namespace Camera
{

// force link for log
utest::Exception e("");

//------------------------------------------------------------------------------

void V1ToV2Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V1ToV2Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V1ToV2Test::applyPatchTestFile()
{
    using sight::atoms::Object;

    sight::atoms::Object::sptr camObjV1 = sight::atoms::Object::New();
    sight::atoms::Object::sptr camObjV2;

    io::atoms::patch::helper::setClassname(camObjV1, "data::Camera");
    io::atoms::patch::helper::setVersion(camObjV1, "1");

    io::atoms::patch::helper::Object helper(camObjV1);

    helper.addAttribute("camera_id", sight::atoms::String::New("file"));
    helper.addAttribute("description", sight::atoms::String::New("Videos/myVideo.mp4"));
    sight::atoms::Sequence::sptr intrinsicParam = sight::atoms::Sequence::New();
    intrinsicParam->push_back(sight::atoms::Numeric::New(2596.78));
    intrinsicParam->push_back(sight::atoms::Numeric::New(2554.72));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1020.74));
    intrinsicParam->push_back(sight::atoms::Numeric::New(500.189));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1920));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1080));
    helper.addAttribute("intrinsic", intrinsicParam);

    camObjV2 = sight::atoms::Object::dynamicCast(camObjV1->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    patch::structural::data::Camera::V1ToV2::sptr cam1ToCam2Patch;
    cam1ToCam2Patch = patch::structural::data::Camera::V1ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(cam1ToCam2Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("width"));
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), camObjV2->getAttribute("width")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("height"));
    CPPUNIT_ASSERT_EQUAL(std::string("1080"), camObjV2->getAttribute("height")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("max_framerate"));
    CPPUNIT_ASSERT_EQUAL(std::string("30"), camObjV2->getAttribute("max_framerate")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("video_file"));
    CPPUNIT_ASSERT_EQUAL(std::string("Videos/myVideo.mp4"), camObjV2->getAttribute("video_file")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("stream_url"));
    CPPUNIT_ASSERT(camObjV2->getAttribute("stream_url")->getString().empty());
    CPPUNIT_ASSERT(camObjV2->getAttribute("camera_source"));
    CPPUNIT_ASSERT_EQUAL(std::string("FILE"), camObjV2->getAttribute("camera_source")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("pixel_format"));
    CPPUNIT_ASSERT_EQUAL(std::string("RGBA32"), camObjV2->getAttribute("pixel_format")->getString());
}

//------------------------------------------------------------------------------

void V1ToV2Test::applyPatchTestStream()
{
    sight::atoms::Object::sptr camObjV1 = sight::atoms::Object::New();
    sight::atoms::Object::sptr camObjV2;

    io::atoms::patch::helper::setClassname(camObjV1, "data::Camera");
    io::atoms::patch::helper::setVersion(camObjV1, "1");

    io::atoms::patch::helper::Object helper(camObjV1);

    helper.addAttribute("camera_id", sight::atoms::String::New("stream"));
    helper.addAttribute("description", sight::atoms::String::New("rtsp://@127.0.0.1"));
    sight::atoms::Sequence::sptr intrinsicParam = sight::atoms::Sequence::New();
    intrinsicParam->push_back(sight::atoms::Numeric::New(2596.78));
    intrinsicParam->push_back(sight::atoms::Numeric::New(2554.72));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1020.74));
    intrinsicParam->push_back(sight::atoms::Numeric::New(500.189));
    intrinsicParam->push_back(sight::atoms::Numeric::New(800));
    intrinsicParam->push_back(sight::atoms::Numeric::New(600));
    helper.addAttribute("intrinsic", intrinsicParam);

    camObjV2 = sight::atoms::Object::dynamicCast(camObjV1->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    patch::structural::data::Camera::V1ToV2::sptr cam1ToCam2Patch;
    cam1ToCam2Patch = patch::structural::data::Camera::V1ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(cam1ToCam2Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("width"));
    CPPUNIT_ASSERT_EQUAL(std::string("800"), camObjV2->getAttribute("width")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("height"));
    CPPUNIT_ASSERT_EQUAL(std::string("600"), camObjV2->getAttribute("height")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("max_framerate"));
    CPPUNIT_ASSERT_EQUAL(std::string("30"), camObjV2->getAttribute("max_framerate")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("video_file"));
    CPPUNIT_ASSERT(camObjV2->getAttribute("video_file")->getString().empty());
    CPPUNIT_ASSERT(camObjV2->getAttribute("stream_url"));
    CPPUNIT_ASSERT_EQUAL(std::string("rtsp://@127.0.0.1"), camObjV2->getAttribute("stream_url")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("camera_source"));
    CPPUNIT_ASSERT_EQUAL(std::string("STREAM"), camObjV2->getAttribute("camera_source")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("pixel_format"));
    CPPUNIT_ASSERT_EQUAL(std::string("RGBA32"), camObjV2->getAttribute("pixel_format")->getString());
}

//------------------------------------------------------------------------------

void V1ToV2Test::applyPatchTestDevice()
{
    sight::atoms::Object::sptr camObjV1 = sight::atoms::Object::New();
    sight::atoms::Object::sptr camObjV2;

    io::atoms::patch::helper::setClassname(camObjV1, "data::Camera");
    io::atoms::patch::helper::setVersion(camObjV1, "1");

    io::atoms::patch::helper::Object helper(camObjV1);

    helper.addAttribute("camera_id", sight::atoms::String::New("jreoigjflxgjpeohf4g1hf12df4gdf2gdkfos"));
    helper.addAttribute("description", sight::atoms::String::New("My camera"));
    sight::atoms::Sequence::sptr intrinsicParam = sight::atoms::Sequence::New();
    intrinsicParam->push_back(sight::atoms::Numeric::New(2596.78));
    intrinsicParam->push_back(sight::atoms::Numeric::New(2554.72));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1020.74));
    intrinsicParam->push_back(sight::atoms::Numeric::New(500.189));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1920));
    intrinsicParam->push_back(sight::atoms::Numeric::New(1080));
    helper.addAttribute("intrinsic", intrinsicParam);

    camObjV2 = sight::atoms::Object::dynamicCast(camObjV1->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    patch::structural::data::Camera::V1ToV2::sptr cam1ToCam2Patch;
    cam1ToCam2Patch = patch::structural::data::Camera::V1ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(cam1ToCam2Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("width"));
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), camObjV2->getAttribute("width")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("height"));
    CPPUNIT_ASSERT_EQUAL(std::string("1080"), camObjV2->getAttribute("height")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("max_framerate"));
    CPPUNIT_ASSERT_EQUAL(std::string("30"), camObjV2->getAttribute("max_framerate")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("video_file"));
    CPPUNIT_ASSERT(camObjV2->getAttribute("video_file")->getString().empty());
    CPPUNIT_ASSERT(camObjV2->getAttribute("stream_url"));
    CPPUNIT_ASSERT(camObjV2->getAttribute("stream_url")->getString().empty());
    CPPUNIT_ASSERT(camObjV2->getAttribute("camera_source"));
    CPPUNIT_ASSERT_EQUAL(std::string("DEVICE"), camObjV2->getAttribute("camera_source")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("pixel_format"));
    CPPUNIT_ASSERT_EQUAL(std::string("RGBA32"), camObjV2->getAttribute("pixel_format")->getString());
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    sight::atoms::Sequence::sptr intrinsic = sight::atoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
    CPPUNIT_ASSERT_EQUAL(size_t(4), intrinsic->size());
}

//------------------------------------------------------------------------------

} //namespace Camera

} //namespace data

} //namespace ut

} //namespace sight::io::patch::structural

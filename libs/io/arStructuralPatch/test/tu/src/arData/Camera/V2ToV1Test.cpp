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

#include "data/Camera/V2ToV1Test.hpp"

#include <arStructuralPatch/data/Camera/V2ToV1.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arStructuralPatch::utdata::Camera::V2ToV1Test );

namespace arStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace Camera
{
// force link for log

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

void V2ToV1Test::applyPatchTestFile()
{
    atoms::Object::sptr camObjV1 = atoms::Object::New();
    atoms::Object::sptr camObjV2;

    io::atoms::patch::helper::setClassname(camObjV1, "data::Camera");
    io::atoms::patch::helper::setVersion(camObjV1, "2");

    io::atoms::patch::helper::Object helper(camObjV1);

    helper.addAttribute("is_calibrated", atoms::Boolean::New(true));
    helper.addAttribute("skew", atoms::Numeric::New(0));
    helper.addAttribute("camera_id", atoms::String::New(""));
    helper.addAttribute("description", atoms::String::New(""));
    atoms::Sequence::sptr intrinsicParam = atoms::Sequence::New();
    intrinsicParam->push_back(atoms::Numeric::New(2596.78));
    intrinsicParam->push_back(atoms::Numeric::New(2554.72));
    intrinsicParam->push_back(atoms::Numeric::New(1020.74));
    intrinsicParam->push_back(atoms::Numeric::New(500.189));
    helper.addAttribute("intrinsic", intrinsicParam);
    helper.addAttribute("width", atoms::Numeric::New(1920));
    helper.addAttribute("height", atoms::Numeric::New(1080));
    helper.addAttribute("camera_source", atoms::String::New("FILE"));
    helper.addAttribute("video_file", atoms::String::New("Videos/myVideo.mp4"));
    helper.addAttribute("stream_url", atoms::String::New(""));
    helper.addAttribute("max_framerate", atoms::String::New("33"));
    helper.addAttribute("pixel_format", atoms::String::New("RGBA32"));

    camObjV2 = atoms::Object::dynamicCast(camObjV1->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatchdata::Camera::V2ToV1::sptr cam2ToCam1Patch;
    cam2ToCam1Patch = ::arStructuralPatchdata::Camera::V2ToV1::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    atoms::Sequence::sptr intrinsicParam2 = atoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
    CPPUNIT_ASSERT_EQUAL(size_t(6), intrinsicParam2->size());
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), (*intrinsicParam2)[4]->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("1080"), (*intrinsicParam2)[5]->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), camObjV2->getAttribute("width")->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("file"), camObjV2->getAttribute("camera_id")->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("Videos/myVideo.mp4"), camObjV2->getAttribute("description")->getString());
    CPPUNIT_ASSERT(!camObjV2->getAttribute("max_framerate"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("video_file"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("stream_url"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("camera_source"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("pixel_format"));
}

//------------------------------------------------------------------------------

void V2ToV1Test::applyPatchTestStream()
{
    atoms::Object::sptr camObjV1 = atoms::Object::New();
    atoms::Object::sptr camObjV2;

    io::atoms::patch::helper::setClassname(camObjV1, "data::Camera");
    io::atoms::patch::helper::setVersion(camObjV1, "2");

    io::atoms::patch::helper::Object helper(camObjV1);

    helper.addAttribute("is_calibrated", atoms::Boolean::New(true));
    helper.addAttribute("skew", atoms::Numeric::New(0));
    helper.addAttribute("camera_id", atoms::String::New(""));
    helper.addAttribute("description", atoms::String::New(""));
    atoms::Sequence::sptr intrinsicParam = atoms::Sequence::New();
    intrinsicParam->push_back(atoms::Numeric::New(2596.78));
    intrinsicParam->push_back(atoms::Numeric::New(2554.72));
    intrinsicParam->push_back(atoms::Numeric::New(1020.74));
    intrinsicParam->push_back(atoms::Numeric::New(500.189));
    helper.addAttribute("intrinsic", intrinsicParam);
    helper.addAttribute("width", atoms::Numeric::New(1920));
    helper.addAttribute("height", atoms::Numeric::New(1080));
    helper.addAttribute("camera_source", atoms::String::New("STREAM"));
    helper.addAttribute("video_file", atoms::String::New(""));
    helper.addAttribute("stream_url", atoms::String::New("rtsp://@127.0.0.1"));
    helper.addAttribute("max_framerate", atoms::String::New("33"));
    helper.addAttribute("pixel_format", atoms::String::New("RGBA32"));

    camObjV2 = atoms::Object::dynamicCast(camObjV1->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatchdata::Camera::V2ToV1::sptr cam2ToCam1Patch;
    cam2ToCam1Patch = ::arStructuralPatchdata::Camera::V2ToV1::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    atoms::Sequence::sptr intrinsicParam2 = atoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
    CPPUNIT_ASSERT_EQUAL(size_t(6), intrinsicParam2->size());
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), (*intrinsicParam2)[4]->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("1080"), (*intrinsicParam2)[5]->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), camObjV2->getAttribute("width")->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("stream"), camObjV2->getAttribute("camera_id")->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("rtsp://@127.0.0.1"), camObjV2->getAttribute("description")->getString());
    CPPUNIT_ASSERT(!camObjV2->getAttribute("max_framerate"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("video_file"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("stream_url"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("camera_source"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("pixel_format"));
}

//------------------------------------------------------------------------------

void V2ToV1Test::applyPatchTestDevice()
{
    atoms::Object::sptr camObjV1 = atoms::Object::New();
    atoms::Object::sptr camObjV2;

    io::atoms::patch::helper::setClassname(camObjV1, "data::Camera");
    io::atoms::patch::helper::setVersion(camObjV1, "2");

    io::atoms::patch::helper::Object helper(camObjV1);

    helper.addAttribute("is_calibrated", atoms::Boolean::New(true));
    helper.addAttribute("skew", atoms::Numeric::New(0));
    helper.addAttribute("camera_id", atoms::String::New("myCameraID"));
    helper.addAttribute("description", atoms::String::New("my camera description"));
    atoms::Sequence::sptr intrinsicParam = atoms::Sequence::New();
    intrinsicParam->push_back(atoms::Numeric::New(2596.78));
    intrinsicParam->push_back(atoms::Numeric::New(2554.72));
    intrinsicParam->push_back(atoms::Numeric::New(1020.74));
    intrinsicParam->push_back(atoms::Numeric::New(500.189));
    helper.addAttribute("intrinsic", intrinsicParam);
    helper.addAttribute("width", atoms::Numeric::New(1920));
    helper.addAttribute("height", atoms::Numeric::New(1080));
    helper.addAttribute("camera_source", atoms::String::New("DEVICE"));
    helper.addAttribute("video_file", atoms::String::New(""));
    helper.addAttribute("stream_url", atoms::String::New(""));
    helper.addAttribute("max_framerate", atoms::String::New("33"));
    helper.addAttribute("pixel_format", atoms::String::New("RGBA32"));

    camObjV2 = atoms::Object::dynamicCast(camObjV1->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatchdata::Camera::V2ToV1::sptr cam2ToCam1Patch;
    cam2ToCam1Patch = ::arStructuralPatchdata::Camera::V2ToV1::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    atoms::Sequence::sptr intrinsicParam2 = atoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
    CPPUNIT_ASSERT_EQUAL(size_t(6), intrinsicParam2->size());
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), (*intrinsicParam2)[4]->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("1080"), (*intrinsicParam2)[5]->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), camObjV2->getAttribute("width")->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("myCameraID"), camObjV2->getAttribute("camera_id")->getString());
    CPPUNIT_ASSERT_EQUAL(std::string("my camera description"), camObjV2->getAttribute("description")->getString());
    CPPUNIT_ASSERT(!camObjV2->getAttribute("max_framerate"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("video_file"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("stream_url"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("camera_source"));
    CPPUNIT_ASSERT(!camObjV2->getAttribute("pixel_format"));
}

//------------------------------------------------------------------------------
} //namespace Camera
} //namespace sight::data
} //namespace ut
} //namespace arStructuralPatch

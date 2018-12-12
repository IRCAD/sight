/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include "arData/Camera/V1ToV2Test.hpp"

#include <arStructuralPatch/arData/Camera/V1ToV2.hpp>

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arStructuralPatch::ut::arData::Camera::V1ToV2Test );

namespace arStructuralPatch
{
namespace ut
{
namespace arData
{
namespace Camera
{
// force link for log
::fwTest::Exception e("");

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
    ::fwAtoms::Object::sptr camObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV1, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV1, "1");

    ::fwAtomsPatch::helper::Object helper(camObjV1);

    helper.addAttribute("camera_id", ::fwAtoms::String::New("file"));
    helper.addAttribute("description", ::fwAtoms::String::New("Videos/myVideo.mp4"));
    ::fwAtoms::Sequence::sptr intrinsicParam = ::fwAtoms::Sequence::New();
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2596.78));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2554.72));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1020.74));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(500.189));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1920));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1080));
    helper.addAttribute("intrinsic", intrinsicParam);

    camObjV2 = ::fwAtoms::Object::dynamicCast(camObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatch::arData::Camera::V1ToV2::sptr cam1ToCam2Patch;
    cam1ToCam2Patch = ::arStructuralPatch::arData::Camera::V1ToV2::New();
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
    ::fwAtoms::Object::sptr camObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV1, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV1, "1");

    ::fwAtomsPatch::helper::Object helper(camObjV1);

    helper.addAttribute("camera_id", ::fwAtoms::String::New("stream"));
    helper.addAttribute("description", ::fwAtoms::String::New("rtsp://@127.0.0.1"));
    ::fwAtoms::Sequence::sptr intrinsicParam = ::fwAtoms::Sequence::New();
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2596.78));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2554.72));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1020.74));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(500.189));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(800));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(600));
    helper.addAttribute("intrinsic", intrinsicParam);

    camObjV2 = ::fwAtoms::Object::dynamicCast(camObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatch::arData::Camera::V1ToV2::sptr cam1ToCam2Patch;
    cam1ToCam2Patch = ::arStructuralPatch::arData::Camera::V1ToV2::New();
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
    ::fwAtoms::Object::sptr camObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV1, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV1, "1");

    ::fwAtomsPatch::helper::Object helper(camObjV1);

    helper.addAttribute("camera_id", ::fwAtoms::String::New("jreoigjflxgjpeohf4g1hf12df4gdf2gdkfos"));
    helper.addAttribute("description", ::fwAtoms::String::New("My camera"));
    ::fwAtoms::Sequence::sptr intrinsicParam = ::fwAtoms::Sequence::New();
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2596.78));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2554.72));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1020.74));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(500.189));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1920));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1080));
    helper.addAttribute("intrinsic", intrinsicParam);

    camObjV2 = ::fwAtoms::Object::dynamicCast(camObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatch::arData::Camera::V1ToV2::sptr cam1ToCam2Patch;
    cam1ToCam2Patch = ::arStructuralPatch::arData::Camera::V1ToV2::New();
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
    ::fwAtoms::Sequence::sptr intrinsic = ::fwAtoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
    CPPUNIT_ASSERT_EQUAL(size_t(4), intrinsic->size());
}

//------------------------------------------------------------------------------
} //namespace Camera
} //namespace arData
} //namespace ut
} //namespace arStructuralPatch

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/Camera/V2ToV1Test.hpp"

#include <arStructuralPatch/arData/Camera/V2ToV1.hpp>

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arStructuralPatch::ut::arData::Camera::V2ToV1Test );

namespace arStructuralPatch
{
namespace ut
{
namespace arData
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
    ::fwAtoms::Object::sptr camObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV1, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV1, "2");

    ::fwAtomsPatch::helper::Object helper(camObjV1);

    helper.addAttribute("is_calibrated", ::fwAtoms::Boolean::New(true));
    helper.addAttribute("skew", ::fwAtoms::Numeric::New(0));
    helper.addAttribute("camera_id", ::fwAtoms::String::New(""));
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    ::fwAtoms::Sequence::sptr intrinsicParam = ::fwAtoms::Sequence::New();
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2596.78));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2554.72));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1020.74));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(500.189));
    helper.addAttribute("intrinsic", intrinsicParam);
    helper.addAttribute("width", ::fwAtoms::Numeric::New(1920));
    helper.addAttribute("height", ::fwAtoms::Numeric::New(1080));
    helper.addAttribute("camera_source", ::fwAtoms::String::New("FILE"));
    helper.addAttribute("video_file", ::fwAtoms::String::New("Videos/myVideo.mp4"));
    helper.addAttribute("stream_url", ::fwAtoms::String::New(""));
    helper.addAttribute("max_framerate", ::fwAtoms::String::New("33"));
    helper.addAttribute("pixel_format", ::fwAtoms::String::New("RGBA32"));

    camObjV2 = ::fwAtoms::Object::dynamicCast(camObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatch::arData::Camera::V2ToV1::sptr cam2ToCam1Patch;
    cam2ToCam1Patch = ::arStructuralPatch::arData::Camera::V2ToV1::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    ::fwAtoms::Sequence::sptr intrinsicParam2 = ::fwAtoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
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
    ::fwAtoms::Object::sptr camObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV1, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV1, "2");

    ::fwAtomsPatch::helper::Object helper(camObjV1);

    helper.addAttribute("is_calibrated", ::fwAtoms::Boolean::New(true));
    helper.addAttribute("skew", ::fwAtoms::Numeric::New(0));
    helper.addAttribute("camera_id", ::fwAtoms::String::New(""));
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    ::fwAtoms::Sequence::sptr intrinsicParam = ::fwAtoms::Sequence::New();
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2596.78));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2554.72));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1020.74));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(500.189));
    helper.addAttribute("intrinsic", intrinsicParam);
    helper.addAttribute("width", ::fwAtoms::Numeric::New(1920));
    helper.addAttribute("height", ::fwAtoms::Numeric::New(1080));
    helper.addAttribute("camera_source", ::fwAtoms::String::New("STREAM"));
    helper.addAttribute("video_file", ::fwAtoms::String::New(""));
    helper.addAttribute("stream_url", ::fwAtoms::String::New("rtsp://@127.0.0.1"));
    helper.addAttribute("max_framerate", ::fwAtoms::String::New("33"));
    helper.addAttribute("pixel_format", ::fwAtoms::String::New("RGBA32"));

    camObjV2 = ::fwAtoms::Object::dynamicCast(camObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatch::arData::Camera::V2ToV1::sptr cam2ToCam1Patch;
    cam2ToCam1Patch = ::arStructuralPatch::arData::Camera::V2ToV1::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    ::fwAtoms::Sequence::sptr intrinsicParam2 = ::fwAtoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
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
    ::fwAtoms::Object::sptr camObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV1, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV1, "2");

    ::fwAtomsPatch::helper::Object helper(camObjV1);

    helper.addAttribute("is_calibrated", ::fwAtoms::Boolean::New(true));
    helper.addAttribute("skew", ::fwAtoms::Numeric::New(0));
    helper.addAttribute("camera_id", ::fwAtoms::String::New("myCameraID"));
    helper.addAttribute("description", ::fwAtoms::String::New("my camera description"));
    ::fwAtoms::Sequence::sptr intrinsicParam = ::fwAtoms::Sequence::New();
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2596.78));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(2554.72));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(1020.74));
    intrinsicParam->push_back(::fwAtoms::Numeric::New(500.189));
    helper.addAttribute("intrinsic", intrinsicParam);
    helper.addAttribute("width", ::fwAtoms::Numeric::New(1920));
    helper.addAttribute("height", ::fwAtoms::Numeric::New(1080));
    helper.addAttribute("camera_source", ::fwAtoms::String::New("DEVICE"));
    helper.addAttribute("video_file", ::fwAtoms::String::New(""));
    helper.addAttribute("stream_url", ::fwAtoms::String::New(""));
    helper.addAttribute("max_framerate", ::fwAtoms::String::New("33"));
    helper.addAttribute("pixel_format", ::fwAtoms::String::New("RGBA32"));

    camObjV2 = ::fwAtoms::Object::dynamicCast(camObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV1] = camObjV2;

    ::arStructuralPatch::arData::Camera::V2ToV1::sptr cam2ToCam1Patch;
    cam2ToCam1Patch = ::arStructuralPatch::arData::Camera::V2ToV1::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV1, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    ::fwAtoms::Sequence::sptr intrinsicParam2 = ::fwAtoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
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
} //namespace arData
} //namespace ut
} //namespace arStructuralPatch

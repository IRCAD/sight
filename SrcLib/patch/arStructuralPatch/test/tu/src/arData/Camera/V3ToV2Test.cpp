/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/Camera/V3ToV2Test.hpp"

#include <arStructuralPatch/arData/Camera/V3ToV2.hpp>

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
CPPUNIT_TEST_SUITE_REGISTRATION( ::arStructuralPatch::ut::arData::Camera::V3ToV2Test );

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

void V3ToV2Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V3ToV2Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V3ToV2Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr camObjV3 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV3, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV3, "3");

    ::fwAtomsPatch::helper::Object helper(camObjV3);

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
    helper.addAttribute("scale", ::fwAtoms::Numeric::New(1.0162));

    camObjV2 = ::fwAtoms::Object::dynamicCast(camObjV3->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV3] = camObjV2;

    auto cam2ToCam1Patch = ::arStructuralPatch::arData::Camera::V3ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV3, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    ::fwAtoms::Sequence::sptr intrinsicParam2 = ::fwAtoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
    CPPUNIT_ASSERT_EQUAL(size_t(4), intrinsicParam2->size());
    CPPUNIT_ASSERT(camObjV3->getAttribute("width"));
    CPPUNIT_ASSERT_EQUAL(std::string("1920"), camObjV3->getAttribute("width")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("height"));
    CPPUNIT_ASSERT_EQUAL(std::string("1080"), camObjV3->getAttribute("height")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("max_framerate"));
    CPPUNIT_ASSERT_EQUAL(std::string("33"), camObjV3->getAttribute("max_framerate")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("video_file"));
    CPPUNIT_ASSERT_EQUAL(std::string("Videos/myVideo.mp4"), camObjV3->getAttribute("video_file")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("stream_url"));
    CPPUNIT_ASSERT(camObjV3->getAttribute("stream_url")->getString().empty());
    CPPUNIT_ASSERT(camObjV3->getAttribute("camera_source"));
    CPPUNIT_ASSERT_EQUAL(std::string("FILE"), camObjV3->getAttribute("camera_source")->getString());
    CPPUNIT_ASSERT(camObjV3->getAttribute("pixel_format"));
    CPPUNIT_ASSERT_EQUAL(std::string("RGBA32"), camObjV3->getAttribute("pixel_format")->getString());
    CPPUNIT_ASSERT(!camObjV2->getAttribute("scale"));
}

//------------------------------------------------------------------------------

} //namespace Camera
} //namespace arData
} //namespace ut
} //namespace arStructuralPatch

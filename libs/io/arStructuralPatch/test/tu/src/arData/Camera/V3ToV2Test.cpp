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

#include "arData/Camera/V3ToV2Test.hpp"

#include <arStructuralPatch/arData/Camera/V3ToV2.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

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
    atoms::Object::sptr camObjV3 = atoms::Object::New();
    atoms::Object::sptr camObjV2;

    ::fwAtomsPatch::helper::setClassname(camObjV3, "::arData::Camera");
    ::fwAtomsPatch::helper::setVersion(camObjV3, "3");

    ::fwAtomsPatch::helper::Object helper(camObjV3);

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
    helper.addAttribute("scale", atoms::Numeric::New(1.0162));

    camObjV2 = atoms::Object::dynamicCast(camObjV3->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[camObjV3] = camObjV2;

    auto cam2ToCam1Patch = ::arStructuralPatch::arData::Camera::V3ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(cam2ToCam1Patch->apply(camObjV3, camObjV2, newVersions));

    CPPUNIT_ASSERT(camObjV2);
    CPPUNIT_ASSERT(camObjV2->getAttribute("intrinsic"));
    atoms::Sequence::sptr intrinsicParam2 = atoms::Sequence::dynamicCast(camObjV2->getAttribute("intrinsic"));
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

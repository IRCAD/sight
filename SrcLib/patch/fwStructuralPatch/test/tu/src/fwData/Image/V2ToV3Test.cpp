/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/Image/V2ToV3Test.hpp"

#include <fwStructuralPatch/fwData/Image/V2ToV3.hpp>

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Image::V2ToV3Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Image
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
    auto patch = ::fwStructuralPatch::fwData::Image::V2ToV3::New();

    // test gray scale image
    {
        ::fwAtoms::Object::sptr imgObjV2 = ::fwAtoms::Object::New();
        ::fwAtoms::Object::sptr imgObjV3;

        ::fwAtomsPatch::helper::setClassname(imgObjV2, "::fwData::Image");
        ::fwAtomsPatch::helper::setVersion(imgObjV2, "2");

        ::fwAtomsPatch::helper::Object helper(imgObjV2);

        helper.addAttribute("nb_components", ::fwAtoms::Numeric::New(1));

        imgObjV3 = ::fwAtoms::Object::dynamicCast(imgObjV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[imgObjV2] = imgObjV3;

        CPPUNIT_ASSERT_NO_THROW(patch->apply(imgObjV2, imgObjV3, newVersions));

        CPPUNIT_ASSERT(imgObjV2);
        CPPUNIT_ASSERT(imgObjV3->getAttribute("pixel_format"));
        ::fwAtoms::String::sptr format = ::fwAtoms::String::dynamicCast(imgObjV3->getAttribute("pixel_format"));
        CPPUNIT_ASSERT(format->getValue() == "GRAY_SCALE");
    }

    // test RGB image
    {
        ::fwAtoms::Object::sptr imgObjV2 = ::fwAtoms::Object::New();
        ::fwAtoms::Object::sptr imgObjV3;

        ::fwAtomsPatch::helper::setClassname(imgObjV2, "::fwData::Image");
        ::fwAtomsPatch::helper::setVersion(imgObjV2, "2");

        ::fwAtomsPatch::helper::Object helper(imgObjV2);

        helper.addAttribute("nb_components", ::fwAtoms::Numeric::New(3));

        imgObjV3 = ::fwAtoms::Object::dynamicCast(imgObjV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[imgObjV2] = imgObjV3;

        CPPUNIT_ASSERT_NO_THROW(patch->apply(imgObjV2, imgObjV3, newVersions));

        CPPUNIT_ASSERT(imgObjV2);
        CPPUNIT_ASSERT(imgObjV3->getAttribute("pixel_format"));
        ::fwAtoms::String::sptr format = ::fwAtoms::String::dynamicCast(imgObjV3->getAttribute("pixel_format"));
        CPPUNIT_ASSERT(format->getValue() == "RGB");
    }

    // test RGBA image
    {
        ::fwAtoms::Object::sptr imgObjV2 = ::fwAtoms::Object::New();
        ::fwAtoms::Object::sptr imgObjV3;

        ::fwAtomsPatch::helper::setClassname(imgObjV2, "::fwData::Image");
        ::fwAtomsPatch::helper::setVersion(imgObjV2, "2");

        ::fwAtomsPatch::helper::Object helper(imgObjV2);

        helper.addAttribute("nb_components", ::fwAtoms::Numeric::New(4));

        imgObjV3 = ::fwAtoms::Object::dynamicCast(imgObjV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[imgObjV2] = imgObjV3;

        CPPUNIT_ASSERT_NO_THROW(patch->apply(imgObjV2, imgObjV3, newVersions));

        CPPUNIT_ASSERT(imgObjV2);
        CPPUNIT_ASSERT(imgObjV3->getAttribute("pixel_format"));
        ::fwAtoms::String::sptr format = ::fwAtoms::String::dynamicCast(imgObjV3->getAttribute("pixel_format"));
        CPPUNIT_ASSERT(format->getValue() == "RGBA");
    }

    // test UNDEFINED image
    {
        ::fwAtoms::Object::sptr imgObjV2 = ::fwAtoms::Object::New();
        ::fwAtoms::Object::sptr imgObjV3;

        ::fwAtomsPatch::helper::setClassname(imgObjV2, "::fwData::Image");
        ::fwAtomsPatch::helper::setVersion(imgObjV2, "2");

        ::fwAtomsPatch::helper::Object helper(imgObjV2);

        helper.addAttribute("nb_components", ::fwAtoms::Numeric::New(2));

        imgObjV3 = ::fwAtoms::Object::dynamicCast(imgObjV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[imgObjV2] = imgObjV3;

        CPPUNIT_ASSERT_NO_THROW(patch->apply(imgObjV2, imgObjV3, newVersions));

        CPPUNIT_ASSERT(imgObjV2);
        CPPUNIT_ASSERT(imgObjV3->getAttribute("pixel_format"));
        ::fwAtoms::String::sptr format = ::fwAtoms::String::dynamicCast(imgObjV3->getAttribute("pixel_format"));
        CPPUNIT_ASSERT(format->getValue() == "UNDEFINED");
    }
}

//------------------------------------------------------------------------------
} //namespace Image
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

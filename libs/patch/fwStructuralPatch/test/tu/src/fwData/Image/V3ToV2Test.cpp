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

#include "fwData/Image/V3ToV2Test.hpp"

#include <fwStructuralPatch/fwData/Image/V3ToV2.hpp>

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Image::V3ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Image
{

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
    auto patch = ::fwStructuralPatch::fwData::Image::V3ToV2::New();

    ::fwAtoms::Object::sptr imgObjV3 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr imgObjV2;

    ::fwAtomsPatch::helper::setClassname(imgObjV3, "::fwData::Image");
    ::fwAtomsPatch::helper::setVersion(imgObjV3, "3");

    ::fwAtomsPatch::helper::Object helper(imgObjV3);

    helper.addAttribute("nb_components", ::fwAtoms::Numeric::New(3));
    helper.addAttribute("pixel_format", ::fwAtoms::String::New("BGR"));

    imgObjV2 = ::fwAtoms::Object::dynamicCast(imgObjV3->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[imgObjV3] = imgObjV2;

    CPPUNIT_ASSERT_NO_THROW(patch->apply(imgObjV3, imgObjV2, newVersions));

    CPPUNIT_ASSERT(imgObjV3);
    CPPUNIT_ASSERT(nullptr == imgObjV2->getAttribute("pixel_format"));
}

//------------------------------------------------------------------------------
} //namespace Image
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

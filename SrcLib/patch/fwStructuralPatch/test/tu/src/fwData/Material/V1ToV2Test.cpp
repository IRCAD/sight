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

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/Object.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

#include <fwStructuralPatch/fwData/Material/V1ToV2.hpp>

#include <fwTest/Exception.hpp>

#include "fwData/Material/V1ToV2Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Material::V1ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Material
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

void V1ToV2Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr matObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr matObjV2;

    ::fwAtomsPatch::helper::setClassname(matObjV1, "::fwData::Material");
    ::fwAtomsPatch::helper::setVersion(matObjV1, "1");

    ::fwAtomsPatch::helper::Object helper(matObjV1);

    helper.addAttribute("shading_mode", ::fwAtoms::String::New("PHONG"));
    helper.addAttribute("representation_mode", ::fwAtoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", ::fwAtoms::String::New("STANDARD"));
    helper.addAttribute("ambient", ::fwAtoms::Object::sptr());
    helper.addAttribute("diffuse", ::fwAtoms::Object::sptr());

    matObjV2 = ::fwAtoms::Object::dynamicCast(matObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[matObjV1] = matObjV2;



    ::fwStructuralPatch::fwData::Material::V1ToV2::sptr mat1ToMat2Patch;
    mat1ToMat2Patch = ::fwStructuralPatch::fwData::Material::V1ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(mat1ToMat2Patch->apply(matObjV1, matObjV2, newVersions));

    CPPUNIT_ASSERT(matObjV2);
    CPPUNIT_ASSERT(matObjV2->getAttribute("diffuse_texture") == ::fwAtoms::Object::sptr());
    CPPUNIT_ASSERT(matObjV2->getAttribute("diffuse_texture_filtering"));
    CPPUNIT_ASSERT(matObjV2->getAttribute("diffuse_texture_wrapping"));
}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

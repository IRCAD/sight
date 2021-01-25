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

#include "data/Material/V2ToV3Test.hpp"

#include <fwStructuralPatch/data/Material/V2ToV3.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Material::V2ToV3Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace Material
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
    atoms::Object::sptr matObjV2 = atoms::Object::New();
    atoms::Object::sptr matObjV3;

    ::fwAtomsPatch::helper::setClassname(matObjV2, "::sight::data::Material");
    ::fwAtomsPatch::helper::setVersion(matObjV2, "2");

    ::fwAtomsPatch::helper::Object helper(matObjV2);

    helper.addAttribute("shading_mode", atoms::String::New("PHONG"));
    helper.addAttribute("representation_mode", atoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", atoms::String::New("STANDARD"));
    helper.addAttribute("ambient", atoms::Object::sptr());
    helper.addAttribute("diffuse", atoms::Object::sptr());
    helper.addAttribute("diffuse_texture", atoms::Object::sptr());
    helper.addAttribute("diffuse_texture_filtering", atoms::String::New("NEAREST"));
    helper.addAttribute("diffuse_texture_wrapping", atoms::String::New("CLAMP"));

    matObjV3 = atoms::Object::dynamicCast(matObjV2->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[matObjV2] = matObjV3;

    ::fwStructuralPatchdata::Material::V2ToV3::sptr mat2ToMat3Patch;
    mat2ToMat3Patch = ::fwStructuralPatchdata::Material::V2ToV3::New();
    CPPUNIT_ASSERT_NO_THROW(mat2ToMat3Patch->apply(matObjV2, matObjV3, newVersions));

    CPPUNIT_ASSERT(matObjV3);
    CPPUNIT_ASSERT(matObjV3->getAttribute("lighting"));
    atoms::Boolean::sptr lighting = atoms::Boolean::dynamicCast(matObjV3->getAttribute("lighting"));
    CPPUNIT_ASSERT(lighting->getValue() == true);
}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

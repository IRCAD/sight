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

#include "data/Material/V1ToV2Test.hpp"

#include <fwStructuralPatch/data/Material/V1ToV2.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Material::V1ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace Material
{
// force link for log
::utest::Exception e("");

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
    atoms::Object::sptr matObjV1 = atoms::Object::New();
    atoms::Object::sptr matObjV2;

    ::fwAtomsPatch::helper::setClassname(matObjV1, "::sight::data::Material");
    ::fwAtomsPatch::helper::setVersion(matObjV1, "1");

    ::fwAtomsPatch::helper::Object helper(matObjV1);

    helper.addAttribute("shading_mode", atoms::String::New("PHONG"));
    helper.addAttribute("representation_mode", atoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", atoms::String::New("STANDARD"));
    helper.addAttribute("ambient", atoms::Object::sptr());
    helper.addAttribute("diffuse", atoms::Object::sptr());

    matObjV2 = atoms::Object::dynamicCast(matObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[matObjV1] = matObjV2;

    ::fwStructuralPatchdata::Material::V1ToV2::sptr mat1ToMat2Patch;
    mat1ToMat2Patch = ::fwStructuralPatchdata::Material::V1ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(mat1ToMat2Patch->apply(matObjV1, matObjV2, newVersions));

    CPPUNIT_ASSERT(matObjV2);
    CPPUNIT_ASSERT(matObjV2->getAttribute("diffuse_texture") == atoms::Object::sptr());
    CPPUNIT_ASSERT(matObjV2->getAttribute("diffuse_texture_filtering"));
    CPPUNIT_ASSERT(matObjV2->getAttribute("diffuse_texture_wrapping"));
}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "data/Material/V3ToV4Test.hpp"

#include <fwStructuralPatch/data/Material/V3ToV4.hpp>

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
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Material::V3ToV4Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace Material
{

//------------------------------------------------------------------------------

void V3ToV4Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V3ToV4Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V3ToV4Test::applyPatchTest()
{
    atoms::Object::sptr matObjV3 = atoms::Object::New();
    atoms::Object::sptr matObjV4;

    ::fwAtomsPatch::helper::setClassname(matObjV3, "::sight::data::Material");
    ::fwAtomsPatch::helper::setVersion(matObjV3, "3");

    ::fwAtomsPatch::helper::Object helper(matObjV3);

    atoms::Sequence::sptr ambientSeq = atoms::Sequence::New();
    ambientSeq->push_back(atoms::Numeric::New(1.f));
    ambientSeq->push_back(atoms::Numeric::New(0.5f));
    ambientSeq->push_back(atoms::Numeric::New(0.6f));
    ambientSeq->push_back(atoms::Numeric::New(0.12f));
    atoms::Object::sptr ambient = atoms::Object::New();
    ambient->setAttribute("rgba", ambientSeq);

    atoms::Sequence::sptr diffuseSeq = atoms::Sequence::New();
    diffuseSeq->push_back(atoms::Numeric::New(0.33f));
    diffuseSeq->push_back(atoms::Numeric::New(0.32f));
    diffuseSeq->push_back(atoms::Numeric::New(0.2f));
    diffuseSeq->push_back(atoms::Numeric::New(0.125f));
    atoms::Object::sptr diffuse = atoms::Object::New();
    diffuse->setAttribute("rgba", diffuseSeq);

    helper.addAttribute("shading_mode", atoms::String::New("PHONG"));
    helper.addAttribute("representation_mode", atoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", atoms::String::New("STANDARD"));
    helper.addAttribute("ambient", ambient);
    helper.addAttribute("diffuse", diffuse);
    helper.addAttribute("diffuse_texture", atoms::Object::sptr());
    helper.addAttribute("diffuse_texture_filtering", atoms::String::New("NEAREST"));
    helper.addAttribute("diffuse_texture_wrapping", atoms::String::New("CLAMP"));
    helper.addAttribute("lighting", atoms::Boolean::New(false));

    // 1. Check that if lighting is false, then shading_mode is set to AMBIENT
    // 2. Check that diffuse is replaced by previous ambient and ambient is 0.05f
    {
        matObjV4 = atoms::Object::dynamicCast(matObjV3->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[matObjV3] = matObjV4;

        ::fwStructuralPatchdata::Material::V3ToV4::sptr mat2ToMat3Patch;
        mat2ToMat3Patch = ::fwStructuralPatchdata::Material::V3ToV4::New();
        CPPUNIT_ASSERT_NO_THROW(mat2ToMat3Patch->apply(matObjV3, matObjV4, newVersions));

        CPPUNIT_ASSERT(matObjV4);
        CPPUNIT_ASSERT(!matObjV4->getAttribute("lighting"));
        atoms::String::sptr shading = atoms::String::dynamicCast(matObjV4->getAttribute("shading_mode"));
        CPPUNIT_ASSERT(shading->getValue() == "AMBIENT");

        atoms::Object::sptr ambientV4 = atoms::Object::dynamicCast(matObjV4->getAttribute("ambient"));
        CPPUNIT_ASSERT(ambientV4);
        atoms::Sequence::sptr ambientSeqV4 = atoms::Sequence::dynamicCast(ambientV4->getAttribute("rgba"));
        CPPUNIT_ASSERT(ambientSeqV4);

        atoms::Object::sptr diffuseV4 = atoms::Object::dynamicCast(matObjV4->getAttribute("diffuse"));
        CPPUNIT_ASSERT(diffuseV4);
        atoms::Sequence::sptr diffuseSeqV4 = atoms::Sequence::dynamicCast(diffuseV4->getAttribute("rgba"));
        CPPUNIT_ASSERT(diffuseSeqV4);

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqV4)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqV4)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqV4)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqV4)[3])->getValue<float>());

        CPPUNIT_ASSERT_EQUAL(0.05f,
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqV4)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(0.05f,
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqV4)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(0.05f,
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqV4)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(1.f,
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqV4)[3])->getValue<float>());
    }

    // Check that if lighting is true, then shading_mode is not modifed
    {
        helper.replaceAttribute("lighting", atoms::Boolean::New(true));

        matObjV4 = atoms::Object::dynamicCast(matObjV3->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[matObjV3] = matObjV4;

        ::fwStructuralPatchdata::Material::V3ToV4::sptr mat2ToMat3Patch;
        mat2ToMat3Patch = ::fwStructuralPatchdata::Material::V3ToV4::New();
        CPPUNIT_ASSERT_NO_THROW(mat2ToMat3Patch->apply(matObjV3, matObjV4, newVersions));

        CPPUNIT_ASSERT(matObjV4);
        CPPUNIT_ASSERT(!matObjV4->getAttribute("lighting"));
        atoms::String::sptr shading = atoms::String::dynamicCast(matObjV4->getAttribute("shading_mode"));
        CPPUNIT_ASSERT(shading->getValue() == "PHONG");
    }

}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

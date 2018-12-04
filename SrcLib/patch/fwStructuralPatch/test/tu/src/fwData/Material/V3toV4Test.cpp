/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/Object.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

#include <fwStructuralPatch/fwData/Material/V3ToV4.hpp>

#include <fwTest/Exception.hpp>

#include "fwData/Material/V3ToV4Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Material::V3ToV4Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
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
    ::fwAtoms::Object::sptr matObjV3 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr matObjV4;

    ::fwAtomsPatch::helper::setClassname(matObjV3, "::fwData::Material");
    ::fwAtomsPatch::helper::setVersion(matObjV3, "3");

    ::fwAtomsPatch::helper::Object helper(matObjV3);

    ::fwAtoms::Sequence::sptr ambientSeq = ::fwAtoms::Sequence::New();
    ambientSeq->push_back(::fwAtoms::Numeric::New(1.f));
    ambientSeq->push_back(::fwAtoms::Numeric::New(0.5f));
    ambientSeq->push_back(::fwAtoms::Numeric::New(0.6f));
    ambientSeq->push_back(::fwAtoms::Numeric::New(0.12f));
    ::fwAtoms::Object::sptr ambient = ::fwAtoms::Object::New();
    ambient->setAttribute("rgba", ambientSeq);

    ::fwAtoms::Sequence::sptr diffuseSeq = ::fwAtoms::Sequence::New();
    diffuseSeq->push_back(::fwAtoms::Numeric::New(0.33f));
    diffuseSeq->push_back(::fwAtoms::Numeric::New(0.32f));
    diffuseSeq->push_back(::fwAtoms::Numeric::New(0.2f));
    diffuseSeq->push_back(::fwAtoms::Numeric::New(0.125f));
    ::fwAtoms::Object::sptr diffuse = ::fwAtoms::Object::New();
    diffuse->setAttribute("rgba", diffuseSeq);

    helper.addAttribute("shading_mode", ::fwAtoms::String::New("PHONG"));
    helper.addAttribute("representation_mode", ::fwAtoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", ::fwAtoms::String::New("STANDARD"));
    helper.addAttribute("ambient", ambient);
    helper.addAttribute("diffuse", diffuse);
    helper.addAttribute("diffuse_texture", ::fwAtoms::Object::sptr());
    helper.addAttribute("diffuse_texture_filtering", ::fwAtoms::String::New("NEAREST"));
    helper.addAttribute("diffuse_texture_wrapping", ::fwAtoms::String::New("CLAMP"));
    helper.addAttribute("lighting", ::fwAtoms::Boolean::New(false));

    // 1. Check that if lighting is false, then shading_mode is set to AMBIENT
    // 2. Check that diffuse is replaced by previous ambient and ambient is 0.05f
    {
        matObjV4 = ::fwAtoms::Object::dynamicCast(matObjV3->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[matObjV3] = matObjV4;

        ::fwStructuralPatch::fwData::Material::V3ToV4::sptr mat2ToMat3Patch;
        mat2ToMat3Patch = ::fwStructuralPatch::fwData::Material::V3ToV4::New();
        CPPUNIT_ASSERT_NO_THROW(mat2ToMat3Patch->apply(matObjV3, matObjV4, newVersions));

        CPPUNIT_ASSERT(matObjV4);
        CPPUNIT_ASSERT(!matObjV4->getAttribute("lighting"));
        ::fwAtoms::String::sptr shading = ::fwAtoms::String::dynamicCast(matObjV4->getAttribute("shading_mode"));
        CPPUNIT_ASSERT(shading->getValue() == "AMBIENT");

        ::fwAtoms::Object::sptr ambientV4 = ::fwAtoms::Object::dynamicCast(matObjV4->getAttribute("ambient"));
        CPPUNIT_ASSERT(ambientV4);
        ::fwAtoms::Sequence::sptr ambientSeqV4 = ::fwAtoms::Sequence::dynamicCast(ambientV4->getAttribute("rgba"));
        CPPUNIT_ASSERT(ambientSeqV4);

        ::fwAtoms::Object::sptr diffuseV4 = ::fwAtoms::Object::dynamicCast(matObjV4->getAttribute("diffuse"));
        CPPUNIT_ASSERT(diffuseV4);
        ::fwAtoms::Sequence::sptr diffuseSeqV4 = ::fwAtoms::Sequence::dynamicCast(diffuseV4->getAttribute("rgba"));
        CPPUNIT_ASSERT(diffuseSeqV4);

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqV4)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqV4)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqV4)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqV4)[3])->getValue<float>());

        CPPUNIT_ASSERT_EQUAL(0.05f,
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqV4)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(0.05f,
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqV4)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(0.05f,
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqV4)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(1.f,
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqV4)[3])->getValue<float>());
    }

    // Check that if lighting is true, then shading_mode is not modifed
    {
        helper.replaceAttribute("lighting", ::fwAtoms::Boolean::New(true));

        matObjV4 = ::fwAtoms::Object::dynamicCast(matObjV3->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[matObjV3] = matObjV4;

        ::fwStructuralPatch::fwData::Material::V3ToV4::sptr mat2ToMat3Patch;
        mat2ToMat3Patch = ::fwStructuralPatch::fwData::Material::V3ToV4::New();
        CPPUNIT_ASSERT_NO_THROW(mat2ToMat3Patch->apply(matObjV3, matObjV4, newVersions));

        CPPUNIT_ASSERT(matObjV4);
        CPPUNIT_ASSERT(!matObjV4->getAttribute("lighting"));
        ::fwAtoms::String::sptr shading = ::fwAtoms::String::dynamicCast(matObjV4->getAttribute("shading_mode"));
        CPPUNIT_ASSERT(shading->getValue() == "PHONG");
    }

}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

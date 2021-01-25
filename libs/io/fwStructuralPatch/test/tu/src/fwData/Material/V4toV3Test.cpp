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

#include "data/Material/V4ToV3Test.hpp"

#include <fwStructuralPatch/data/Material/V4ToV3.hpp>

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
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Material::V4ToV3Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace Material
{

//------------------------------------------------------------------------------

void V4ToV3Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V4ToV3Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V4ToV3Test::applyPatchTest()
{
    atoms::Object::sptr origObj = atoms::Object::New();
    atoms::Object::sptr targetObj;

    ::fwAtomsPatch::helper::setClassname(origObj, "::sight::data::Material");
    ::fwAtomsPatch::helper::setVersion(origObj, "4");

    ::fwAtomsPatch::helper::Object helper(origObj);

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

    helper.addAttribute("shading_mode", atoms::String::New("AMBIENT"));
    helper.addAttribute("representation_mode", atoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", atoms::String::New("CELLS_NORMALS"));
    helper.addAttribute("ambient", ambient);
    helper.addAttribute("diffuse", diffuse);
    helper.addAttribute("diffuse_texture", atoms::Object::sptr());
    helper.addAttribute("diffuse_texture_filtering", atoms::String::New("NEAREST"));
    helper.addAttribute("diffuse_texture_wrapping", atoms::String::New("CLAMP"));

    // 1. if shading_mode is set to AMBIENT, then lighting is set to false
    // 2. Check that diffuse is swapped by previous ambient
    // 3. Check that option_mode is set to NORMALS
    {
        targetObj = atoms::Object::dynamicCast(origObj->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[origObj] = targetObj;

        ::fwStructuralPatchdata::Material::V4ToV3::sptr patch;
        patch = ::fwStructuralPatchdata::Material::V4ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(patch->apply(origObj, targetObj, newVersions));

        CPPUNIT_ASSERT(targetObj);
        CPPUNIT_ASSERT(targetObj->getAttribute("lighting"));
        atoms::Boolean::sptr lighting = atoms::Boolean::dynamicCast(targetObj->getAttribute("lighting"));
        CPPUNIT_ASSERT(lighting);
        CPPUNIT_ASSERT_EQUAL(false, lighting->getValue());

        atoms::String::sptr shading = atoms::String::dynamicCast(targetObj->getAttribute("shading_mode"));
        CPPUNIT_ASSERT(shading->getValue() == "PHONG");

        atoms::Object::sptr ambientTarget = atoms::Object::dynamicCast(targetObj->getAttribute("ambient"));
        CPPUNIT_ASSERT(ambientTarget);
        atoms::Sequence::sptr ambientSeqTarget = atoms::Sequence::dynamicCast(ambientTarget->getAttribute(
                                                                                  "rgba"));
        CPPUNIT_ASSERT(ambientSeqTarget);

        atoms::Object::sptr diffuseTarget = atoms::Object::dynamicCast(targetObj->getAttribute("diffuse"));
        CPPUNIT_ASSERT(diffuseTarget);
        atoms::Sequence::sptr diffuseSeqTarget = atoms::Sequence::dynamicCast(diffuseTarget->getAttribute(
                                                                                  "rgba"));
        CPPUNIT_ASSERT(diffuseSeqTarget);

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqTarget)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqTarget)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqTarget)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeqTarget)[3])->getValue<float>());

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqTarget)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqTarget)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqTarget)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< atoms::Numeric >((*diffuseSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< atoms::Numeric >((*ambientSeqTarget)[3])->getValue<float>());

        atoms::String::sptr option = atoms::String::dynamicCast(targetObj->getAttribute("options_mode"));
        CPPUNIT_ASSERT_EQUAL(std::string("NORMALS"), option->getValue());

    }

    // if shading_mode is not "AMBIENT", the lighting is true
    {
        helper.replaceAttribute("shading_mode", atoms::String::New("PHONG"));

        targetObj = atoms::Object::dynamicCast(origObj->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[origObj] = targetObj;

        ::fwStructuralPatchdata::Material::V4ToV3::sptr patch;
        patch = ::fwStructuralPatchdata::Material::V4ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(patch->apply(origObj, targetObj, newVersions));

        CPPUNIT_ASSERT(targetObj);
        CPPUNIT_ASSERT(targetObj->getAttribute("lighting"));
        atoms::Boolean::sptr lighting = atoms::Boolean::dynamicCast(targetObj->getAttribute("lighting"));
        CPPUNIT_ASSERT(lighting);
        CPPUNIT_ASSERT_EQUAL(true, lighting->getValue());
    }

}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

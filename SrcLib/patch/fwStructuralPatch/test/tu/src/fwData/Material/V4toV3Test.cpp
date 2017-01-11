/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/Object.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

#include <fwStructuralPatch/fwData/Material/V4ToV3.hpp>

#include <fwTest/Exception.hpp>

#include "fwData/Material/V4ToV3Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Material::V4ToV3Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
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
    ::fwAtoms::Object::sptr origObj = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr targetObj;

    ::fwAtomsPatch::helper::setClassname(origObj, "::fwData::Material");
    ::fwAtomsPatch::helper::setVersion(origObj, "4");

    ::fwAtomsPatch::helper::Object helper(origObj);

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

    helper.addAttribute("shading_mode", ::fwAtoms::String::New("AMBIENT"));
    helper.addAttribute("representation_mode", ::fwAtoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", ::fwAtoms::String::New("CELLS_NORMALS"));
    helper.addAttribute("ambient", ambient);
    helper.addAttribute("diffuse", diffuse);
    helper.addAttribute("diffuse_texture", ::fwAtoms::Object::sptr());
    helper.addAttribute("diffuse_texture_filtering", ::fwAtoms::String::New("NEAREST"));
    helper.addAttribute("diffuse_texture_wrapping", ::fwAtoms::String::New("CLAMP"));

    // 1. if shading_mode is set to AMBIENT, then lighting is set to false
    // 2. Check that diffuse is swapped by previous ambient
    // 3. Check that option_mode is set to NORMALS
    {
        targetObj = ::fwAtoms::Object::dynamicCast(origObj->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[origObj] = targetObj;

        ::fwStructuralPatch::fwData::Material::V4ToV3::sptr patch;
        patch = ::fwStructuralPatch::fwData::Material::V4ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(patch->apply(origObj, targetObj, newVersions));

        CPPUNIT_ASSERT(targetObj);
        CPPUNIT_ASSERT(targetObj->getAttribute("lighting"));
        ::fwAtoms::Boolean::sptr lighting = ::fwAtoms::Boolean::dynamicCast(targetObj->getAttribute("lighting"));
        CPPUNIT_ASSERT(lighting);
        CPPUNIT_ASSERT_EQUAL(false, lighting->getValue());

        ::fwAtoms::String::sptr shading = ::fwAtoms::String::dynamicCast(targetObj->getAttribute("shading_mode"));
        CPPUNIT_ASSERT(shading->getValue() == "PHONG");

        ::fwAtoms::Object::sptr ambientTarget = ::fwAtoms::Object::dynamicCast(targetObj->getAttribute("ambient"));
        CPPUNIT_ASSERT(ambientTarget);
        ::fwAtoms::Sequence::sptr ambientSeqTarget = ::fwAtoms::Sequence::dynamicCast(ambientTarget->getAttribute(
                                                                                          "rgba"));
        CPPUNIT_ASSERT(ambientSeqTarget);

        ::fwAtoms::Object::sptr diffuseTarget = ::fwAtoms::Object::dynamicCast(targetObj->getAttribute("diffuse"));
        CPPUNIT_ASSERT(diffuseTarget);
        ::fwAtoms::Sequence::sptr diffuseSeqTarget = ::fwAtoms::Sequence::dynamicCast(diffuseTarget->getAttribute(
                                                                                          "rgba"));
        CPPUNIT_ASSERT(diffuseSeqTarget);

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqTarget)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqTarget)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqTarget)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeqTarget)[3])->getValue<float>());

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqTarget)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqTarget)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqTarget)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*diffuseSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< ::fwAtoms::Numeric >((*ambientSeqTarget)[3])->getValue<float>());

        ::fwAtoms::String::sptr option = ::fwAtoms::String::dynamicCast(targetObj->getAttribute("options_mode"));
        CPPUNIT_ASSERT_EQUAL(std::string("NORMALS"), option->getValue());

    }

    // if shading_mode is not "AMBIENT", the lighting is true
    {
        helper.replaceAttribute("shading_mode", ::fwAtoms::String::New("PHONG"));

        targetObj = ::fwAtoms::Object::dynamicCast(origObj->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[origObj] = targetObj;

        ::fwStructuralPatch::fwData::Material::V4ToV3::sptr patch;
        patch = ::fwStructuralPatch::fwData::Material::V4ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(patch->apply(origObj, targetObj, newVersions));

        CPPUNIT_ASSERT(targetObj);
        CPPUNIT_ASSERT(targetObj->getAttribute("lighting"));
        ::fwAtoms::Boolean::sptr lighting = ::fwAtoms::Boolean::dynamicCast(targetObj->getAttribute("lighting"));
        CPPUNIT_ASSERT(lighting);
        CPPUNIT_ASSERT_EQUAL(true, lighting->getValue());
    }

}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

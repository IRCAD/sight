/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Material/V4ToV3.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::utdata::Material::V4ToV3Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
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
    sight::atoms::Object::sptr origObj = sight::atoms::Object::New();
    sight::atoms::Object::sptr targetObj;

    io::atoms::patch::helper::setClassname(origObj, "::sight::data::Material");
    io::atoms::patch::helper::setVersion(origObj, "4");

    io::atoms::patch::helper::Object helper(origObj);

    sight::atoms::Sequence::sptr ambientSeq = sight::atoms::Sequence::New();
    ambientSeq->push_back(sight::atoms::Numeric::New(1.f));
    ambientSeq->push_back(sight::atoms::Numeric::New(0.5f));
    ambientSeq->push_back(sight::atoms::Numeric::New(0.6f));
    ambientSeq->push_back(sight::atoms::Numeric::New(0.12f));
    sight::atoms::Object::sptr ambient = sight::atoms::Object::New();
    ambient->setAttribute("rgba", ambientSeq);

    sight::atoms::Sequence::sptr diffuseSeq = sight::atoms::Sequence::New();
    diffuseSeq->push_back(sight::atoms::Numeric::New(0.33f));
    diffuseSeq->push_back(sight::atoms::Numeric::New(0.32f));
    diffuseSeq->push_back(sight::atoms::Numeric::New(0.2f));
    diffuseSeq->push_back(sight::atoms::Numeric::New(0.125f));
    sight::atoms::Object::sptr diffuse = sight::atoms::Object::New();
    diffuse->setAttribute("rgba", diffuseSeq);

    helper.addAttribute("shading_mode", sight::atoms::String::New("AMBIENT"));
    helper.addAttribute("representation_mode", sight::atoms::String::New("SURFACE"));
    helper.addAttribute("options_mode", sight::atoms::String::New("CELLS_NORMALS"));
    helper.addAttribute("ambient", ambient);
    helper.addAttribute("diffuse", diffuse);
    helper.addAttribute("diffuse_texture", sight::atoms::Object::sptr());
    helper.addAttribute("diffuse_texture_filtering", sight::atoms::String::New("NEAREST"));
    helper.addAttribute("diffuse_texture_wrapping", sight::atoms::String::New("CLAMP"));

    // 1. if shading_mode is set to AMBIENT, then lighting is set to false
    // 2. Check that diffuse is swapped by previous ambient
    // 3. Check that option_mode is set to NORMALS
    {
        targetObj = sight::atoms::Object::dynamicCast(origObj->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[origObj] = targetObj;

        patch::structural::data::Material::V4ToV3::sptr patch;
        patch = patch::structural::data::Material::V4ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(patch->apply(origObj, targetObj, newVersions));

        CPPUNIT_ASSERT(targetObj);
        CPPUNIT_ASSERT(targetObj->getAttribute("lighting"));
        sight::atoms::Boolean::sptr lighting = sight::atoms::Boolean::dynamicCast(targetObj->getAttribute("lighting"));
        CPPUNIT_ASSERT(lighting);
        CPPUNIT_ASSERT_EQUAL(false, lighting->getValue());

        sight::atoms::String::sptr shading = sight::atoms::String::dynamicCast(targetObj->getAttribute("shading_mode"));
        CPPUNIT_ASSERT(shading->getValue() == "PHONG");

        sight::atoms::Object::sptr ambientTarget =
            sight::atoms::Object::dynamicCast(targetObj->getAttribute("ambient"));
        CPPUNIT_ASSERT(ambientTarget);
        sight::atoms::Sequence::sptr ambientSeqTarget = sight::atoms::Sequence::dynamicCast(ambientTarget->getAttribute(
                                                                                                "rgba"));
        CPPUNIT_ASSERT(ambientSeqTarget);

        sight::atoms::Object::sptr diffuseTarget =
            sight::atoms::Object::dynamicCast(targetObj->getAttribute("diffuse"));
        CPPUNIT_ASSERT(diffuseTarget);
        sight::atoms::Sequence::sptr diffuseSeqTarget = sight::atoms::Sequence::dynamicCast(diffuseTarget->getAttribute(
                                                                                                "rgba"));
        CPPUNIT_ASSERT(diffuseSeqTarget);

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*ambientSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*diffuseSeqTarget)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*ambientSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*diffuseSeqTarget)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*ambientSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*diffuseSeqTarget)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*ambientSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*diffuseSeqTarget)[3])->getValue<float>());

        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*diffuseSeq)[0])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*ambientSeqTarget)[0])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*diffuseSeq)[1])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*ambientSeqTarget)[1])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*diffuseSeq)[2])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*ambientSeqTarget)[2])->getValue<float>());
        CPPUNIT_ASSERT_EQUAL(std::dynamic_pointer_cast< sight::atoms::Numeric >((*diffuseSeq)[3])->getValue<float>(),
                             std::dynamic_pointer_cast< sight::atoms::Numeric >(
                                 (*ambientSeqTarget)[3])->getValue<float>());

        sight::atoms::String::sptr option = sight::atoms::String::dynamicCast(targetObj->getAttribute("options_mode"));
        CPPUNIT_ASSERT_EQUAL(std::string("NORMALS"), option->getValue());

    }

    // if shading_mode is not "AMBIENT", the lighting is true
    {
        helper.replaceAttribute("shading_mode", sight::atoms::String::New("PHONG"));

        targetObj = sight::atoms::Object::dynamicCast(origObj->clone());

        io::atoms::patch::IPatch::NewVersionsType newVersions;
        newVersions[origObj] = targetObj;

        patch::structural::data::Material::V4ToV3::sptr patch;
        patch = patch::structural::data::Material::V4ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(patch->apply(origObj, targetObj, newVersions));

        CPPUNIT_ASSERT(targetObj);
        CPPUNIT_ASSERT(targetObj->getAttribute("lighting"));
        sight::atoms::Boolean::sptr lighting = sight::atoms::Boolean::dynamicCast(targetObj->getAttribute("lighting"));
        CPPUNIT_ASSERT(lighting);
        CPPUNIT_ASSERT_EQUAL(true, lighting->getValue());
    }

}

//------------------------------------------------------------------------------
} //namespace Material
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

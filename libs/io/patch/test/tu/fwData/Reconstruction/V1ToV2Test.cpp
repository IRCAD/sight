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

#include "V1ToV2Test.hpp"

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Reconstruction/V1ToV2.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::patch::structural::ut::data::Reconstruction::V1ToV2Test);

namespace sight::io::patch::structural
{

namespace ut
{

namespace data
{

namespace Reconstruction
{

// force link for log
utest::Exception e("");

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
    sight::atoms::Object::sptr recObjV1 = sight::atoms::Object::New();
    sight::atoms::Object::sptr recObjV2;

    io::atoms::patch::helper::setClassname(recObjV1, "::sight::data::Reconstruction");
    io::atoms::patch::helper::setVersion(recObjV1, "1");

    io::atoms::patch::helper::Object helper(recObjV1);

    helper.addAttribute("is_visible", sight::atoms::Boolean::New(true));
    helper.addAttribute("organ_name", sight::atoms::String::New("Organ"));
    helper.addAttribute("structure_type", sight::atoms::String::New(""));
    helper.addAttribute("material", sight::atoms::String::New(""));
    helper.addAttribute("image", sight::atoms::String::New(""));
    helper.addAttribute("mesh", sight::atoms::String::New(""));

    std::vector<std::string> v1Attributs;

    v1Attributs.push_back("reconstruction_format");
    v1Attributs.push_back("is_closed");
    v1Attributs.push_back("is_automatic");
    v1Attributs.push_back("avg_volume");
    v1Attributs.push_back("vol_deviation");
    v1Attributs.push_back("vol_pct_confidence");
    v1Attributs.push_back("reconstruction_time");
    v1Attributs.push_back("mask_generated");
    v1Attributs.push_back("level");
    v1Attributs.push_back("label");
    v1Attributs.push_back("generated_3D");
    v1Attributs.push_back("type_3D");
    v1Attributs.push_back("db_id");

    for(std::string attribut : v1Attributs)
    {
        helper.addAttribute(attribut, sight::atoms::String::New(""));
    }

    recObjV2 = sight::atoms::Object::dynamicCast(recObjV1->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[recObjV1] = recObjV2;

    patch::structural::data::Reconstruction::V1ToV2::sptr rec1Torec2Patch;
    rec1Torec2Patch = patch::structural::data::Reconstruction::V1ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(rec1Torec2Patch->apply(recObjV1, recObjV2, newVersions));

    CPPUNIT_ASSERT(recObjV2);
    CPPUNIT_ASSERT(recObjV2->getAttribute("is_visible"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("organ_name"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("structure_type"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("material"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("image"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("mesh"));

    for(std::string attribut : v1Attributs)
    {
        CPPUNIT_ASSERT_MESSAGE("'" + attribut + "' attribut must not exist", !recObjV2->getAttribute(attribut));
    }
}

//------------------------------------------------------------------------------

} //namespace Reconstruction

} //namespace data

} //namespace ut

} //namespace sight::io::patch::structural

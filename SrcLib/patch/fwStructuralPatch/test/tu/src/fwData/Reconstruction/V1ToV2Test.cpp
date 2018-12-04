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

#include <fwStructuralPatch/fwData/Reconstruction/V1ToV2.hpp>

#include <fwTest/Exception.hpp>

#include "fwData/Reconstruction/V1ToV2Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Reconstruction::V1ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Reconstruction
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
    ::fwAtoms::Object::sptr recObjV1 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr recObjV2;

    ::fwAtomsPatch::helper::setClassname(recObjV1, "::fwData::Reconstruction");
    ::fwAtomsPatch::helper::setVersion(recObjV1, "1");

    ::fwAtomsPatch::helper::Object helper(recObjV1);

    helper.addAttribute("is_visible", ::fwAtoms::Boolean::New(true));
    helper.addAttribute("organ_name", ::fwAtoms::String::New("Organ"));
    helper.addAttribute("structure_type", ::fwAtoms::String::New(""));
    helper.addAttribute("material", ::fwAtoms::String::New(""));
    helper.addAttribute("image", ::fwAtoms::String::New(""));
    helper.addAttribute("mesh", ::fwAtoms::String::New(""));

    std::vector< std::string> v1Attributs;

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

    for(std::string attribut :  v1Attributs)
    {
        helper.addAttribute(attribut, ::fwAtoms::String::New(""));
    }

    recObjV2 = ::fwAtoms::Object::dynamicCast(recObjV1->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[recObjV1] = recObjV2;



    ::fwStructuralPatch::fwData::Reconstruction::V1ToV2::sptr rec1Torec2Patch;
    rec1Torec2Patch = ::fwStructuralPatch::fwData::Reconstruction::V1ToV2::New();
    CPPUNIT_ASSERT_NO_THROW(rec1Torec2Patch->apply(recObjV1, recObjV2, newVersions));

    CPPUNIT_ASSERT(recObjV2);
    CPPUNIT_ASSERT(recObjV2->getAttribute("is_visible"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("organ_name"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("structure_type"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("material"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("image"));
    CPPUNIT_ASSERT(recObjV2->getAttribute("mesh"));

    for(std::string attribut :  v1Attributs)
    {
        CPPUNIT_ASSERT_MESSAGE("'" + attribut + "' attribut must not exist", !recObjV2->getAttribute(attribut));
    }
}

//------------------------------------------------------------------------------
} //namespace Reconstruction
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

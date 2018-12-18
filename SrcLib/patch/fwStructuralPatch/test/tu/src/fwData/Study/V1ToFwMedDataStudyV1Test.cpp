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

#include "fwData/Study/V1ToFwMedDataStudyV1Test.hpp"

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwStructuralPatch/fwData/Study/V1ToFwMedDataStudyV1.hpp>

#include <fwTest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Study::V1ToFwMedDataStudyV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Study
{


//------------------------------------------------------------------------------

void V1ToFwMedDataStudyV1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V1ToFwMedDataStudyV1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V1ToFwMedDataStudyV1Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr originObj = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr targetObj;

    ::fwAtomsPatch::helper::setClassname(originObj, "::fwData::Study");
    ::fwAtomsPatch::helper::setVersion(originObj, "1");

    ::fwAtomsPatch::helper::Object helper(originObj);

    helper.addAttribute("hospital", ::fwAtoms::String::New("hospital"));
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("acquisition_zone", ::fwAtoms::String::New(""));
    helper.addAttribute("uid", ::fwAtoms::String::New("4564634"));
    helper.addAttribute("db_id", ::fwAtoms::String::New("4529"));
    helper.addAttribute("date", ::fwAtoms::String::New("20130613"));
    helper.addAttribute("time", ::fwAtoms::String::New("102445"));
    helper.addAttribute("description", ::fwAtoms::String::New("description of study"));
    helper.addAttribute("acquisitions", ::fwAtoms::Sequence::New());

    targetObj = ::fwAtoms::Object::dynamicCast(originObj->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[originObj] = targetObj;

    ::fwStructuralPatch::fwData::Study::V1ToFwMedDataStudyV1::sptr patch;
    patch = ::fwStructuralPatch::fwData::Study::V1ToFwMedDataStudyV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(originObj, targetObj, newVersions));

    CPPUNIT_ASSERT(targetObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::fwMedData::Study"), ::fwAtomsPatch::helper::getClassname(targetObj));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), ::fwAtomsPatch::helper::getVersion(targetObj));

    CPPUNIT_ASSERT(!targetObj->getAttribute("hospital"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("modality"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("acquisition_zone"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("uid"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("db_id"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("acquisitions"));

    CPPUNIT_ASSERT(targetObj->getAttribute("instance_uid"));
    CPPUNIT_ASSERT(targetObj->getAttribute("date"));
    CPPUNIT_ASSERT(targetObj->getAttribute("time"));
    CPPUNIT_ASSERT(targetObj->getAttribute("referring_physician_name"));
    CPPUNIT_ASSERT(targetObj->getAttribute("description"));
    CPPUNIT_ASSERT(targetObj->getAttribute("patient_age"));

    ::fwAtoms::String::sptr uid = targetObj->getAttribute< ::fwAtoms::String >("instance_uid");
    CPPUNIT_ASSERT_EQUAL(std::string("4564634"), uid->getString());

    ::fwAtoms::String::sptr description = targetObj->getAttribute< ::fwAtoms::String >("description");
    CPPUNIT_ASSERT_EQUAL(std::string("description of study"), description->getString());

    ::fwAtoms::String::sptr date = targetObj->getAttribute< ::fwAtoms::String >("date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130613"), date->getString());

    ::fwAtoms::String::sptr time = targetObj->getAttribute< ::fwAtoms::String >("time");
    CPPUNIT_ASSERT_EQUAL(std::string("102445"), time->getString());
}

//------------------------------------------------------------------------------
} //namespace Study
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

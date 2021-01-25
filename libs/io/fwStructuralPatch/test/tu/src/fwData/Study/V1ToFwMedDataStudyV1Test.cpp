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

#include "data/Study/V1ToFwMedDataStudyV1Test.hpp"

#include <fwStructuralPatch/data/Study/V1ToFwMedDataStudyV1.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Study::V1ToFwMedDataStudyV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
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
    atoms::Object::sptr originObj = atoms::Object::New();
    atoms::Object::sptr targetObj;

    ::fwAtomsPatch::helper::setClassname(originObj, "::sight::data::Study");
    ::fwAtomsPatch::helper::setVersion(originObj, "1");

    ::fwAtomsPatch::helper::Object helper(originObj);

    helper.addAttribute("hospital", atoms::String::New("hospital"));
    helper.addAttribute("modality", atoms::String::New(""));
    helper.addAttribute("acquisition_zone", atoms::String::New(""));
    helper.addAttribute("uid", atoms::String::New("4564634"));
    helper.addAttribute("db_id", atoms::String::New("4529"));
    helper.addAttribute("date", atoms::String::New("20130613"));
    helper.addAttribute("time", atoms::String::New("102445"));
    helper.addAttribute("description", atoms::String::New("description of study"));
    helper.addAttribute("acquisitions", atoms::Sequence::New());

    targetObj = atoms::Object::dynamicCast(originObj->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[originObj] = targetObj;

    ::fwStructuralPatchdata::Study::V1ToFwMedDataStudyV1::sptr patch;
    patch = ::fwStructuralPatchdata::Study::V1ToFwMedDataStudyV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(originObj, targetObj, newVersions));

    CPPUNIT_ASSERT(targetObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Study"), ::fwAtomsPatch::helper::getClassname(targetObj));
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

    atoms::String::sptr uid = targetObj->getAttribute< atoms::String >("instance_uid");
    CPPUNIT_ASSERT_EQUAL(std::string("4564634"), uid->getString());

    atoms::String::sptr description = targetObj->getAttribute< atoms::String >("description");
    CPPUNIT_ASSERT_EQUAL(std::string("description of study"), description->getString());

    atoms::String::sptr date = targetObj->getAttribute< atoms::String >("date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130613"), date->getString());

    atoms::String::sptr time = targetObj->getAttribute< atoms::String >("time");
    CPPUNIT_ASSERT_EQUAL(std::string("102445"), time->getString());
}

//------------------------------------------------------------------------------
} //namespace Study
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

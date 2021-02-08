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

#include "V1ToFwMedDataStudyV1Test.hpp"

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Study/V1ToFwMedDataStudyV1.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::data::Study::V1ToFwMedDataStudyV1Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
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
    sight::atoms::Object::sptr originObj = sight::atoms::Object::New();
    sight::atoms::Object::sptr targetObj;

    io::atoms::patch::helper::setClassname(originObj, "::sight::data::Study");
    io::atoms::patch::helper::setVersion(originObj, "1");

    io::atoms::patch::helper::Object helper(originObj);

    helper.addAttribute("hospital", sight::atoms::String::New("hospital"));
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("acquisition_zone", sight::atoms::String::New(""));
    helper.addAttribute("uid", sight::atoms::String::New("4564634"));
    helper.addAttribute("db_id", sight::atoms::String::New("4529"));
    helper.addAttribute("date", sight::atoms::String::New("20130613"));
    helper.addAttribute("time", sight::atoms::String::New("102445"));
    helper.addAttribute("description", sight::atoms::String::New("description of study"));
    helper.addAttribute("acquisitions", sight::atoms::Sequence::New());

    targetObj = sight::atoms::Object::dynamicCast(originObj->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[originObj] = targetObj;

    patch::structural::data::Study::V1ToFwMedDataStudyV1::sptr patch;
    patch = patch::structural::data::Study::V1ToFwMedDataStudyV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(originObj, targetObj, newVersions));

    CPPUNIT_ASSERT(targetObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Study"), io::atoms::patch::helper::getClassname(targetObj));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), io::atoms::patch::helper::getVersion(targetObj));

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

    sight::atoms::String::sptr uid = targetObj->getAttribute< sight::atoms::String >("instance_uid");
    CPPUNIT_ASSERT_EQUAL(std::string("4564634"), uid->getString());

    sight::atoms::String::sptr description = targetObj->getAttribute< sight::atoms::String >("description");
    CPPUNIT_ASSERT_EQUAL(std::string("description of study"), description->getString());

    sight::atoms::String::sptr date = targetObj->getAttribute< sight::atoms::String >("date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130613"), date->getString());

    sight::atoms::String::sptr time = targetObj->getAttribute< sight::atoms::String >("time");
    CPPUNIT_ASSERT_EQUAL(std::string("102445"), time->getString());
}

//------------------------------------------------------------------------------
} //namespace Study
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

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

#include "V1ToFwMedDataPatientV1Test.hpp"

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Patient/V1ToFwMedDataPatientV1.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::patch::structural::ut::data::Patient::V1ToFwMedDataPatientV1Test);

namespace sight::io::patch::structural
{

namespace ut
{

namespace data
{

namespace Patient
{

//------------------------------------------------------------------------------

void V1ToFwMedDataPatientV1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V1ToFwMedDataPatientV1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V1ToFwMedDataPatientV1Test::applyPatchTest()
{
    sight::atoms::Object::sptr originObj = sight::atoms::Object::New();
    sight::atoms::Object::sptr targetObj;

    io::atoms::patch::helper::setClassname(originObj, "::sight::data::Patient");
    io::atoms::patch::helper::setVersion(originObj, "1");

    io::atoms::patch::helper::Object helper(originObj);

    helper.addAttribute("name", sight::atoms::String::New("name"));
    helper.addAttribute("firstname", sight::atoms::String::New("firstname"));
    helper.addAttribute("id_dicom", sight::atoms::String::New("46539"));
    helper.addAttribute("birthdate", sight::atoms::String::New("2013-Jun-13 10:10:10"));
    helper.addAttribute("is_male", sight::atoms::Boolean::New(false));
    helper.addAttribute("db_id", sight::atoms::String::New("4529"));
    helper.addAttribute("studies", sight::atoms::Sequence::New());

    targetObj = sight::atoms::Object::dynamicCast(originObj->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[originObj] = targetObj;

    patch::structural::data::Patient::V1ToFwMedDataPatientV1::sptr patch;
    patch = patch::structural::data::Patient::V1ToFwMedDataPatientV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(originObj, targetObj, newVersions));

    CPPUNIT_ASSERT(targetObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Patient"), io::atoms::patch::helper::getClassname(targetObj));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), io::atoms::patch::helper::getVersion(targetObj));

    CPPUNIT_ASSERT(!targetObj->getAttribute("firstname"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("id_dicom"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("birthdate"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("is_male"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("db_id"));
    CPPUNIT_ASSERT(!targetObj->getAttribute("studies"));

    CPPUNIT_ASSERT(targetObj->getAttribute("name"));
    CPPUNIT_ASSERT(targetObj->getAttribute("patient_id"));
    CPPUNIT_ASSERT(targetObj->getAttribute("birth_date"));
    CPPUNIT_ASSERT(targetObj->getAttribute("sex"));

    sight::atoms::String::sptr name = targetObj->getAttribute<sight::atoms::String>("name");
    CPPUNIT_ASSERT_EQUAL(std::string("name^firstname"), name->getString());

    sight::atoms::String::sptr patientId = targetObj->getAttribute<sight::atoms::String>("patient_id");
    CPPUNIT_ASSERT_EQUAL(std::string("46539"), patientId->getString());

    sight::atoms::String::sptr birthdate = targetObj->getAttribute<sight::atoms::String>("birth_date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130613"), birthdate->getString());

    sight::atoms::String::sptr sex = targetObj->getAttribute<sight::atoms::String>("sex");
    CPPUNIT_ASSERT_EQUAL(std::string("F"), sex->getString());
}

//------------------------------------------------------------------------------

} //namespace Patient

} //namespace data

} //namespace ut

} //namespace sight::io::patch::structural

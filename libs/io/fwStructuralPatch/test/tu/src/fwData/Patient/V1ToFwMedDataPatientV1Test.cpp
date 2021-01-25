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

#include "data/Patient/V1ToFwMedDataPatientV1Test.hpp"

#include <fwStructuralPatch/data/Patient/V1ToFwMedDataPatientV1.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Patient::V1ToFwMedDataPatientV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
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
    atoms::Object::sptr originObj = atoms::Object::New();
    atoms::Object::sptr targetObj;

    ::fwAtomsPatch::helper::setClassname(originObj, "::sight::data::Patient");
    ::fwAtomsPatch::helper::setVersion(originObj, "1");

    ::fwAtomsPatch::helper::Object helper(originObj);

    helper.addAttribute("name", atoms::String::New("name"));
    helper.addAttribute("firstname", atoms::String::New("firstname"));
    helper.addAttribute("id_dicom", atoms::String::New("46539"));
    helper.addAttribute("birthdate", atoms::String::New("2013-Jun-13 10:10:10"));
    helper.addAttribute("is_male", atoms::Boolean::New(false));
    helper.addAttribute("db_id", atoms::String::New("4529"));
    helper.addAttribute("studies", atoms::Sequence::New());

    targetObj = atoms::Object::dynamicCast(originObj->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[originObj] = targetObj;

    ::fwStructuralPatchdata::Patient::V1ToFwMedDataPatientV1::sptr patch;
    patch = ::fwStructuralPatchdata::Patient::V1ToFwMedDataPatientV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(originObj, targetObj, newVersions));

    CPPUNIT_ASSERT(targetObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Patient"), ::fwAtomsPatch::helper::getClassname(targetObj));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), ::fwAtomsPatch::helper::getVersion(targetObj));

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

    atoms::String::sptr name = targetObj->getAttribute< atoms::String >("name");
    CPPUNIT_ASSERT_EQUAL(std::string("name^firstname"), name->getString());

    atoms::String::sptr patientId = targetObj->getAttribute< atoms::String >("patient_id");
    CPPUNIT_ASSERT_EQUAL(std::string("46539"), patientId->getString());

    atoms::String::sptr birthdate = targetObj->getAttribute< atoms::String >("birth_date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130613"), birthdate->getString());

    atoms::String::sptr sex = targetObj->getAttribute< atoms::String >("sex");
    CPPUNIT_ASSERT_EQUAL(std::string("F"), sex->getString());
}

//------------------------------------------------------------------------------
} //namespace Patient
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch

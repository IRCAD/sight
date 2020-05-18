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


#include "fwData/Patient/V1ToFwMedDataPatientV1Test.hpp"

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwStructuralPatch/fwData/Patient/V1ToFwMedDataPatientV1.hpp>

#include <fwTest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Patient::V1ToFwMedDataPatientV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
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
    ::fwAtoms::Object::sptr originObj = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr targetObj;

    ::fwAtomsPatch::helper::setClassname(originObj, "::fwData::Patient");
    ::fwAtomsPatch::helper::setVersion(originObj, "1");

    ::fwAtomsPatch::helper::Object helper(originObj);

    helper.addAttribute("name", ::fwAtoms::String::New("name"));
    helper.addAttribute("firstname", ::fwAtoms::String::New("firstname"));
    helper.addAttribute("id_dicom", ::fwAtoms::String::New("46539"));
    helper.addAttribute("birthdate", ::fwAtoms::String::New("2013-Jun-13 10:10:10"));
    helper.addAttribute("is_male", ::fwAtoms::Boolean::New(false));
    helper.addAttribute("db_id", ::fwAtoms::String::New("4529"));
    helper.addAttribute("studies", ::fwAtoms::Sequence::New());

    targetObj = ::fwAtoms::Object::dynamicCast(originObj->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[originObj] = targetObj;

    ::fwStructuralPatch::fwData::Patient::V1ToFwMedDataPatientV1::sptr patch;
    patch = ::fwStructuralPatch::fwData::Patient::V1ToFwMedDataPatientV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(originObj, targetObj, newVersions));

    CPPUNIT_ASSERT(targetObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::fwMedData::Patient"), ::fwAtomsPatch::helper::getClassname(targetObj));
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

    ::fwAtoms::String::sptr name = targetObj->getAttribute< ::fwAtoms::String >("name");
    CPPUNIT_ASSERT_EQUAL(std::string("name^firstname"), name->getString());

    ::fwAtoms::String::sptr patientId = targetObj->getAttribute< ::fwAtoms::String >("patient_id");
    CPPUNIT_ASSERT_EQUAL(std::string("46539"), patientId->getString());

    ::fwAtoms::String::sptr birthdate = targetObj->getAttribute< ::fwAtoms::String >("birth_date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130613"), birthdate->getString());

    ::fwAtoms::String::sptr sex = targetObj->getAttribute< ::fwAtoms::String >("sex");
    CPPUNIT_ASSERT_EQUAL(std::string("F"), sex->getString());
}

//------------------------------------------------------------------------------
} //namespace Patient
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

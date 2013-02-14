/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/Patient.hpp"

#include "PatientTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::PatientTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void PatientTest::setUp()
{
    // Set up context before running a test.
    m_patient = ::fwMedData::Patient::New();
}

//------------------------------------------------------------------------------

void PatientTest::tearDown()
{
    // Clean up after the test run.
    m_patient.reset();
}

//------------------------------------------------------------------------------

void PatientTest::nameTest()
{
    const std::string name = "John DOE";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setName(name);
    CPPUNIT_ASSERT_EQUAL(name, m_patient->getName());
}

//------------------------------------------------------------------------------

void PatientTest::idTest()
{
    const std::string id = "42";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setId(id);
    CPPUNIT_ASSERT_EQUAL(id, m_patient->getId());
}

//------------------------------------------------------------------------------

void PatientTest::birthdateTest()
{
    const std::string birthdate = "02-14-2013 11:28";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setBirthdate(birthdate);
    CPPUNIT_ASSERT_EQUAL(birthdate, m_patient->getBirthdate());
}

//------------------------------------------------------------------------------

void PatientTest::sexTest()
{
    const std::string sex = "M";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setSex(sex);
    CPPUNIT_ASSERT_EQUAL(sex, m_patient->getSex());
}

//------------------------------------------------------------------------------



} //namespace ut
} //namespace fwMedData

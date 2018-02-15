/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwMedData/Study.hpp"

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
{
namespace ut
{

class StudyTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( StudyTest );
CPPUNIT_TEST(instanceUIDTest);
CPPUNIT_TEST(dateTest);
CPPUNIT_TEST(timeTest);
CPPUNIT_TEST(referringPhysicianNameTest);
CPPUNIT_TEST(descriptionTest);
CPPUNIT_TEST(patientAgeTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void instanceUIDTest();
    void dateTest();
    void timeTest();
    void referringPhysicianNameTest();
    void descriptionTest();
    void patientAgeTest();

private:
    Study::sptr m_study;
};

} //namespace ut
} //namespace fwMedData

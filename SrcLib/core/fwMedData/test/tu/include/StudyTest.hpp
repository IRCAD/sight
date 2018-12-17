/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

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

#include "TestService.hpp"

#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test many methods to create service (with uuid, with configuration,...).
 * And test service methods (start, stop and update).
 */
class ServiceTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ServiceTest );
CPPUNIT_TEST( testServiceCreation );
CPPUNIT_TEST( testServiceConfiguration );
CPPUNIT_TEST( testServiceCreationWithMultipleData );
CPPUNIT_TEST( testServiceCreationWithTemplateMethods );
CPPUNIT_TEST( testServiceCreationWithUUID );
CPPUNIT_TEST( testStartStopUpdate );
CPPUNIT_TEST( testStartStopUpdateExceptions );
CPPUNIT_TEST( testCommunication );
CPPUNIT_TEST( testWithInAndOut );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // fonctions de tests

    /// Test add and remove service
    void testServiceCreation();

    /// Test to configure a service
    void testServiceConfiguration();

    /// Test add and remove service with multiple associated data
    void testServiceCreationWithMultipleData();

    /// Test add and remove service using template methods
    void testServiceCreationWithTemplateMethods();

    /// Test add and remove service with UUID
    void testServiceCreationWithUUID();

    /// test start/stop/update service
    void testStartStopUpdate();

    /// test exceptions handling when starting/stopping/updating services
    void testStartStopUpdateExceptions();

    /// test sending/receiving message
    void testCommunication();

    /// test service with an input and an output
    void testWithInAndOut();

private:

    static void startStopUpdateExceptions(::fwServices::ut::TestService::sptr _service);
};

} //namespace ut
} //namespace fwServices

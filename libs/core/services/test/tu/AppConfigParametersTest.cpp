/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "AppConfigParametersTest.hpp"

#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/Runtime.hpp>

#include <services/registry/AppConfig.hpp>
#include <services/registry/AppConfigParameters.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::services::ut::AppConfigParametersTest );

namespace sight::services
{
namespace ut
{

//------------------------------------------------------------------------------

void AppConfigParametersTest::setUp()
{
    // Set up context before running a test.
    //modules location
    core::runtime::init();

    std::filesystem::path location = core::runtime::getResourceFilePath("tu_exec_services-0.0");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    auto& runtime = core::runtime::Runtime::get();
    runtime.addModules(location);

    core::runtime::loadModule("module_services");
    core::runtime::loadModule("AppConfigParametersTest");

    services::registry::AppConfigParameters::sptr appConfigParam;
    appConfigParam = services::registry::AppConfigParameters::getDefault();
    appConfigParam->parseBundleInformation();
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::tearDown()
{
    // Clean up after the test run.
    services::registry::AppConfigParameters::sptr appConfigParameters;
    appConfigParameters = services::registry::AppConfigParameters::getDefault();
    appConfigParameters->clearRegistry();
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::appConfigParametersTest()
{
    services::registry::AppConfigParameters::sptr appConfigParam;
    appConfigParam = services::registry::AppConfigParameters::getDefault();

    services::registry::FieldAdaptorType parameters = appConfigParam->getParameters("id1");
    CPPUNIT_ASSERT(!parameters.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value1"), parameters["param1"]);

    services::registry::FieldAdaptorType parameters2 = appConfigParam->getParameters("id2");
    CPPUNIT_ASSERT(!parameters2.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value3"), parameters2["param3"]);
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::concurentAccessToAppConfigParametersTest()
{
    const auto fn = std::bind(&AppConfigParametersTest::appConfigParametersTest, this);
    std::vector< std::future<void> > futures;
    for (unsigned int i = 0; i < 20; ++i)
    {
        futures.push_back( std::async(std::launch::async, fn) );
    }

    for (auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::services

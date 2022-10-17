/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <service/extension/AppConfig.hpp>
#include <service/extension/AppConfigParameters.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::AppConfigParametersTest);

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void AppConfigParametersTest::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::getResourceFilePath("tu_exec_service");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    auto& runtime = core::runtime::Runtime::get();
    runtime.addModules(location);

    core::runtime::loadModule("sight::module::service");
    core::runtime::loadModule("AppConfigParametersTest");

    service::extension::AppConfigParameters::sptr appConfigParam;
    appConfigParam = service::extension::AppConfigParameters::getDefault();
    appConfigParam->clearRegistry();
    appConfigParam->parseBundleInformation();
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::tearDown()
{
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::appConfigParametersTest()
{
    service::extension::AppConfigParameters::sptr appConfigParam;
    appConfigParam = service::extension::AppConfigParameters::getDefault();

    service::FieldAdaptorType parameters = appConfigParam->getParameters("id1");
    CPPUNIT_ASSERT(!parameters.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value1"), parameters["param1"]);

    service::FieldAdaptorType parameters2 = appConfigParam->getParameters("id2");
    CPPUNIT_ASSERT(!parameters2.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value3"), parameters2["param3"]);
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::concurentAccessToAppConfigParametersTest()
{
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 20 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, appConfigParametersTest));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut

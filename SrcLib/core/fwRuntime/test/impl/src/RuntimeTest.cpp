/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "RuntimeTest.hpp"

#include <fwRuntime/Extension.hpp>
#include <fwRuntime/impl/ExtensionPoint.hpp>
#include <fwRuntime/impl/Module.hpp>
#include <fwRuntime/impl/Runtime.hpp>
#include <fwRuntime/operations.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRuntime::impl::ut::RuntimeTest );

namespace fwRuntime
{
namespace impl
{
namespace ut
{

//------------------------------------------------------------------------------

RuntimeTest::RuntimeTest()
{
    // Set up context before running a test.
    ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();
    runtime->addDefaultBundles();
}

//------------------------------------------------------------------------------

void RuntimeTest::setUp()
{
}

//------------------------------------------------------------------------------

void RuntimeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void RuntimeTest::testRuntime()
{
    ::fwRuntime::impl::Runtime& runtime = ::fwRuntime::impl::Runtime::get();

    // Test bundle dataReg
    CPPUNIT_ASSERT(runtime.findModule("dataReg"));
    auto bundle = std::dynamic_pointer_cast< ::fwRuntime::impl::Module >(runtime.findModule("dataReg"));
    bundle->setEnable(true);
    CPPUNIT_ASSERT(bundle->isEnable());

    // Test bundle servicesReg
    CPPUNIT_ASSERT(runtime.findModule("servicesReg"));
    auto bundle2 = std::dynamic_pointer_cast< ::fwRuntime::impl::Module >(runtime.findModule("servicesReg"));
    bundle2->setEnable(true);
    CPPUNIT_ASSERT(bundle2->isEnable());

    // Test runtime extensions
    CPPUNIT_ASSERT(runtime.findExtensionPoint("::fwServices::registry::ServiceFactory"));
    CPPUNIT_ASSERT(runtime.findExtensionPoint("::fwServices::registry::ServiceConfig"));
    CPPUNIT_ASSERT(runtime.findExtensionPoint("::fwServices::registry::AppConfig"));
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace impl
} // namespace fwRuntime

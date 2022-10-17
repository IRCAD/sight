/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <core/base.hpp>

#include <service/IService.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::service::ut
{

/**
 * @brief   Test the build of objects from XML configurations.
 */
class ConfigParserTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ConfigParserTest);
CPPUNIT_TEST(testObjectCreationWithConfig);
CPPUNIT_TEST(testImageParser);
CPPUNIT_TEST(testTransferFunctionParser);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    /// test object with services creation from a configuration
    static void testObjectCreationWithConfig();
    static void testImageParser();
    static void testTransferFunctionParser();

private:

    /// Create a configuration to build an object
    static service::IService::config_t buildObjectConfig();
};

} // namespace sight::service::ut

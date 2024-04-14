/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::app::ut
{

/**
 * @brief   Test the build of objects from XML configurations.
 */
class data_parser_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(data_parser_test);
CPPUNIT_TEST(test_object_creation_with_config);
CPPUNIT_TEST(test_image_parser);
CPPUNIT_TEST(test_transfer_function_parser);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    /// test object with services creation from a configuration
    static void test_object_creation_with_config();
    static void test_image_parser();
    static void test_transfer_function_parser();

private:

    /// Create a configuration to build an object
    static service::config_t build_object_config();
};

} // namespace sight::app::ut

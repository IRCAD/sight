/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <core/runtime/runtime.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class config_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(config_test);
    CPPUNIT_TEST(basic);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void basic();

private:

    std::shared_ptr<core::runtime::module> m_data_module;
    std::shared_ptr<core::runtime::module> m_ut_config_module;
};

} // namespace sight::data::ut

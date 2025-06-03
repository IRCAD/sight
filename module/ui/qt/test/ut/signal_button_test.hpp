/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "core/runtime/module.hpp"

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::ui::qt::ut
{

/**
 * @brief Test many methods to create mesh.
 */
class signal_button_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(signal_button_test);
CPPUNIT_TEST(click_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void click_test();

private:

    /// The container service.
    sight::service::base::sptr m_container;

    // The worker for the test slots
    sight::core::thread::worker::sptr m_worker;

    /// The child uid to use to be added to the container.
    std::string m_child_uid;
    std::shared_ptr<sight::core::runtime::module> m_module;
};

} // namespace sight::module::ui::qt::ut

/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::ui::qt::ut
{

class progress_bar_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(progress_bar_test);
CPPUNIT_TEST(basic_test);
CPPUNIT_TEST(pulse_test);
CPPUNIT_TEST(svg_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void basic_test();
    void pulse_test();
    void svg_test();

private:

    void launch_test(
        bool _show_title,
        bool _show_cancel,
        bool _pulse,
        const std::string& _svg = std::string(),
        bool _show_log          = false
    );

    /// The container service.
    sight::service::base::sptr m_container;

    /// The child uuid to use to be added to the container.
    std::string m_child_uuid;
};

} // namespace sight::module::ui::qt::ut

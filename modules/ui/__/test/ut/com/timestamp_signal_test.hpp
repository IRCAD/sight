/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

namespace sight::module::ui::com::ut
{

class timestamp_signal_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(timestamp_signal_test);
CPPUNIT_TEST(system_clock_test);
CPPUNIT_TEST(high_res_clock_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void test(bool _use_system_clock);

    void system_clock_test();
    void high_res_clock_test();

private:

    service::base::sptr m_timestamp_signal;
    core::thread::worker::sptr m_worker;
};

} // namespace sight::module::ui::com::ut

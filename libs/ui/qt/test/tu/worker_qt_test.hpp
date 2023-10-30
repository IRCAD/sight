/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
#include <core/thread/worker.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::ui::qt::ut
{

class worker_qt_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(worker_qt_test);

    CPPUNIT_TEST(init_test);
    CPPUNIT_TEST(twice_init_test);
    CPPUNIT_TEST(basic_test);
    CPPUNIT_TEST(post_from_inside_test);
    CPPUNIT_TEST(post_from_outside_test);

    CPPUNIT_TEST(basic_timer_test);

    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void init_test();
    static void twice_init_test();
    void basic_test();

    void post_from_inside_test();
    void post_from_outside_test();

    void basic_timer_test();

protected:

    SPTR(core::thread::worker) m_worker;
};

} // namespace sight::ui::qt::ut

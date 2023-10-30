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

// cspell:ignore Doesnt

#pragma once

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::ui::com::ut
{

class DummyService;

class starter_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(starter_test);
CPPUNIT_TEST(start_test);
CPPUNIT_TEST(start_only_test);
CPPUNIT_TEST(start_if_exists_test);
CPPUNIT_TEST(start_if_exists_but_doesnt_exist_test);
CPPUNIT_TEST(stop_test);
CPPUNIT_TEST(stop_if_exists_test);
CPPUNIT_TEST(stop_if_exists_but_doesnt_exist_test);
CPPUNIT_TEST(start_or_stop_test);
CPPUNIT_TEST(start_only_or_stop_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    enum class should_be
    {
        started,
        stopped,
        started_then_stopped
    };

    void test(
        const std::string& _action_name,
        bool _exists,
        should_be _should_be = should_be::started,
        int _nb_update       = 0
    );

    void start_test();
    void start_only_test();
    void start_if_exists_test();
    void start_if_exists_but_doesnt_exist_test();
    void stop_test();
    void stop_if_exists_test();
    void stop_if_exists_but_doesnt_exist_test();
    void start_or_stop_test();
    void start_only_or_stop_test();

private:

    service::base::sptr m_starter;
    std::shared_ptr<DummyService> m_dummy_service;
};

} // namespace sight::module::ui::com::ut

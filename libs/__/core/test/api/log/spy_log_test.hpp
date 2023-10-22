/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <cppunit/extensions/HelperMacros.h>

#include <sstream>

namespace sight::core::log::ut
{

class spy_log_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(spy_log_test);
CPPUNIT_TEST(log_message_test);
CPPUNIT_TEST(thread_safety_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void log_message_test();
    void thread_safety_test();

private:

    static std::vector<std::string> log_to_vector(const std::stringstream& _logs_stream);
    static void check_log(
        const std::vector<std::string>& _log_messages_ref,
        const std::vector<std::string>& _log_messages
    );

    std::stringstream m_ostream;
};

} // namespace sight::core::log::ut

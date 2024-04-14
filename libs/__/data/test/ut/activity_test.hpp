/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include <data/activity.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data
{

namespace ut
{

class activity_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(activity_test);

CPPUNIT_TEST(activity_config_id_test);
CPPUNIT_TEST(data_test);
CPPUNIT_TEST(equality_test);
CPPUNIT_TEST(shallow_copy_test);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void activity_config_id_test();
    void data_test();
    static void equality_test();
    void shallow_copy_test();

private:

    SPTR(data::activity) m_activity;
};

} //namespace ut

} //namespace sight::data

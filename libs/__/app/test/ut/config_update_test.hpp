/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <app/config_manager.hpp>

#include <service/op.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::app::ut
{

/**
 * @brief Test the different possible configurations of the <update> tag in XML application configurations.
 */
class config_update_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(config_update_test);
CPPUNIT_TEST(simple_sequence_test);
CPPUNIT_TEST(imbricated_sequence_test);
CPPUNIT_TEST(mutiple_config_sequence_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void simple_sequence_test();
    void imbricated_sequence_test();
    void mutiple_config_sequence_test();

private:

    app::config_manager::sptr m_app_config_mgr;
};

} // namespace sight::app::ut

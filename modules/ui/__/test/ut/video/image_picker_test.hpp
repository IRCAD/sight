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

namespace sight::module::ui::video::ut
{

class image_picker_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_picker_test);
CPPUNIT_TEST(basic_test);
CPPUNIT_TEST(click_without_control_test);
CPPUNIT_TEST(top_left_ref_test);
CPPUNIT_TEST(single_point_mode_test);
CPPUNIT_TEST(no_ctrl_modifier_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void basic_test();
    void click_without_control_test();
    void top_left_ref_test();
    void single_point_mode_test();
    void no_ctrl_modifier_test();

private:

    service::base::sptr m_image_picker;
};

} // namespace sight::module::ui::video::ut

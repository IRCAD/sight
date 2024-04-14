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

#include <data/image_series.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class image_series_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_series_test);
CPPUNIT_TEST(image_test);
CPPUNIT_TEST(equality_test);
CPPUNIT_TEST(resize_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void image_test();
    static void equality_test();
    static void resize_test();

private:

    image_series::sptr m_series;
};

} // namespace sight::data::ut

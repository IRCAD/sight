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

// cspell:ignore Arent

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::activity::validator::ut
{

class related_study_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(related_study_test);
CPPUNIT_TEST(studies_are_related_test);
CPPUNIT_TEST(studies_arent_related_test);
CPPUNIT_TEST_SUITE_END();

public:

    static void studies_are_related_test();
    static void studies_arent_related_test();
};

} // namespace sight::activity::validator::ut

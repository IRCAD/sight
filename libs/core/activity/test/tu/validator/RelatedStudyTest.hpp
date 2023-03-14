/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

class RelatedStudyTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(RelatedStudyTest);
CPPUNIT_TEST(studiesAreRelatedTest);
CPPUNIT_TEST(studiesArentRelatedTest);
CPPUNIT_TEST_SUITE_END();

public:

    static void studiesAreRelatedTest();
    static void studiesArentRelatedTest();
};

} // namespace sight::activity::validator::ut
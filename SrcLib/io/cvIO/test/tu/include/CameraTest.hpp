/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

namespace cvIO
{

namespace ut
{

/**
 * @brief Intrinsic camera calibration conversion tests.
 */
class CameraTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE( CameraTest );
    CPPUNIT_TEST(copyToCv);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void copyToCv();
};

}

} // namespace cvIO

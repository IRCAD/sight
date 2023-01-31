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

#pragma once

#include <ui/testCore/ITest.hpp>

namespace sight::sightcalibrator::test::ui
{

class IntrinsicCalibration : public sight::ui::testCore::ITest
{
CPPUNIT_TEST_SUITE(IntrinsicCalibration);
CPPUNIT_TEST(test);
CPPUNIT_TEST_SUITE_END();

public:

    std::filesystem::path getProfilePath() override;

    void test();
};

} // namespace sight::sightcalibrator::test::ui

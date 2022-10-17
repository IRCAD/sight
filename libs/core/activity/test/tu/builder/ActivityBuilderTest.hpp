/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <activity/extension/Activity.hpp>

#include <core/macros.hpp>
#include <core/runtime/Module.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::activity
{

namespace ut
{

class ActivityBuilderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ActivityBuilderTest);

CPPUNIT_TEST(buildDataTest);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void buildDataTest();

protected:

    SPTR(core::runtime::Module) m_module;
    SPTR(activity::extension::Activity) m_activity;
};

} //namespace ut

} //namespace sight::activity

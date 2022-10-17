/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <service/AppManager.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::service::ut
{

/**
 * @brief Test .
 */
class AppManagerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(AppManagerTest);
CPPUNIT_TEST(managerCreationTest);
CPPUNIT_TEST(managerWithObjectTest);
CPPUNIT_TEST(managerWithObjectConnectionTest);
CPPUNIT_TEST(managerWithServiceConnectionTest);
CPPUNIT_TEST(managerWithOutputCreationTest);
CPPUNIT_TEST(managerWithGroupTest);
CPPUNIT_TEST(managerWithInputsTest);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void managerCreationTest();
    void managerWithObjectTest();
    void managerWithObjectConnectionTest();
    void managerWithServiceConnectionTest();
    void managerWithOutputCreationTest();
    void managerWithGroupTest();
    static void managerWithInputsTest();

private:

    std::unique_ptr<service::AppManager> m_appMgr;
};

class AppManagerForTest : public service::AppManager
{
public:

    friend class AppManagerTest;
    AppManagerForTest()
    = default;

    ~AppManagerForTest() override
    {
        this->destroy();
    }
};

} // namespace sight::service::ut

/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include <fwServices/AppManager.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test .
 */
class AppManagerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( AppManagerTest );
CPPUNIT_TEST( managerCreationTest );
CPPUNIT_TEST( managerWithObjectTest );
CPPUNIT_TEST( managerWithObjectConnectionTest );
CPPUNIT_TEST( managerWithServiceConnectionTest );
CPPUNIT_TEST( managerWithOutputCreationTest );
CPPUNIT_TEST( managerWithGroupTest );
CPPUNIT_TEST( managerWithInputsTest );

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void managerCreationTest();
    void managerWithObjectTest();
    void managerWithObjectConnectionTest();
    void managerWithServiceConnectionTest();
    void managerWithOutputCreationTest();
    void managerWithGroupTest();
    void managerWithInputsTest();

private:

    std::unique_ptr< ::fwServices::AppManager > m_appMgr;
};

class AppManagerForTest : public ::fwServices::AppManager
{
public:

    friend class AppManagerTest;
    AppManagerForTest()
    {

    }
    ~AppManagerForTest()
    {
        this->destroy();
    }
};

} //namespace ut
} //namespace fwServices

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void managerCreationTest();
    void managerWithObjectTest();
    void managerWithObjectConnectionTest();
    void managerWithServiceConnectionTest();

private:

    std::unique_ptr< ::fwServices::AppManager > m_appMgr;
};

} //namespace ut
} //namespace fwServices

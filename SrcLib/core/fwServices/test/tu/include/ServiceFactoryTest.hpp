/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>
namespace fwServices
{
namespace ut
{

/**
 * @brief
 */
class ServiceFactoryTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ServiceFactoryTest );
CPPUNIT_TEST( factoryTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void factoryTest();

private:
};

} //namespace ut
} //namespace fwServices

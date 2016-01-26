/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_SERVICECONFIGTEST_HPP__
#define __FWSERVICES_UT_SERVICECONFIGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test .
 */
class ServiceConfigTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ServiceConfigTest );
CPPUNIT_TEST( serviceConfigTest );
CPPUNIT_TEST( concurentAccessToServiceConfigTest );
CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();


    // fonctions de tests
    void serviceConfigTest();
    void concurentAccessToServiceConfigTest();

private:
    ::fwRuntime::ConfigurationElement::sptr buildConfig();
};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_SERVICECONFIGTEST_HPP__

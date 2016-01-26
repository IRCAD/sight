/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_APPCONFIGTEST_HPP__
#define __FWSERVICES_UT_APPCONFIGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test .
 */
class AppConfigTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( AppConfigTest );
CPPUNIT_TEST( parametersConfigTest );
CPPUNIT_TEST( concurentAccessToAppConfigTest );
CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();


    // fonctions de tests
    void parametersConfigTest();
    void concurentAccessToAppConfigTest();

private:
    ::fwRuntime::ConfigurationElement::sptr buildConfig();
    ::fwRuntime::ConfigurationElement::sptr buildParametersConfig();
};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_APPCONFIGTEST_HPP__

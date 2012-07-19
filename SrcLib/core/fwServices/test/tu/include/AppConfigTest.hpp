/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_TEST_TU_APPCONFIGTEST_HPP_
#define _FWSERVICES_TEST_TU_APPCONFIGTEST_HPP_

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
    CPPUNIT_TEST( standardConfigTest );
    CPPUNIT_TEST( templateConfigTest );
    CPPUNIT_TEST( parametersConfigTest );
    CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();


    // fonctions de tests
    void standardConfigTest();
    void templateConfigTest();
    void parametersConfigTest();

private:
    ::fwRuntime::ConfigurationElement::sptr buildConfig();
    ::fwRuntime::ConfigurationElement::sptr buildParametersConfig();
};

} //namespace ut
} //namespace fwServices

#endif // _FWSERVICES_TEST_TU_APPCONFIGTEST_HPP_

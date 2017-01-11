/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_CONFIGPARSERTEST_HPP__
#define __FWSERVICES_UT_CONFIGPARSERTEST_HPP__

#include <fwServices/macros.hpp>

#include <fwCore/base.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>


namespace fwServices
{
namespace ut
{

/**
 * @brief   Test ProcessObject building from a ConfigurationElement and composite message.
 * @see     ICompositeParser
 * @see     CompositeEditor
 */
class ConfigParserTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ConfigParserTest );
CPPUNIT_TEST( testObjectCreationWithConfig );
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    /// test object with services creation from a configuration
    void testObjectCreationWithConfig();

private:
    /// Create a configurationElement to build an object
    ::fwRuntime::ConfigurationElement::sptr buildObjectConfig();
};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_CONFIGPARSERTEST_HPP__

/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

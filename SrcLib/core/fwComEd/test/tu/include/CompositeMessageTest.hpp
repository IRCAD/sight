/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TEST_TU_COMPOSITEMESSAGETEST_HPP_
#define _FWCOMED_TEST_TU_COMPOSITEMESSAGETEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwRuntime/EConfigurationElement.hpp>


/**
 * @brief   Test ProcessObject building from a ConfigurationElement and composite message.
 * @see     ICompositeParser
 * @see     CompositeEditor
 * @see     CompositeMsg
 */
class CompositeMessageTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( CompositeMessageTest );
    CPPUNIT_TEST( testCompositeMessage );
//    CPPUNIT_TEST( testMessageNotification );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // fonctions de tests
    /// Test the compositeMsg with sending and receiving the message in the test services
    void testCompositeMessage();

    /// Test sending and receiving message from a composite's object.
    void testMessageNotification();

private:
    /// Create a configurationElement to build the composite
    ::fwRuntime::ConfigurationElement::sptr buildConfig();
};


#endif // _FWCOMED_TEST_TU_COMPOSITEMESSAGETEST_HPP_

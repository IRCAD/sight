/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_UT_FIELDHELPERTEST_HPP__
#define __FWCOMED_UT_FIELDHELPERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>

namespace fwComEd
{
namespace ut
{

/**
 * @brief   Test ProcessObject building from a ConfigurationElement and composite message.
 * @see     ICompositeParser
 * @see     CompositeEditor
 * @see     CompositeMsg
 */
class FieldHelperTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( FieldHelperTest );
CPPUNIT_TEST( testHelper );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // fonctions de tests
    /// Test the compositeMsg with sending and receiving the message in the test services
    void testHelper();

};


} //namespace ut
} //namespace fwComEd

#endif // __FWCOMED_UT_FIELDHELPERTEST_HPP__

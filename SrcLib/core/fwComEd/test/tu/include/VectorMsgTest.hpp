/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_UT_VECTORMSGTEST_HPP__
#define __FWCOMED_UT_VECTORMSGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include "ConfigParserTest.hpp"

namespace fwComEd
{
namespace ut
{

/**
 * @brief   Tests for VectorMsg.
 */
class VectorMsgTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( VectorMsgTest );
CPPUNIT_TEST( basicTest );
CPPUNIT_TEST( vectorHelperTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void basicTest();

    void vectorHelperTest();

};

} //namespace ut
} //namespace fwComEd

#endif // __FWCOMED_UT_VECTORMSGTEST_HPP__

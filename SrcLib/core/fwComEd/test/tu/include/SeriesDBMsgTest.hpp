/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_UT_SERIESDBMSGTEST_HPP__
#define __FWCOMED_UT_SERIESDBMSGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include "ConfigParserTest.hpp"

namespace fwComEd
{
namespace ut
{

/**
 * @brief   Tests for SeriesDBMsg.
 */
class SeriesDBMsgTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesDBMsgTest );
CPPUNIT_TEST( basicTest );
CPPUNIT_TEST( seriesDBHelperTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void basicTest();

    void seriesDBHelperTest();

};

} //namespace ut
} //namespace fwComEd

#endif // __FWCOMED_UT_SERIESDBMSGTEST_HPP__

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWLOG_UT_LOGGERTEST_HPP__
#define __FWLOG_UT_LOGGERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwLogTest
{
namespace ut
{

class LoggerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( LoggerTest );
CPPUNIT_TEST( simpleLoggerTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void simpleLoggerTest();

};

} // namespace ut
} // namespace fwLogTest

#endif // __FWLOG_UT_LOGGERTEST_HPP__

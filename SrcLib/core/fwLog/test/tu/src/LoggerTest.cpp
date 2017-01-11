/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "LoggerTest.hpp"

#include <fwLog/Logger.hpp>

#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwLogTest::ut::LoggerTest );

namespace fwLogTest
{
namespace ut
{

//------------------------------------------------------------------------------

void LoggerTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void LoggerTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void LoggerTest::simpleLoggerTest()
{
    ::fwLog::Logger::sptr logger = ::fwLog::Logger::New();

    const std::string info1     = "This is an information message.";
    const std::string info2     = "This is another information message.";
    const std::string info3     = "This is the last information message.";
    const std::string warning1  = "This is a warning message.";
    const std::string warning2  = "This is another warning message.";
    const std::string critical1 = "This is a critical message.";
    const std::string critical2 = "This is another critical message.";
    const std::string critical3 = "This is almost the last critical message.";
    const std::string critical4 = "This is the last critical message.";

    // Fill logger
    logger->information(info1);
    logger->critical(critical1);
    logger->warning(warning1);
    logger->information(info2);
    logger->critical(critical2);
    logger->critical(critical3);
    logger->information(info3);
    logger->warning(warning2);
    logger->critical(critical4);

    // Check number of logs
    std::size_t expected = 9;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count());
    expected = 3;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count(::fwLog::Log::INFORMATION));
    expected = 2;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count(::fwLog::Log::WARNING));
    expected = 4;
    CPPUNIT_ASSERT_EQUAL(expected, logger->count(::fwLog::Log::CRITICAL));

    // Check message values
    CPPUNIT_ASSERT_EQUAL(info1, logger->getLog(0).getMessage());
    CPPUNIT_ASSERT_EQUAL(critical1, logger->getLog(1).getMessage());
    CPPUNIT_ASSERT_EQUAL(warning1, logger->getLog(2).getMessage());
    CPPUNIT_ASSERT_EQUAL(info2, logger->getLog(3).getMessage());
    CPPUNIT_ASSERT_EQUAL(critical2, logger->getLog(4).getMessage());
    CPPUNIT_ASSERT_EQUAL(critical3, logger->getLog(5).getMessage());
    CPPUNIT_ASSERT_EQUAL(info3, logger->getLog(6).getMessage());
    CPPUNIT_ASSERT_EQUAL(warning2, logger->getLog(7).getMessage());
    CPPUNIT_ASSERT_EQUAL(critical4, logger->getLog(8).getMessage());

    // Check level types
    ::fwLog::Logger::IteratorType it = logger->begin();
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::INFORMATION, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::WARNING, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::INFORMATION, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::INFORMATION, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::WARNING, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());

    // Sort logs
    logger->sort();

    // Check level types
    it = logger->begin();
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::CRITICAL, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::WARNING, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::WARNING, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::INFORMATION, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::INFORMATION, (it++)->getLevel());
    CPPUNIT_ASSERT_EQUAL(::fwLog::Log::INFORMATION, (it++)->getLevel());

    // Clear logs
    logger->clear();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), logger->count());

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwLogTest

/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SpyLogTest.hpp"

#include <core/mt/types.hpp>
#include <core/spyLog.hpp>

#include <utest/Exception.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/log/core.hpp>

#include <exception>
#include <iostream>
#include <regex>
#include <streambuf>
#include <string>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::log::ut::SpyLogTest);

namespace sight::core::log::ut
{

static utest::Exception e("");

//------------------------------------------------------------------------------

void SpyLogTest::setUp()
{
    core::log::SpyLogger::add_console_log(m_ostream);
}

//-----------------------------------------------------------------------------

void SpyLogTest::tearDown()
{
    boost::log::core::get()->remove_all_sinks();
    m_ostream.clear();
}

//-----------------------------------------------------------------------------

void SpyLogTest::logMessageTest()
{
    std::vector<std::string> logs;

    logs.emplace_back("trace message");
    core::log::SpyLogger::trace(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::SpyLogTest::checkLog(logs, sight::core::log::ut::SpyLogTest::logToVector(m_ostream));

    logs.emplace_back("debug message");
    core::log::SpyLogger::debug(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::SpyLogTest::checkLog(logs, sight::core::log::ut::SpyLogTest::logToVector(m_ostream));

    logs.emplace_back("info message");
    core::log::SpyLogger::info(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::SpyLogTest::checkLog(logs, sight::core::log::ut::SpyLogTest::logToVector(m_ostream));

    logs.emplace_back("warn message");
    core::log::SpyLogger::warn(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::SpyLogTest::checkLog(logs, sight::core::log::ut::SpyLogTest::logToVector(m_ostream));

    logs.emplace_back("error message");
    core::log::SpyLogger::error(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::SpyLogTest::checkLog(logs, sight::core::log::ut::SpyLogTest::logToVector(m_ostream));

    logs.emplace_back("fatal message");
    core::log::SpyLogger::fatal(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::SpyLogTest::checkLog(logs, sight::core::log::ut::SpyLogTest::logToVector(m_ostream));
}

//-----------------------------------------------------------------------------

struct LogProducerThread
{
    using sptr             = std::shared_ptr<LogProducerThread>;
    using LogContainerType = std::vector<std::string>;

    LogProducerThread()
    = default;

    //------------------------------------------------------------------------------

    static void run(LogContainerType& logs, std::size_t nbLogs, std::size_t offset)
    {
        for(std::size_t i = offset ; i < nbLogs + offset ; ++i)
        {
            std::stringstream ss;
            ss << "msg n ";
            ss.width(10);
            ss.fill('0');
            ss << i;
            logs[i] = ss.str();
            core::log::SpyLogger::fatal(logs[i], __FILE__, __LINE__);
        }
    }
};

//-----------------------------------------------------------------------------

struct RegexLogCompare
{
    //------------------------------------------------------------------------------

    bool operator()(std::string a, std::string b)
    {
        std::regex re(".*(msg n [\\d]+)$");
        std::smatch matchA;
        std::smatch matchB;
        bool doMatchA = std::regex_match(a, matchA, re);
        bool doMatchB = std::regex_match(b, matchB, re);
        CPPUNIT_ASSERT_MESSAGE(std::string("Regex do not match ") + a, doMatchA);
        CPPUNIT_ASSERT_MESSAGE(std::string("Regex do not match ") + b, doMatchB);

        std::string strA(matchA[1].first, matchA[1].second);
        std::string strB(matchB[1].first, matchB[1].second);

        return strA < strB;
    }
} regex_compare;

//------------------------------------------------------------------------------

void SpyLogTest::threadSafetyTest()
{
    const std::size_t NB_THREAD(20);
    const std::size_t NB_LOG(20);
    LogProducerThread::LogContainerType logs(NB_THREAD * NB_LOG, "test");
    std::vector<std::thread> tg;
    for(std::size_t i = 0 ; i < NB_THREAD ; ++i)
    {
        LogProducerThread::sptr ct = std::make_shared<LogProducerThread>();
        std::size_t offset         = i * NB_LOG;
        tg.emplace_back([&, offset](auto&& ...){return LogProducerThread::run(logs, NB_LOG, offset);});
    }

    for(auto& t : tg)
    {
        t.join();
    }

    LogProducerThread::LogContainerType logMessages = sight::core::log::ut::SpyLogTest::logToVector(m_ostream);

    std::sort(logMessages.begin(), logMessages.end(), regex_compare);
    sight::core::log::ut::SpyLogTest::checkLog(logs, logMessages);
}

//-----------------------------------------------------------------------------

std::vector<std::string> SpyLogTest::logToVector(const std::stringstream& logsStream)
{
    std::vector<std::string> lines;
    std::string line;
    std::istringstream input;
    input.str(logsStream.str());
    while(std::getline(input, line))
    {
        lines.push_back(line);
    }

    return lines;
}

//-----------------------------------------------------------------------------

void SpyLogTest::checkLog(const std::vector<std::string>& logMessagesRef, const std::vector<std::string>& logMessages)
{
    CPPUNIT_ASSERT_EQUAL(logMessagesRef.size(), logMessages.size());

    const std::string linePattern("(\\[[0-9]+\\])");
    const std::string timePattern("(\\[.+\\])");
    const std::string levelPattern("( *\\[[a-z]+\\])");
    const std::string filePattern("( \\[.*:)");
    const std::string fileLinePattern("([0-9]+\\] )");
    const std::string messagePattern("(.*)$");

    std::regex re(std::string(linePattern).append(timePattern).append(levelPattern).append(filePattern)
                  .append(fileLinePattern).append(messagePattern));

    std::smatch match;
    std::string regexMessage;
    std::size_t i = 0;

    for(const std::string& log : logMessages)
    {
        const bool doMatch = std::regex_match(log, match, re);
        CPPUNIT_ASSERT_MESSAGE(log + " doesn't match regex.", doMatch);

        regexMessage.assign(match[6].first, match[6].second);
        CPPUNIT_ASSERT_EQUAL(logMessagesRef[i], regexMessage);
        ++i;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::core::log::ut

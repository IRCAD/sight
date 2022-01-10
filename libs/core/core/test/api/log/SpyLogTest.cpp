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

namespace sight::core::log
{

namespace ut
{

static utest::Exception e("");

//------------------------------------------------------------------------------

void SpyLogTest::setUp()
{
    core::log::SpyLogger& log = core::log::SpyLogger::get();
    log.add_console_log(m_ostream);
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
    core::log::SpyLogger& log = core::log::SpyLogger::get();
    std::vector<std::string> logs;

    logs.push_back("trace message");
    log.trace(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("debug message");
    log.debug(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("info message");
    log.info(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("warn message");
    log.warn(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("error message");
    log.error(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("fatal message");
    log.fatal(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));
}

//-----------------------------------------------------------------------------

struct LogProducerThread
{
    typedef std::shared_ptr<LogProducerThread> sptr;
    typedef std::vector<std::string> LogContainerType;

    LogProducerThread()
    {
    }

    //------------------------------------------------------------------------------

    void run(LogContainerType& logs, size_t nbLogs, size_t offset)
    {
        core::log::SpyLogger& log = core::log::SpyLogger::get();
        for(size_t i = offset ; i < nbLogs + offset ; ++i)
        {
            std::stringstream ss;
            ss << "msg n ";
            ss.width(10);
            ss.fill('0');
            ss << i;
            logs[i] = ss.str();
            log.fatal(logs[i], __FILE__, __LINE__);
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
    const size_t NB_THREAD(20);
    const size_t NB_LOG(20);
    LogProducerThread::LogContainerType logs(NB_THREAD * NB_LOG, "test");
    std::vector<std::thread> tg;
    for(size_t i = 0 ; i < NB_THREAD ; ++i)
    {
        LogProducerThread::sptr ct = std::make_shared<LogProducerThread>();
        size_t offset              = i * NB_LOG;
        tg.push_back(std::thread(std::bind(&LogProducerThread::run, ct, std::ref(logs), NB_LOG, offset)));
    }

    for(auto& t : tg)
    {
        t.join();
    }

    LogProducerThread::LogContainerType logMessages = this->logToVector(m_ostream);

    std::sort(logMessages.begin(), logMessages.end(), regex_compare);
    this->checkLog(logs, logMessages);
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

    std::regex re(
        linePattern
        + timePattern
        + levelPattern
        + filePattern
        + fileLinePattern
        + messagePattern);

    std::smatch match;
    std::string regexMessage;
    size_t i = 0;

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

} //namespace ut

} //namespace sight::core::log

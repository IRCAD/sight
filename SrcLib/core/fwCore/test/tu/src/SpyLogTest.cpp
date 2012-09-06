/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <streambuf>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/make_shared.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>

#include <fwCore/spyLog.hpp>
#include <fwCore/mt/types.hpp>

#include "SpyLogTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCore::ut::SpyLogTest );

namespace fwCore
{
namespace ut
{

void SpyLogTest::setUp()
{
    ::fwCore::log::SpyLogger& log = ::fwCore::log::SpyLogger::getSpyLogger();
    log.addStreamAppender(m_ostream);
}

//-----------------------------------------------------------------------------

void SpyLogTest::tearDown()
{
}

//-----------------------------------------------------------------------------

void SpyLogTest::logMessageTest()
{
    m_ostream.clear();
    ::fwCore::log::SpyLogger& log = ::fwCore::log::SpyLogger::getSpyLogger();

    std::vector< std::string > logs;

    logs.push_back("trace message");
    log.trace(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("debug message");
    log.debug(logs.back(), __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("info message");
    log.info(logs.back(),   __FILE__, __LINE__);
    this->checkLog(logs, this->logToVector(m_ostream));

    logs.push_back("warn message");
    log.warn(logs.back(),   __FILE__, __LINE__);
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
    typedef ::boost::shared_ptr< LogProducerThread > sptr;
    typedef std::vector< std::string > LogContainerType;

    LogProducerThread()
    {}

    void run(LogContainerType& logs, size_t nbLogs, size_t offset)
    {
        ::fwCore::log::SpyLogger& log = ::fwCore::log::SpyLogger::getSpyLogger();
        for(size_t i=offset; i < nbLogs + offset; ++i)
        {
            std::stringstream ss;
            ss << "msg n ";
            ss.width(10);
            ss.fill('0');
            ss << i ;
            logs[i] = ss.str();
            log.fatal(logs[i], __FILE__, __LINE__);
        }
    }
};

//-----------------------------------------------------------------------------

struct RegexLogCompare
{
  bool operator() (std::string a, std::string b)
  {
      ::boost::regex re(".*(msg n [[:digit:]]+)$");
      ::boost::smatch matchA;
      ::boost::smatch matchB;
      bool doMatchA = boost::regex_match(a, matchA, re);
      bool doMatchB = boost::regex_match(b, matchB, re);
      CPPUNIT_ASSERT_MESSAGE( std::string("Regex do not match ") + a, doMatchA);
      CPPUNIT_ASSERT_MESSAGE( std::string("Regex do not match ") + b, doMatchB);

      std::string strA(matchA[1].first, matchA[1].second);
      std::string strB(matchB[1].first, matchB[1].second);

      return strA < strB;
  }
} regex_compare;

void SpyLogTest::threadSafetyTest()
{
    m_ostream.clear();
    const size_t NB_THREAD(100);
    const size_t NB_LOG(100);
    LogProducerThread::LogContainerType logs(NB_THREAD * NB_LOG, "test");
    ::boost::thread_group tg;
    for(size_t i = 0; i < NB_THREAD; ++i)
    {
        LogProducerThread::sptr ct = ::boost::make_shared<LogProducerThread>();
        size_t offset = i * NB_LOG;
        ::boost::thread* t  = new ::boost::thread(::boost::bind(&LogProducerThread::run, ct, boost::ref(logs), NB_LOG, offset) );
        tg.add_thread(t);
    }
    tg.join_all();

    LogProducerThread::LogContainerType logMessages = this->logToVector(m_ostream);

    std::sort( logMessages.begin(), logMessages.end(), regex_compare);
    this->checkLog(logs, logMessages);
}

//-----------------------------------------------------------------------------

std::vector<std::string> SpyLogTest::logToVector(const std::stringstream &logsStream)
{
    std::vector<std::string> lines;
    ::boost::split_regex(lines, logsStream.str(), ::boost::regex("\r?\n"));

    if(lines.back().empty())
    {
        lines.pop_back();
    }
    return lines;
}

//-----------------------------------------------------------------------------

void SpyLogTest::checkLog(const std::vector<std::string> &logMessagesRef, const std::vector<std::string> &logMessages)
{
    CPPUNIT_ASSERT_EQUAL(logMessagesRef.size(), logMessages.size());

    const std::string countPattern("(\\[[0-9]+\\])");
    const std::string processPattern("(\\[.+\\])");
    const std::string threadPattern("(\\[.+\\])");
    const std::string timePattern("(\\[.+\\])");
    const std::string levelPattern("( *\\[[A-Z]+\\])");
    const std::string filePattern("( .*:)");
    const std::string linePattern("([0-9]+: )");
    const std::string messagePattern("(.*)$");

    ::boost::regex re(
            countPattern
            + processPattern
            + threadPattern
            + timePattern
            + levelPattern
            + filePattern
            + linePattern
            + messagePattern
            );
    ::boost::smatch match;
    std::string regexMessage;
    size_t i=0;
    BOOST_FOREACH(const std::string &log, logMessages)
    {
        bool doMatch = boost::regex_match(log, match, re);
        CPPUNIT_ASSERT_MESSAGE(log + " don't match regex.", doMatch);

        regexMessage.assign(match[8].first, match[8].second);

        CPPUNIT_ASSERT_EQUAL(logMessagesRef[i], regexMessage);
        ++i;
    }
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwCore

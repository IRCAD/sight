/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_UT_SPYLOGTEST_HPP__
#define __FWCORE_UT_SPYLOGTEST_HPP__

#include <sstream>

#include <boost/filesystem/path.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwCore
{
namespace ut
{

class SpyLogTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SpyLogTest );
    CPPUNIT_TEST( logMessageTest );
    CPPUNIT_TEST( threadSafetyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void logMessageTest();
    void threadSafetyTest();

private:

    std::vector<std::string> logToVector(const std::stringstream &logsStream);
    void checkLog(const std::vector<std::string> &logMessagesRef, const std::vector<std::string> &logMessages);

    ::boost::filesystem::path m_tempFile;
    std::stringstream m_ostream;
};

} //namespace ut
} //namespace fwCore

#endif //__FWCORE_UT_SPYLOGTEST_HPP__

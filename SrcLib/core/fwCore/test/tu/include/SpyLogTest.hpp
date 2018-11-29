/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWCORE_UT_SPYLOGTEST_HPP__
#define __FWCORE_UT_SPYLOGTEST_HPP__

#include <sstream>

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

    std::stringstream m_ostream;
};

} //namespace ut
} //namespace fwCore

#endif //__FWCORE_UT_SPYLOGTEST_HPP__

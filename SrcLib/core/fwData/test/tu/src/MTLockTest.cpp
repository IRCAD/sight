/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "MTLockTest.hpp"

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <functional>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MTLockTest );

namespace fwData
{
namespace ut
{

//-----------------------------------------------------------------------------

void MTLockTest::setUp()
{
    // Set up context before running a test.
    m_string = ::fwData::String::New();
}

//-----------------------------------------------------------------------------

void MTLockTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void MTLockTest::lockTest()
{
    auto future = std::async(std::launch::async, std::bind(&MTLockTest::runLock, this));

    const std::future_status status = future.wait_for(std::chrono::seconds(2));
    CPPUNIT_ASSERT(status == std::future_status::ready);
    future.get(); // Trigger exceptions

}

//-----------------------------------------------------------------------------

void MTLockTest::runLock()
{
    {
        ::fwData::mt::ObjectReadLock readLock(m_string);
    }

    {
        ::fwData::mt::ObjectWriteLock writeLock(m_string);

        writeLock.unlock();

        ::fwData::mt::ObjectReadToWriteLock updrageLock(m_string);

        updrageLock.unlock();
        writeLock.lock();

        writeLock.unlock();

        updrageLock.lock();

        updrageLock.upgrade();

        updrageLock.downgrade();

        ::fwData::mt::ObjectReadLock readLock(m_string);

        readLock.unlock();

        updrageLock.unlock();

        writeLock.lock();
    }
}

//-----------------------------------------------------------------------------

void MTLockTest::multipleLockTest()
{
    auto future1 = std::async(std::launch::async, std::bind(&MTLockTest::runMultipleLock1, this));
    auto future2 = std::async(std::launch::async, std::bind(&MTLockTest::runMultipleLock2, this));

    const std::future_status status1 = future1.wait_for(std::chrono::milliseconds(2500));
    CPPUNIT_ASSERT(status1 == std::future_status::ready);
    future1.get(); // Trigger exceptions

    const std::future_status status2 = future2.wait_for(std::chrono::milliseconds(2500));
    CPPUNIT_ASSERT(status2 == std::future_status::ready);
    future2.get(); // Trigger exceptions

    CPPUNIT_ASSERT_MESSAGE(m_string->value(), m_string->value() == "lili" ||  m_string->value() == "toto");
}

//-----------------------------------------------------------------------------

void MTLockTest::runMultipleLock1()
{
    {
        ::fwData::mt::ObjectWriteLock writeLock(m_string);
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "t";
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "o";
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "t";
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "o";
    }
    std::this_thread::sleep_for( std::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadLock readLock(m_string);
        CPPUNIT_ASSERT(m_string->value().find("toto") != std::string::npos);
    }
    std::this_thread::sleep_for( std::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadToWriteLock lock(m_string);
        if (m_string->value() == "totolili")
        {
            std::this_thread::sleep_for( std::chrono::milliseconds(100));
            lock.upgrade();
            m_string->value() = "toto";
        }
    }
}

//-----------------------------------------------------------------------------

void MTLockTest::runMultipleLock2()
{
    {
        ::fwData::mt::ObjectWriteLock writeLock(m_string);
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "l";
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "i";
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "l";
        std::this_thread::sleep_for( std::chrono::milliseconds(100));
        m_string->value() += "i";
    }
    std::this_thread::sleep_for( std::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadLock readLock(m_string);
        CPPUNIT_ASSERT(m_string->value().find("lili") != std::string::npos);
    }
    std::this_thread::sleep_for( std::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadToWriteLock lock(m_string);
        if (m_string->value() == "lilitoto")
        {
            std::this_thread::sleep_for( std::chrono::milliseconds(100));
            lock.upgrade();
            m_string->value() = "lili";
        }
    }
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwData

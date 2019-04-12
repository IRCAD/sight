/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
    // Initialize the string with at least 4 letters, we need that in our tests
    m_string = ::fwData::String::New("tata");
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
    auto future1 = std::async(std::launch::async, std::bind(&MTLockTest::runMultipleLock, this, 100, "tata"));
    auto future2 = std::async(std::launch::async, std::bind(&MTLockTest::runMultipleLock, this, 99, "lili"));

    // read the last four letters and check that it is "tata" or "lili"
    for (size_t i = 0; i < 40; i += 4)
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(10));
        ::fwData::mt::ObjectReadLock readLock(m_string);
        const std::string str = m_string->value();

        const std::string substr = str.substr(str.size()-4, 4);
        CPPUNIT_ASSERT_MESSAGE(substr, substr == "lili" ||  substr == "tata");
    }

    // read the last four letters and replace by something else
    for (size_t i = 0; i < 40; i += 4)
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(10));
        ::fwData::mt::ObjectReadToWriteLock lock(m_string);
        std::string& str         = m_string->value();
        const std::string substr = str.substr(str.size()-4, 4);
        if (substr == "tata")
        {
            lock.upgrade();
            str.replace(str.size()-4, 4, "lili");
        }
        else if (substr == "lili")
        {
            lock.upgrade();
            str.replace(str.size()-4, 4, "tata");
        }
    }

    CPPUNIT_ASSERT_NO_THROW(future1.get()); // Trigger exceptions
    CPPUNIT_ASSERT_NO_THROW(future2.get()); // Trigger exceptions

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(200*4), m_string->value().size());

    // check that the letters from "lili" and "tata" are not mixed
    const std::string str = m_string->value();
    for (size_t i = 0; i < str.size(); i += 4)
    {
        const std::string substr = str.substr(i, 4);
        CPPUNIT_ASSERT_MESSAGE(m_string->value(), substr == "lili" ||  substr == "tata");
    }
}

//-----------------------------------------------------------------------------

void MTLockTest::runMultipleLock(size_t nb, const char value[4])
{
    for (size_t i = 0; i < nb; ++i)
    {
        {
            ::fwData::mt::ObjectWriteLock writeLock(m_string);
            std::string& str = m_string->value();
            std::this_thread::sleep_for( std::chrono::milliseconds(2));
            str += value[0];
            std::this_thread::sleep_for( std::chrono::milliseconds(2));
            str += value[1];
            std::this_thread::sleep_for( std::chrono::milliseconds(2));
            str += value[2];
            std::this_thread::sleep_for( std::chrono::milliseconds(2));
            str += value[3];
        }
        std::this_thread::sleep_for( std::chrono::milliseconds(5));
    }
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwData

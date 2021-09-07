/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "SessionTest.hpp"

#include <core/thread/ActiveWorkers.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::session::ut::SessionTest);

namespace sight::module::io::session
{

namespace ut
{

//------------------------------------------------------------------------------

void SessionTest::setUp()
{
    // Set up context before running a test.
    core::thread::Worker::sptr worker = core::thread::Worker::New();
    core::thread::ActiveWorkers::setDefaultWorker(worker);
}

//------------------------------------------------------------------------------

void SessionTest::tearDown()
{
    // Clean up after the test run.
    core::thread::ActiveWorkers::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

void SessionTest::BasicTest()
{
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::module::io::session

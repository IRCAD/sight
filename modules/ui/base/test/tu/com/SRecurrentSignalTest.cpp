/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SRecurrentSignalTest.hpp"

#include <core/com/Slot.hxx>

#include <service/base.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::com::ut::SRecurrentSignalTest);

namespace sight::module::ui::base::com::ut
{

//------------------------------------------------------------------------------

void SRecurrentSignalTest::setUp()
{
    m_recurrentSignal = service::add("sight::module::ui::base::com::SRecurrentSignal");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::base::com::SRecurrentSignal'",
        m_recurrentSignal
    );
}

//------------------------------------------------------------------------------

void SRecurrentSignalTest::tearDown()
{
    m_worker->stop();
    if(!m_recurrentSignal->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_recurrentSignal->stop().get());
    }

    service::remove(m_recurrentSignal);
}

//------------------------------------------------------------------------------

void SRecurrentSignalTest::basicTest()
{
    using namespace std::literals::chrono_literals;

    int nbRepeat = 0;
    CPPUNIT_ASSERT_NO_THROW(m_recurrentSignal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_recurrentSignal->start().get());

    auto repeatedSlot = core::com::newSlot(
        [&nbRepeat]
        {
            nbRepeat++;
        });
    m_worker = core::thread::Worker::New();
    repeatedSlot->setWorker(m_worker);
    m_recurrentSignal->signal("repeated")->connect(repeatedSlot);

    std::this_thread::sleep_for(999ms);
    // 999/100 = 9, but on the CI 8 might be returned, most likely because of heavy load, so we tolerate 8.
    CPPUNIT_ASSERT(nbRepeat == 9 || nbRepeat == 8);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::com::ut

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

#include "CrossTypeActionTest.hpp"

#include <core/com/Slot.hxx>

#include <service/base.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::viz::ut::CrossTypeActionTest);

namespace sight::module::ui::base::viz::ut
{

//------------------------------------------------------------------------------

void CrossTypeActionTest::setUp()
{
    m_crossTypeAction = service::add("sight::module::ui::base::viz::CrossTypeAction");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::base::viz::CrossTypeAction'",
        m_crossTypeAction
    );
}

//------------------------------------------------------------------------------

void CrossTypeActionTest::tearDown()
{
    m_worker->stop();
    if(!m_crossTypeAction->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_crossTypeAction->stop().get());
    }

    service::remove(m_crossTypeAction);
}

//------------------------------------------------------------------------------

void CrossTypeActionTest::test(const std::string& crossType, double expectedScale)
{
    boost::property_tree::ptree ptree;
    ptree.put("crossType", crossType);
    m_crossTypeAction->setConfiguration(ptree);
    double scale               = -1;
    auto crossTypeModifiedSlot = core::com::newSlot(
        [&scale](double _scale)
        {
            scale = _scale;
        });
    m_worker = core::thread::Worker::New();
    crossTypeModifiedSlot->setWorker(m_worker);
    m_crossTypeAction->signal("crossTypeModified")->connect(crossTypeModifiedSlot);
    CPPUNIT_ASSERT_NO_THROW(m_crossTypeAction->configure());
    CPPUNIT_ASSERT_NO_THROW(m_crossTypeAction->start().get());

    CPPUNIT_ASSERT_NO_THROW(m_crossTypeAction->update().get());
    fwTestWaitMacro(expectedScale == scale);
    CPPUNIT_ASSERT_EQUAL(expectedScale, scale);
}

//------------------------------------------------------------------------------

void CrossTypeActionTest::fullTest()
{
    test("full", 1);
}

//------------------------------------------------------------------------------

void CrossTypeActionTest::halfTest()
{
    test("half", 0.5);
}

//------------------------------------------------------------------------------

void CrossTypeActionTest::hideTest()
{
    test("hide", 0);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::viz::ut

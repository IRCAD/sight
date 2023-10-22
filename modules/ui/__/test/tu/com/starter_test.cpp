/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

// cspell:ignore Doesnt

#include "starter_test.hpp"

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::com::ut::starter_test);

namespace sight::module::ui::com::ut
{

class DummyService : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(DummyService, service::base);

    int nbUpdate = 0;

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        nbUpdate++;
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }
};

SIGHT_REGISTER_SERVICE(service::base, DummyService);

//------------------------------------------------------------------------------

void starter_test::setUp()
{
    m_starter = service::add("sight::module::ui::com::starter");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::com::starter'", m_starter);
}

//------------------------------------------------------------------------------

void starter_test::tearDown()
{
    if(m_dummyService != nullptr)
    {
        if(!m_dummyService->stopped())
        {
            CPPUNIT_ASSERT_NO_THROW(m_dummyService->stop().get());
        }

        service::remove(m_dummyService);
        m_dummyService = nullptr;
    }

    if(!m_starter->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_starter->stop().get());
    }

    service::remove(m_starter);
}

//------------------------------------------------------------------------------

void starter_test::test(const std::string& _action_name, bool _exists, ShouldBe _should_be, int _nb_update)
{
    if(_exists)
    {
        service::base::sptr target_service = service::add("DummyService", "targetService");
        CPPUNIT_ASSERT_MESSAGE("Failed to create service 'DummyService'", target_service);
        m_dummyService = std::dynamic_pointer_cast<DummyService>(target_service);
        CPPUNIT_ASSERT_MESSAGE("Failed to cast service to 'DummyService'", m_dummyService);
        CPPUNIT_ASSERT_NO_THROW(m_dummyService->configure());
        if(_should_be == ShouldBe::STOPPED)
        {
            CPPUNIT_ASSERT_NO_THROW(m_dummyService->start().get());
        }
    }

    boost::property_tree::ptree ptree;
    ptree.put(_action_name + ".<xmlattr>.uid", "targetService");
    m_starter->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_starter->configure());
    CPPUNIT_ASSERT_NO_THROW(m_starter->start().get());
    CPPUNIT_ASSERT_NO_THROW(m_starter->update().get());

    if(_exists)
    {
        if(_should_be == ShouldBe::STARTED)
        {
            CPPUNIT_ASSERT(m_dummyService->started());
        }
        else if(_should_be == ShouldBe::STOPPED)
        {
            CPPUNIT_ASSERT(m_dummyService->stopped());
        }
        else
        {
            CPPUNIT_ASSERT(m_dummyService->started());
            CPPUNIT_ASSERT_EQUAL(_nb_update, m_dummyService->nbUpdate);
            CPPUNIT_ASSERT_NO_THROW(m_starter->update().get());
            CPPUNIT_ASSERT(m_dummyService->stopped());
        }

        CPPUNIT_ASSERT_EQUAL(_nb_update, m_dummyService->nbUpdate);
    }
}

//------------------------------------------------------------------------------

void starter_test::startTest()
{
    test("start", true, ShouldBe::STARTED, 1);
}

//------------------------------------------------------------------------------

void starter_test::startOnlyTest()
{
    test("start_only", true, ShouldBe::STARTED, 0);
}

//------------------------------------------------------------------------------

void starter_test::startIfExistsTest()
{
    test("start_if_exists", true, ShouldBe::STARTED, 1);
}

//------------------------------------------------------------------------------

void starter_test::startIfExistsButDoesntExistTest()
{
    test("start_if_exists", false);
}

//------------------------------------------------------------------------------

void starter_test::stopTest()
{
    test("stop", true, ShouldBe::STOPPED);
}

//------------------------------------------------------------------------------

void starter_test::stopIfExistsTest()
{
    test("stop_if_exists", true, ShouldBe::STOPPED);
}

//------------------------------------------------------------------------------

void starter_test::stopIfExistsButDoesntExistTest()
{
    test("stop_if_exists", false);
}

//------------------------------------------------------------------------------

void starter_test::startOrStopTest()
{
    test("start_or_stop", true, ShouldBe::STARTED_THEN_STOPPED, 1);
}

//------------------------------------------------------------------------------

void starter_test::startOnlyOrStopTest()
{
    test("start_only_or_stop", true, ShouldBe::STARTED_THEN_STOPPED, 0);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::com::ut

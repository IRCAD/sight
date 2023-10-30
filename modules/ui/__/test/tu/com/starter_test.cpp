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

    int nb_update = 0;

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
        nb_update++;
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
    if(m_dummy_service != nullptr)
    {
        if(!m_dummy_service->stopped())
        {
            CPPUNIT_ASSERT_NO_THROW(m_dummy_service->stop().get());
        }

        service::remove(m_dummy_service);
        m_dummy_service = nullptr;
    }

    if(!m_starter->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_starter->stop().get());
    }

    service::remove(m_starter);
}

//------------------------------------------------------------------------------

void starter_test::test(const std::string& _action_name, bool _exists, should_be _should_be, int _nb_update)
{
    if(_exists)
    {
        service::base::sptr target_service = service::add("DummyService", "targetService");
        CPPUNIT_ASSERT_MESSAGE("Failed to create service 'DummyService'", target_service);
        m_dummy_service = std::dynamic_pointer_cast<DummyService>(target_service);
        CPPUNIT_ASSERT_MESSAGE("Failed to cast service to 'DummyService'", m_dummy_service);
        CPPUNIT_ASSERT_NO_THROW(m_dummy_service->configure());
        if(_should_be == should_be::stopped)
        {
            CPPUNIT_ASSERT_NO_THROW(m_dummy_service->start().get());
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
        if(_should_be == should_be::started)
        {
            CPPUNIT_ASSERT(m_dummy_service->started());
        }
        else if(_should_be == should_be::stopped)
        {
            CPPUNIT_ASSERT(m_dummy_service->stopped());
        }
        else
        {
            CPPUNIT_ASSERT(m_dummy_service->started());
            CPPUNIT_ASSERT_EQUAL(_nb_update, m_dummy_service->nb_update);
            CPPUNIT_ASSERT_NO_THROW(m_starter->update().get());
            CPPUNIT_ASSERT(m_dummy_service->stopped());
        }

        CPPUNIT_ASSERT_EQUAL(_nb_update, m_dummy_service->nb_update);
    }
}

//------------------------------------------------------------------------------

void starter_test::start_test()
{
    test("start", true, should_be::started, 1);
}

//------------------------------------------------------------------------------

void starter_test::start_only_test()
{
    test("start_only", true, should_be::started, 0);
}

//------------------------------------------------------------------------------

void starter_test::start_if_exists_test()
{
    test("start_if_exists", true, should_be::started, 1);
}

//------------------------------------------------------------------------------

void starter_test::start_if_exists_but_doesnt_exist_test()
{
    test("start_if_exists", false);
}

//------------------------------------------------------------------------------

void starter_test::stop_test()
{
    test("stop", true, should_be::stopped);
}

//------------------------------------------------------------------------------

void starter_test::stop_if_exists_test()
{
    test("stop_if_exists", true, should_be::stopped);
}

//------------------------------------------------------------------------------

void starter_test::stop_if_exists_but_doesnt_exist_test()
{
    test("stop_if_exists", false);
}

//------------------------------------------------------------------------------

void starter_test::start_or_stop_test()
{
    test("start_or_stop", true, should_be::started_then_stopped, 1);
}

//------------------------------------------------------------------------------

void starter_test::start_only_or_stop_test()
{
    test("start_only_or_stop", true, should_be::started_then_stopped, 0);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::com::ut

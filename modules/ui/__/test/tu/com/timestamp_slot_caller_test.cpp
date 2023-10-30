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

#include "timestamp_slot_caller_test.hpp"

#include <core/com/slots.hxx>

#include <service/op.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::com::ut::timestamp_slot_caller_test);

namespace sight::module::ui::com::ut
{

class test_object : public sight::data::object,
                    public sight::core::com::has_slots
{
public:

    test_object();

    ~test_object() override;

    void slot(double _timestamp);

    const std::vector<double>& get_timestamps() const;

private:

    std::vector<double> m_timestamps;

    core::thread::worker::sptr m_worker = core::thread::worker::make();
};

test_object::test_object()
{
    auto slot = new_slot("slot", &test_object::slot, this);
    slot->set_worker(m_worker);
}

test_object::~test_object()
{
    m_worker->stop();
}

//------------------------------------------------------------------------------

void test_object::slot(double _timestamp)
{
    m_timestamps.push_back(_timestamp);
}

//------------------------------------------------------------------------------

const std::vector<double>& test_object::get_timestamps() const
{
    return m_timestamps;
}

//------------------------------------------------------------------------------

void timestamp_slot_caller_test::setUp()
{
    m_timestamp_slot_caller = service::add("sight::module::ui::com::timestamp_slot_caller");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::com::timestamp_slot_caller'",
        m_timestamp_slot_caller
    );
}

//------------------------------------------------------------------------------

void timestamp_slot_caller_test::tearDown()
{
    if(!m_timestamp_slot_caller->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_timestamp_slot_caller->stop().get());
    }

    service::remove(m_timestamp_slot_caller);
}

//------------------------------------------------------------------------------

void timestamp_slot_caller_test::basic_test()
{
    using namespace std::literals::chrono_literals;

    auto obj = std::make_shared<test_object>();
    obj->set_id("targetObject");

    boost::property_tree::ptree ptree;
    ptree.put("slots.slot", "targetObject/slot");
    m_timestamp_slot_caller->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_timestamp_slot_caller->configure());
    CPPUNIT_ASSERT_NO_THROW(m_timestamp_slot_caller->start().get());

    CPPUNIT_ASSERT_NO_THROW(m_timestamp_slot_caller->update().get());
    SIGHT_TEST_WAIT(1 == obj->get_timestamps().size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), obj->get_timestamps().size());

    std::this_thread::sleep_for(1000ms);

    CPPUNIT_ASSERT_NO_THROW(m_timestamp_slot_caller->update().get());
    SIGHT_TEST_WAIT(2 == obj->get_timestamps().size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->get_timestamps().size());

    CPPUNIT_ASSERT(obj->get_timestamps()[1] - obj->get_timestamps()[0] >= 1);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::com::ut

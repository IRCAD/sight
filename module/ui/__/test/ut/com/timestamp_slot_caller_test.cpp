/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <core/com/has_slots.hpp>
#include <core/com/slots.hpp>
#include <core/thread/worker.hpp>

#include <data/object.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <memory>
#include <thread>
#include <vector>

namespace
{

class test_object : public sight::data::object,
                    public sight::core::com::has_slots
{
public:

    test_object()
    {
        auto slot = new_slot("slot", &test_object::slot, this);
        slot->set_worker(m_worker);
    }

    ~test_object() override
    {
        m_worker->stop();
    }

    //------------------------------------------------------------------------------

    void slot(double _timestamp)
    {
        m_timestamps.push_back(_timestamp);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] const std::vector<double>& timestamps() const
    {
        return m_timestamps;
    }

private:

    std::vector<double> m_timestamps;

    sight::core::thread::worker::sptr m_worker = sight::core::thread::worker::make();
};

struct timestamp_slot_caller_fixture
{
    timestamp_slot_caller_fixture()
    {
        m_timestamp_slot_caller = sight::service::add("sight::module::ui::com::timestamp_slot_caller");
        REQUIRE_MESSAGE(
            m_timestamp_slot_caller,
            "Failed to create service 'sight::module::ui::com::timestamp_slot_caller'"
        );
    }

    ~timestamp_slot_caller_fixture()
    {
        if(!m_timestamp_slot_caller->stopped())
        {
            CHECK_NOTHROW(m_timestamp_slot_caller->stop().get());
        }

        sight::service::remove(m_timestamp_slot_caller);
    }

    timestamp_slot_caller_fixture(const timestamp_slot_caller_fixture&)            = delete;
    timestamp_slot_caller_fixture& operator=(const timestamp_slot_caller_fixture&) = delete;
    timestamp_slot_caller_fixture(timestamp_slot_caller_fixture&&)                 = delete;
    timestamp_slot_caller_fixture& operator=(timestamp_slot_caller_fixture&&)      = delete;

    sight::service::base::sptr m_timestamp_slot_caller;
};

} // namespace

TEST_SUITE("sight::module::ui::com::timestamp_slot_caller")
{
    TEST_CASE_FIXTURE(timestamp_slot_caller_fixture, "basic")
    {
        using namespace std::literals::chrono_literals;

        auto obj = std::make_shared<test_object>();
        obj->set_id("targetObject");

        boost::property_tree::ptree ptree;
        ptree.put("slots.slot", "targetObject/slot");
        m_timestamp_slot_caller->set_config(ptree);

        CHECK_NOTHROW(m_timestamp_slot_caller->configure());
        CHECK_NOTHROW(m_timestamp_slot_caller->start().get());

        CHECK_NOTHROW(m_timestamp_slot_caller->update().get());
        SIGHT_TEST_WAIT(1 == obj->timestamps().size());
        CHECK_EQ(std::size_t(1), obj->timestamps().size());

        std::this_thread::sleep_for(1000ms);

        CHECK_NOTHROW(m_timestamp_slot_caller->update().get());
        SIGHT_TEST_WAIT(2 == obj->timestamps().size());
        REQUIRE_EQ(std::size_t(2), obj->timestamps().size());

        CHECK(obj->timestamps()[1] - obj->timestamps()[0] >= 1);
    }
}

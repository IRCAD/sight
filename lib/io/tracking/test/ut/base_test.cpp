/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <io/tracking/base.hpp>

#include <doctest/doctest.h>

#include <memory>

namespace
{

/// Test implementation of tracking::base interface
class test_tracker final : public sight::io::tracking::base<sight::io::tracking::sensor_t>
{
public:

    SIGHT_DECLARE_SERVICE(test_tracker, sight::service::base);

    /// Constructor
    test_tracker() = default;

    /// Destructor
    ~test_tracker() final = default;

    /// Get the number of times tracking was called
    [[nodiscard]] std::size_t get_tracking_call_count() const
    {
        return m_tracking_call_count;
    }

    /// Public accessor to sensors
    using base::sensors;

protected:

    /// Implementation of tracking method for testing
    void tracking(sight::core::clock::type& /*_timestamp*/) final
    {
        m_tracking_call_count++;
    }

    /// Service starting method
    void starting() final
    {
    }

    /// Service stopping method
    void stopping() final
    {
    }

    /// Service updating method
    void updating() final
    {
    }

private:

    std::size_t m_tracking_call_count = 0;
};

} // namespace

TEST_SUITE("sight::io::tracking::base")
{
    TEST_CASE("is_tracking_and_set_tracking")
    {
        auto tracker = std::make_shared<test_tracker>();

        // Initially not tracking
        CHECK_FALSE(tracker->is_tracking());

        // Set tracking to true
        bool previous = tracker->set_tracking(true);
        CHECK_FALSE(previous);
        CHECK(tracker->is_tracking());

        // Set tracking to false
        previous = tracker->set_tracking(false);
        CHECK(previous);
        CHECK_FALSE(tracker->is_tracking());
    }

    TEST_CASE("enable_drop_and_is_dropping")
    {
        auto tracker = std::make_shared<test_tracker>();

        // By default, drop is enabled
        CHECK(tracker->is_dropping());

        // Disable drop
        tracker->enable_drop(false);
        CHECK_FALSE(tracker->is_dropping());

        // Enable drop again
        tracker->enable_drop(true);
        CHECK(tracker->is_dropping());
    }

    TEST_CASE("sensors_access")
    {
        auto tracker = std::make_shared<test_tracker>();

        // Initially no sensors
        CHECK_EQ(tracker->sensors().size(), 0);

        // Add sensors manually
        sight::io::tracking::sensor_t sensor1;
        sensor1.name     = "sensor_1";
        sensor1.optional = false;
        sensor1.tl_index = 0;
        tracker->sensors().push_back(sensor1);

        sight::io::tracking::sensor_t sensor2;
        sensor2.name     = "sensor_2";
        sensor2.optional = true;
        sensor2.tl_index = 1;
        tracker->sensors().push_back(sensor2);

        // Check sensors
        CHECK_EQ(tracker->sensors().size(), 2);
        CHECK_EQ(tracker->sensors()[0].name, "sensor_1");
        CHECK_FALSE(tracker->sensors()[0].optional);
        CHECK_EQ(tracker->sensors()[0].tl_index, 0);

        CHECK_EQ(tracker->sensors()[1].name, "sensor_2");
        CHECK(tracker->sensors()[1].optional);
        CHECK_EQ(tracker->sensors()[1].tl_index, 1);
    }

    TEST_CASE("sensor_t_to_string")
    {
        sight::io::tracking::sensor_t sensor;
        sensor.name     = "test_sensor";
        sensor.optional = true;
        sensor.tl_index = 5;

        std::string sensor_str = sensor.to_string();

        CHECK(sensor_str.find("test_sensor") != std::string::npos);
        CHECK(sensor_str.find('5') != std::string::npos);
    }

    TEST_CASE("multiple_sensors")
    {
        auto tracker = std::make_shared<test_tracker>();

        // Add multiple sensors
        for(std::size_t i = 0 ; i < 5 ; ++i)
        {
            sight::io::tracking::sensor_t sensor;
            sensor.name     = "sensor_" + std::to_string(i);
            sensor.optional = (i % 2) == 0;
            sensor.tl_index = i;
            tracker->sensors().push_back(sensor);
        }

        CHECK_EQ(tracker->sensors().size(), 5);

        // Verify all sensors
        for(std::size_t i = 0 ; i < 5 ; ++i)
        {
            CHECK_EQ(tracker->sensors()[i].name, "sensor_" + std::to_string(i));
            CHECK_EQ(tracker->sensors()[i].optional, (i % 2) == 0);
            CHECK_EQ(tracker->sensors()[i].tl_index, i);
        }
    }

    TEST_CASE("sensor_status")
    {
        sight::io::tracking::sensor_t sensor;

        // Initial status
        CHECK_EQ(sensor.current_tracking_quality, sight::io::tracking::status_t::untracked);
        CHECK_EQ(sensor.current_status, "");

        // Update status
        sensor.current_tracking_quality = sight::io::tracking::status_t::tracked;
        sensor.current_status           = "active";

        CHECK_EQ(sensor.current_tracking_quality, sight::io::tracking::status_t::tracked);
        CHECK_EQ(sensor.current_status, "active");
    }
} // TEST_SUITE

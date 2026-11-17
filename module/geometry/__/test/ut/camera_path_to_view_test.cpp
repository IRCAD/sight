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
 * Sight is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <data/fiducials_series.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/matrix4.hpp>
#include <data/real.hpp>

#include <core/thread/worker.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <array>
#include <atomic>
#include <cmath>

namespace
{

class context
{
public:

    context() :
        image(std::make_shared<sight::data::image_series>()),
        transform(std::make_shared<sight::data::matrix4>()),
        distance(std::make_shared<sight::data::real>(0.0)),
        speed(std::make_shared<sight::data::integer>(10)),
        direction(std::make_shared<sight::data::integer>(1))
    {
        srv = sight::service::add("sight::module::geometry::camera_path_to_view");
        REQUIRE(srv != nullptr);
        srv->set_input(image, "data.image");
        srv->set_inout(transform, "data.transform_out");
        srv->set_inout(distance, "config.distance");
        srv->set_input(speed, "config.speed_mm_s");
        srv->set_input(direction, "config.direction");
        srv->set_config(R"(<config path_group="Flight" />)");
        srv->configure();
        srv->start().get();
    }

    ~context()
    {
        if(srv->started())
        {
            srv->stop().get();
        }

        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    void add_point(const std::string& _group, double _x, double _y, double _z)
    {
        image->get_fiducials()->add_fiducial(
            [ = ](sight::data::fiducials_series::query_result& _result)
            {
                _result.m_contour_data = std::vector {_x, _y, _z};
                return true;
            },
            sight::data::fiducials_series::shape::point,
            _group
        );
        srv->update().get();
    }

    //------------------------------------------------------------------------------

    void set_distance(double _distance)
    {
        distance->set_value(_distance);
        srv->update().get();
    }

    sight::service::base::sptr srv;
    sight::data::image_series::sptr image;
    sight::data::matrix4::sptr transform;
    sight::data::real::sptr distance;
    sight::data::integer::sptr speed;
    sight::data::integer::sptr direction;
};

} // namespace

TEST_SUITE("sight::module::geometry::camera_path_to_view")
{
    TEST_CASE_FIXTURE(context, "playback_advances_the_camera_transform")
    {
        add_point("Flight", 0.0, 0.0, 0.0);
        add_point("Flight", 0.0, 0.0, 1000.0);

        std::atomic<bool> distance_changed {false};
        auto distance_slot = sight::core::com::new_slot(
            [&distance_changed]()
        {
            distance_changed.store(true);
        });
        distance_slot->set_worker(sight::core::thread::get_default_worker());
        const auto connection = distance->signal(sight::data::signals::MODIFIED)->connect(distance_slot);

        srv->slot("play")->run();
        SIGHT_TEST_WAIT(distance_changed.load());
        srv->slot("pause")->run();

        CHECK(distance->value() > 0.0);
        CHECK((*transform)(2, 3) == doctest::Approx(distance->value()));
    }

    TEST_CASE_FIXTURE(context, "path_group_and_distance")
    {
        add_point("Other", 100.0, 100.0, 100.0);
        add_point("Flight", 10.0, 20.0, 30.0);
        CHECK(transform->is_identity());

        add_point("Flight", 10.0, 20.0, 40.0);
        CHECK_EQ((*transform)(0, 3), 10.0);
        CHECK_EQ((*transform)(1, 3), 20.0);
        CHECK_EQ((*transform)(2, 3), 30.0);
        CHECK_EQ((*transform)(2, 2), 1.0);

        set_distance(5.0);
        CHECK_EQ((*transform)(2, 3), 35.0);

        add_point("Flight", 20.0, 20.0, 40.0);
        set_distance(15.0);
        CHECK_EQ((*transform)(0, 3), 15.0);
        CHECK_EQ((*transform)(2, 3), 40.0);
        CHECK((*transform)(0, 2) > 0.9);
        CHECK(sight::geometry::data::is_orthogonal(*transform));

        set_distance(25.0);
        CHECK_EQ((*transform)(0, 3), 10.0);
        CHECK_EQ((*transform)(1, 3), 20.0);
        CHECK_EQ((*transform)(2, 3), 35.0);
        CHECK((*transform)(2, 2) == doctest::Approx(std::sqrt(0.5)));
        CHECK(sight::geometry::data::is_homogeneous(*transform));

        set_distance(40.0);
        CHECK_EQ((*transform)(0, 3), 10.0);
        CHECK_EQ((*transform)(2, 3), 30.0);

        set_distance(45.0);
        CHECK_EQ((*transform)(0, 3), 10.0);
        CHECK_EQ((*transform)(2, 3), 35.0);
    }

    TEST_CASE_FIXTURE(context, "playback_uses_bound_speed_and_direction")
    {
        add_point("Flight", 0.0, 0.0, 0.0);
        add_point("Flight", 0.0, 0.0, 1000.0);

        speed->set_value(20);
        direction->set_value(0);

        std::atomic<bool> transform_changed {false};
        auto transform_slot = sight::core::com::new_slot(
            [&transform_changed, this]()
        {
            transform_changed.store((*transform)(2, 3) > 0.0);
        });
        transform_slot->set_worker(sight::core::thread::get_default_worker());
        const auto connection = transform->signal(sight::data::signals::MODIFIED)->connect(transform_slot);

        srv->slot("play")->run();
        SIGHT_TEST_WAIT(transform_changed.load());
        srv->slot("pause")->run();

        CHECK(distance->value() < 0.0);
        CHECK((*transform)(2, 3) > 0.0);
    }

    TEST_CASE_FIXTURE(context, "external_distance_change_updates_the_transform")
    {
        add_point("Flight", 0.0, 0.0, 0.0);
        add_point("Flight", 0.0, 0.0, 10.0);

        std::atomic<bool> transform_changed {false};
        auto transform_slot = sight::core::com::new_slot(
            [&transform_changed]()
        {
            transform_changed.store(true);
        });
        transform_slot->set_worker(sight::core::thread::get_default_worker());
        const auto connection = transform->signal(sight::data::signals::MODIFIED)->connect(transform_slot);

        distance->set_value(5.0);
        distance->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(transform_changed.load());

        CHECK_EQ((*transform)(2, 3), 5.0);
    }

    TEST_CASE_FIXTURE(context, "opposite_direction")
    {
        add_point("Flight", 0.0, 0.0, 0.0);
        add_point("Flight", 0.0, 0.0, -10.0);

        set_distance(5.0);
        CHECK_EQ((*transform)(2, 3), -5.0);
        CHECK_EQ((*transform)(2, 2), -1.0);
        CHECK(sight::geometry::data::is_orthogonal(*transform));
        CHECK(sight::geometry::data::is_homogeneous(*transform));
    }

    TEST_CASE_FIXTURE(context, "smooth_direction_at_bend")
    {
        add_point("Flight", 0.0, 0.0, 0.0);
        add_point("Flight", 0.0, 0.0, 10.0);
        add_point("Flight", 10.0, 0.0, 10.0);

        set_distance(5.0);
        const double before_turn_x = (*transform)(0, 2);
        CHECK_EQ((*transform)(0, 3), 0.0);
        CHECK_EQ((*transform)(2, 3), 5.0);
        CHECK(before_turn_x == doctest::Approx(std::sqrt(0.5)));

        set_distance(10.0);
        const double at_turn_x = (*transform)(0, 2);
        CHECK(at_turn_x == doctest::Approx(1.0));
        CHECK((*transform)(2, 2) == doctest::Approx(0.0));
        CHECK(sight::geometry::data::is_orthogonal(*transform));

        set_distance(12.5);
        CHECK_EQ((*transform)(0, 3), 2.5);
        CHECK_EQ((*transform)(2, 3), 10.0);
        CHECK((*transform)(0, 2) == doctest::Approx(at_turn_x));
        CHECK(before_turn_x < at_turn_x);
    }

    TEST_CASE_FIXTURE(context, "no_roll_flip_at_reverse_bend")
    {
        add_point("Flight", 0.0, 0.0, 0.0);
        add_point("Flight", 0.0, 0.0, 10.0);
        add_point("Flight", 0.0, 10.0, 10.0);
        add_point("Flight", 0.0, 10.0, 0.0);

        set_distance(24.9);
        const std::array<double, 3> previous_up {
            (*transform)(0, 1), (*transform)(1, 1), (*transform)(2, 1)
        };

        set_distance(25.0);
        const double up_alignment = previous_up[0] * (*transform)(0, 1)
                                    + previous_up[1] * (*transform)(1, 1)
                                    + previous_up[2] * (*transform)(2, 1);
        CHECK(up_alignment > 0.99);
        CHECK((*transform)(1, 1) < -0.9);
        CHECK((*transform)(2, 2) < -0.9);
        CHECK(sight::geometry::data::is_orthogonal(*transform));

        set_distance(55.0);
        CHECK((*transform)(1, 1) < -0.9);
    }
}

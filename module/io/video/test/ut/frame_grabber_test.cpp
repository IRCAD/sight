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

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/os/temp_path.hpp>
#include <core/runtime/runtime.hpp>
#include <core/thread/worker.hpp>

#include <data/boolean.hpp>
#include <data/camera.hpp>
#include <data/frame_tl.hpp>
#include <data/mt/locked_ptr.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <utest/wait.hpp>

#include <array>
#include <atomic>

namespace
{

class fixture
{
public:

    fixture()
    {
        sight::core::runtime::init();
        sight::core::runtime::load_module("sight::module::io::video");
        srv = sight::service::add("sight::module::io::video::frame_grabber");
        srv->set_input(camera, "config.camera", false);
        srv->set_inout(timeline, "timeline.image", false);
        camera->set_camera_source(sight::data::camera::file);
        position_slot = sight::core::com::new_slot([this](std::int64_t){++positions;});
        position_slot->set_worker(sight::core::thread::get_default_worker());
        position_connection = srv->signal("position_modified")->connect(position_slot);
        pushed_slot         = sight::core::com::new_slot([this](sight::core::clock::type){++pushed;});
        pushed_slot->set_worker(sight::core::thread::get_default_worker());
        pushed_connection = timeline->signal(sight::data::timeline::signals::PUSHED)->connect(pushed_slot);
    }

    ~fixture()
    {
        if(srv->started())
        {
            srv->stop().get();
        }

        sight::core::thread::get_default_worker()->post_task<void>([]{}).get();
        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    void start(const std::string& _config)
    {
        srv->set_config(_config);
        srv->configure();
        srv->start().get();
        srv->slot("start_camera")->run();
    }

    //------------------------------------------------------------------------------

    void make_images()
    {
        const std::array values {42, 42, 50, 50, 42, 42, 42, 42};
        for(int i = 0 ; i < 8 ; ++i)
        {
            const auto value = values[static_cast<std::size_t>(i)];
            REQUIRE(
                cv::imwrite(
                    (directory / ("image_" + std::to_string(i + 1) + ".png")).string(),
                    cv::Mat(16, 16, CV_8UC3, cv::Scalar(value, value + 1, value + 2))
                )
            );
        }

        camera->set_video_file(directory / "image_1.png");
    }

    //------------------------------------------------------------------------------

    void next(std::size_t _expected_positions)
    {
        srv->slot("next_image")->run();
        SIGHT_TEST_FAIL_WAIT(positions >= _expected_positions);
        // The position signal precedes insertion; wait for the grabber to release the timeline.
        {
            const sight::data::mt::locked_ptr<sight::data::frame_tl> lock(timeline);
        }
        sight::core::thread::get_default_worker()->post_task<void>([]{}).get();
    }

    sight::core::os::temp_dir directory;
    sight::service::base::sptr srv;
    sight::data::camera::sptr camera {std::make_shared<sight::data::camera>()};
    sight::data::frame_tl::sptr timeline {std::make_shared<sight::data::frame_tl>()};
    std::atomic_size_t positions {0};
    std::atomic_size_t pushed {0};
    sight::core::com::slot<void(std::int64_t)>::sptr position_slot;
    sight::core::com::slot<void(sight::core::clock::type)>::sptr pushed_slot;
    sight::core::com::connection position_connection;
    sight::core::com::connection pushed_connection;
};

} // namespace

TEST_SUITE("sight::module::io::video::frame_grabber")
{
    TEST_CASE_FIXTURE(fixture, "rejects_duplicate_video_frames_before_timeline_insertion")
    {
        const auto file = directory / "duplicates.avi";
        cv::VideoWriter writer(file.string(), cv::VideoWriter::fourcc('F', 'F', 'V', '1'), 30., cv::Size(16, 16));
        REQUIRE(writer.isOpened());
        for(int i = 0 ; i < 120 ; ++i)
        {
            const auto value = i / 4;
            writer.write(cv::Mat(16, 16, CV_8UC3, cv::Scalar(value, value + 1, value + 2)));
        }

        writer.release();
        camera->set_video_file(file);
        start("<config reject_duplicates='true' profile_duplicates='true' />");
        SIGHT_TEST_FAIL_WAIT(positions >= 121);
        srv->slot("stop_camera")->run();
        sight::core::thread::get_default_worker()->post_task<void>([]{}).get();
        // stop_camera publishes one black frame when clearing the timeline.
        CHECK_EQ(pushed.load(), 31);
    }

    TEST_CASE_FIXTURE(fixture, "disabled_by_default")
    {
        make_images();
        start("<oneShot>true</oneShot><useTimelapse>false</useTimelapse>");
        SIGHT_TEST_FAIL_WAIT(pushed == 1);
        next(3);
        CHECK_EQ(pushed.load(), 2);
    }

    TEST_CASE_FIXTURE(fixture, "live_switch_and_restart")
    {
        make_images();
        const auto enabled = std::make_shared<sight::data::boolean>(true);
        srv->set_input(enabled, "config.reject_duplicates", false);
        start("<oneShot>true</oneShot><useTimelapse>false</useTimelapse>");
        SIGHT_TEST_FAIL_WAIT(pushed == 1);
        next(3);
        CHECK_EQ(pushed.load(), 1);
        enabled->value() = false;
        next(4);
        CHECK_EQ(pushed.load(), 2);
        enabled->value() = true;
        next(5);
        CHECK_EQ(pushed.load(), 2);
        next(6);
        CHECK_EQ(pushed.load(), 3);
        next(7);
        CHECK_EQ(pushed.load(), 3);
        srv->slot("stop_camera")->run();
        sight::core::thread::get_default_worker()->post_task<void>([]{}).get();
        const auto before_restart = pushed.load();
        srv->slot("start_camera")->run();
        SIGHT_TEST_FAIL_WAIT(pushed == before_restart + 1);
    }

    TEST_CASE_FIXTURE(fixture, "single_changed_byte_is_not_a_duplicate")
    {
        make_images();
        cv::Mat image = cv::imread((directory / "image_1.png").string());
        image.at<cv::Vec3b>(0, 0)[0] += 1;
        REQUIRE(cv::imwrite((directory / "image_2.png").string(), image));
        start("<config reject_duplicates='true' /><oneShot>true</oneShot><useTimelapse>false</useTimelapse>");
        SIGHT_TEST_FAIL_WAIT(pushed == 1);
        next(3);
        CHECK_EQ(pushed.load(), 2);
    }
}

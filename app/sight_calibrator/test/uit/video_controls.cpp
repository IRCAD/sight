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

#include "fixture.hpp"

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/label.hpp>
#include <ui/test/helper/video_controls.hpp>
#include <ui/test/tester.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

#include <QLabel>

namespace sight::sight_calibrator::uit
{

#ifndef _WIN32
//------------------------------------------------------------------------------

/// Waits for the position label to leave the given value, to prove the player moves before checking that it comes
/// back to that value.
static void wait_position_leaves(sight::ui::test::tester& _tester, const std::string& _position)
{
    const sight::ui::test::helper::selector label("videoSliderSrv/currentPosition");
    label.select(_tester);
    _tester.doubt<QLabel*>(
        "the position label leaves '" + _position + "'",
        [&_position](QLabel* _label)
        {
            return _label->text() != QString::fromStdString(_position);
        });
}
#endif

TEST_SUITE("sight_calibrator")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "video_controls")
    {
        namespace helper = sight::ui::test::helper;

        const std::filesystem::path video_path = utest_data::dir()
                                                 / "sight/ui/sight_calibrator/chessboard_calibration_test.mp4";

        const std::string failure_message = start(
            "video_controls",
            [&video_path](sight::ui::test::tester& _tester)
            {
                helper::button::push(_tester, "activityCreatorSrv/Calibration");

                helper::video_controls::load(_tester, "videoToolbarView", video_path);

                // The video lasts 5 seconds
                helper::label::exactly_match(_tester, "videoSliderSrv/totalDuration", "00:00:05");

                // On windows, the video player do not go to the end of the video, maybe due to a bug in the opencv
                // player.
                /// @todo Investigate the issue and remove the following line.
#ifndef _WIN32
                // Loading a file only enables the Start action: nothing plays, the position stays at the beginning.
                helper::label::exactly_match(_tester, "videoSliderSrv/currentPosition", "00:00:00");

                // The current position shouldn't move while the player is paused
                helper::video_controls::stop(_tester, "videoToolbarView");
                helper::video_controls::start(_tester, "videoToolbarView");
                QTest::qWait(1000);
                helper::video_controls::pause(_tester, "videoToolbarView");
                std::string current_position = helper::label::get(_tester, "videoSliderSrv/currentPosition");
                QTest::qWait(1000);
                helper::label::exactly_match(_tester, "videoSliderSrv/currentPosition", current_position);

                // When enabling loop, the player goes through the end and comes back to the current position.
                helper::video_controls::loop(_tester, "videoToolbarView");
                helper::video_controls::play(_tester, "videoToolbarView");
                wait_position_leaves(_tester, current_position);
                helper::label::exactly_match(_tester, "videoSliderSrv/currentPosition", current_position);
                helper::video_controls::stop(_tester, "videoToolbarView");
#endif
            },
            true
        );

        // Runs on the main thread, after start() has returned: the only doctest assertion for
        // this scenario. See sight::ui::test::base::start().
        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
} // TEST_SUITE

} // namespace sight::sight_calibrator::uit

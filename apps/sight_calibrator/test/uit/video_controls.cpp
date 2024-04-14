/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "video_controls.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/label.hpp>
#include <ui/test/helper/video_controls.hpp>
#include <ui/test/tester.hpp>

#include <utest_data/data.hpp>

#include <QLabel>

#include <array>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_calibrator::uit::video_controls);

namespace sight::sight_calibrator::uit
{

//------------------------------------------------------------------------------

std::filesystem::path video_controls::get_profile_path()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/sight_calibrator/profile.xml";
}

//------------------------------------------------------------------------------

void video_controls::test()
{
    namespace helper = sight::ui::test::helper;

    const std::filesystem::path video_path = utest_data::dir()
                                             / "sight/ui/sight_calibrator/chessboard_calibration_test.mp4";

    start(
        "video_controls",
        [&video_path](sight::ui::test::tester& _tester)
        {
            helper::button::push(_tester, "activityCreatorSrv/Calibration");

            helper::video_controls::load(_tester, "videoToolbarView", video_path);

            // The video lasts 5 seconds
            helper::label::exactly_match(_tester, "videoSliderSrv/totalDuration", "00:00:05");

            // The file should play automatically upon loading, after 5 seconds, the file reaches the end.
            QTest::qWait(5000);
            helper::label::exactly_match(_tester, "videoSliderSrv/currentPosition", "00:00:00");

            // The current position shouldn't move while the player is paused
            helper::video_controls::stop(_tester, "videoToolbarView");
            helper::video_controls::start(_tester, "videoToolbarView");
            QTest::qWait(1000);
            helper::video_controls::pause(_tester, "videoToolbarView");
            std::string current_position = helper::label::get(_tester, "videoSliderSrv/currentPosition");
            QTest::qWait(1000);
            helper::label::exactly_match(_tester, "videoSliderSrv/currentPosition", current_position);

            // When enabling loop, we should return to the current position after 5 seconds.
            helper::video_controls::loop(_tester, "videoToolbarView");
            helper::video_controls::play(_tester, "videoToolbarView");
            QTest::qWait(5000);
            helper::label::exactly_match(_tester, "videoSliderSrv/currentPosition", current_position);
        },
        true
    );
}

} // namespace sight::sight_calibrator::uit

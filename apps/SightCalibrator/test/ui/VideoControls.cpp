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

#include "VideoControls.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Label.hpp>
#include <ui/testCore/helper/VideoControls.hpp>
#include <ui/testCore/Tester.hpp>

#include <utestData/Data.hpp>

#include <QLabel>

#include <array>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightcalibrator::test::ui::VideoControls);

namespace sight::sightcalibrator::test::ui
{

//------------------------------------------------------------------------------

std::filesystem::path VideoControls::getProfilePath()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/SightCalibrator/profile.xml";
}

//------------------------------------------------------------------------------

void VideoControls::test()
{
    namespace helper = sight::ui::testCore::helper;

    const std::filesystem::path videoPath = utestData::Data::dir()
                                            / "sight/ui/SightCalibrator/chessboard_calibration_test.mp4";

    start(
        "VideoControls",
        [&videoPath](sight::ui::testCore::Tester& tester)
        {
            helper::Button::push(tester, "activityCreatorSrv/Calibration");

            helper::VideoControls::load(tester, "videoToolbarView", videoPath);

            // The video lasts 5 seconds
            helper::Label::exactlyMatch(tester, "videoSliderSrv/totalDuration", "00:00:05");

            // The file should play automatically upon loading, after 5 seconds, the file reaches the end.
            QTest::qWait(5000);
            helper::Label::exactlyMatch(tester, "videoSliderSrv/currentPosition", "00:00:00");

            // The current position shouldn't move while the player is paused
            helper::VideoControls::stop(tester, "videoToolbarView");
            helper::VideoControls::start(tester, "videoToolbarView");
            QTest::qWait(1000);
            helper::VideoControls::pause(tester, "videoToolbarView");
            std::string currentPosition = helper::Label::get(tester, "videoSliderSrv/currentPosition");
            QTest::qWait(1000);
            helper::Label::exactlyMatch(tester, "videoSliderSrv/currentPosition", currentPosition);

            // When enabling loop, we should return to the current position after 5 seconds.
            helper::VideoControls::loop(tester, "videoToolbarView");
            helper::VideoControls::play(tester, "videoToolbarView");
            QTest::qWait(5000);
            helper::Label::exactlyMatch(tester, "videoSliderSrv/currentPosition", currentPosition);
        },
        true
    );
}

} // namespace sight::sightcalibrator::test::ui

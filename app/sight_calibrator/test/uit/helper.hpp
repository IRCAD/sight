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

#pragma once

#include <ui/test/locator.hpp>
#include <ui/test/scenario.hpp>

#include <utest_data/data.hpp>

#include <chrono> // NOLINT(misc-include-cleaner): the duration literals
#include <filesystem>
#include <string>
#include <string_view>

namespace sight::sight_calibrator::uit
{

/// An item of the toolbar of the video controls.
[[nodiscard]] inline std::string video(std::string_view _item)
{
    return "videoToolbarView/" + std::string(_item);
}

//------------------------------------------------------------------------------

/// The video of a chessboard, five seconds long.
[[nodiscard]] inline std::filesystem::path chessboard_video()
{
    return utest_data::dir() / "sight/ui/sight_calibrator/chessboard_calibration_test.mp4";
}

//------------------------------------------------------------------------------

/// Opens the calibration activity.
inline void open_calibration(ui::test::scenario& _ui)
{
    _ui.click("activityCreatorSrv/Calibration");
}

//------------------------------------------------------------------------------

/// Plays a video file through the camera selector, with the OpenCV player when the build offers several players.
inline void load_video(ui::test::scenario& _ui, const std::filesystem::path& _path)
{
    using namespace std::chrono_literals;
    _ui.select("videoSelectorSrv/device_selector", "File...");
    _ui.choose_file(_path);

    // Only a build with RealSense asks which player to use.
    if(const auto players = ui::test::item(
           ui::test::dialog() >> "selector/selection_list",
           "OpenCV player"
    );
       _ui.exists(players.view_expression(), 2s))
    {
        _ui.select(players);
        _ui.click(ui::test::dialog() >> "selector/ok");
    }
}

} // namespace sight::sight_calibrator::uit

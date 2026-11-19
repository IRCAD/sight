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

#include "helper.hpp"

#include <ui/test/gui_test.hpp>

#include <QLabel>

#include <chrono> // NOLINT(misc-include-cleaner): the duration literals
#include <string>

namespace sight::sight_calibrator::uit
{

SIGHT_GUI_TEST_CASE("video_controls")
{
    open_calibration(ui);
    load_video(ui, chessboard_video());
    ui.expect("videoSliderSrv/total_duration").text("00:00:05");

    using namespace std::chrono_literals;
    const std::string position = "videoSliderSrv/current_position";

    // Loading a file already starts the player: start it again, and let it play.
    ui.click(video("Stop"));
    ui.click(video("Start"));
    ui.expect(video("Start")).hidden();
    ui.expect(position).that<QLabel>("plays", [](const QLabel& _label){return _label.text() != "00:00:00";});

    // Paused, the position holds.
    ui.click(video("Pause"));
    ui.expect(video("Pause")).hidden();
    const auto paused = ui.read<QLabel>(position, [](const QLabel& _label){return _label.text().toStdString();});
    ui.hold(1s, "a paused video must not move");
    ui.expect(position).text(paused);

    // In a loop, the player goes through the end and comes back to where it was.
    ui.click(video("Loop"));
    ui.click(video("Play"));
    ui.expect(video("Play")).hidden();
    ui.expect(position).that<QLabel>(
        "leaves " + paused,
        [paused](const QLabel& _label){return _label.text().toStdString() != paused;});
    ui.expect(position).within(30s).text(paused);
    ui.click(video("Stop"));
}

} // namespace sight::sight_calibrator::uit

/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <string>

namespace sight::sight_calibrator::uit
{

SIGHT_GUI_TEST_CASE("intrinsic_calibration")
{
    const std::string captures = "calibrationInfoEditorSrv/nb_captures";
    const std::string list     = "calibrationInfoEditorSrv/captures_list";
    const std::string status   = "detectionStatusSrv/button";
    const std::string state    = "cameraInfoSrv/is_calibrated";

    open_calibration(ui);

    // The chessboard of the video has 10x8 squares of 20 mm.
    ui.click("toolBarView/Chessboard size");
    ui.set("chessboard_settings/board_width", 10);
    ui.set("chessboard_settings/board_height", 8);
    ui.set("chessboard_settings/board_square_size", 20.);
    ui.set("chessboard_settings/board_scale", 0.25);
    ui.close(ui::test::dialog());
    ui.expect("chessboard_settings/board_width").gone();

    // Without a chessboard, a capture adds nothing.
    ui.expect(status).tooltip("Red");
    ui.expect(state).text_contains("The camera is not calibrated.");
    for(int i = 0 ; i < 3 ; ++i)
    {
        ui.click("intrinsicCameraView/Add");
    }

    ui.expect(captures).text("0");
    ui.expect(list).rows(0);

    // With one, each capture is listed.
    load_video(ui, chessboard_video());
    ui.expect(status).tooltip("Green");
    for(int i = 1 ; i <= 4 ; ++i)
    {
        ui.click("intrinsicCameraView/Add");
        ui.expect(captures).text(std::to_string(i));
    }

    ui.expect(list).rows(4);

    ui.select(ui::test::item_at(list, "0"));
    ui.click("intrinsicCameraView/Remove");
    ui.expect(captures).text("3");
    ui.expect(list).rows(3);

    ui.click("intrinsicCameraView/Calibrate");
    ui.expect(state).text_contains("The camera is calibrated.");
}

} // namespace sight::sight_calibrator::uit

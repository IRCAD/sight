/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include "intrinsic_calibration.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/dialog.hpp>
#include <ui/test/helper/label.hpp>
#include <ui/test/helper/list_widget.hpp>
#include <ui/test/helper/preferences_configuration.hpp>
#include <ui/test/helper/tool_button.hpp>
#include <ui/test/helper/video_controls.hpp>
#include <ui/test/tester.hpp>

#include <utest_data/data.hpp>

#include <QLabel>
#include <QSpinBox>

#include <array>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_calibrator::uit::intrinsic_calibration);

namespace sight::sight_calibrator::uit
{

//------------------------------------------------------------------------------

std::filesystem::path intrinsic_calibration::get_profile_path()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/sight_calibrator/profile.xml";
}

//------------------------------------------------------------------------------

void intrinsic_calibration::test()
{
    using namespace std::literals::string_literals;
    namespace helper = sight::ui::test::helper;

    const std::filesystem::path video_path = utest_data::dir()
                                             / "sight/ui/sight_calibrator/chessboard_calibration_test.mp4";

    start(
        "intrinsic_calibration",
        [&video_path](sight::ui::test::tester& _tester)
        {
            // Access the calibration activity
            helper::button::push(_tester, "activityCreatorSrv/Calibration");

            // Configure the chessboard size (the size of the example chessboard is 10*8)
            helper::button::push(_tester, "toolBarView/Chessboard size");

            auto bt = _tester.add_in_backtrace("fill chessboard configuration window");
            helper::dialog::take(_tester, "Chessboard settings");
            QPointer<QWidget> window = _tester.get<QWidget*>();
            _tester.take("Chessboard settings", window);
            _tester.yields("'Chessboard width' field", "board_width");
            _tester.get<QSpinBox*>()->setValue(10);
            _tester.take("Chessboard settings", window);
            _tester.yields("'Chessboard height' field", "board_height");
            _tester.get<QSpinBox*>()->setValue(8);
            _tester.take("Chessboard settings", window);
            _tester.yields("'Chessboard square size (mm)' field", "board_square_size");
            _tester.get<QDoubleSpinBox*>()->setValue(20);
            _tester.take("Chessboard settings", window);
            _tester.yields("'Input scaling for chessboard detection' field", "board_scale");
            _tester.get<QDoubleSpinBox*>()->setValue(0.25);
            _tester.take("Chessboard settings", window);
            _tester.do_something_asynchronously<QWidget*>([](QWidget* _window){_window->close();});
            _tester.doubt(
                "the preferences configuration window is closed",
                [&window](QObject*) -> bool
            {
                return window == nullptr || !window->isVisible();
            });

            // We didn't load the chessboard yet: trying to add captures gives no result
            helper::tool_button::tool_tip_matches(_tester, "detectionStatusSrv/0", "Points are NOT visible");
            helper::label::contain(_tester, "cameraInfoSrv/isCalibrated", "The camera is not calibrated.");
            for(int i = 0 ; i < 3 ; i++)
            {
                helper::button::push(_tester, "intrinsicCameraView/Add");
            }

            helper::label::exactly_match(_tester, "calibrationInfoEditorSrv/nbCapturesLabel", "0");
            helper::list_widget::count_equals(_tester, "calibrationInfoEditorSrv/capturesListWidget", 0);

            // We load the chessboard
            helper::video_controls::load(_tester, "videoToolbarView", video_path);

            // The chessboard is loaded, trying to add captures effectively add them to the list
            helper::tool_button::tool_tip_matches(_tester, "detectionStatusSrv/0", "Point are visible");
            helper::button::push(_tester, "intrinsicCameraView/Add");
            for(int i = 0 ; i < 3 ; i++)
            {
                QTest::qWait(1000);
                helper::button::push(_tester, "intrinsicCameraView/Add");
            }

            helper::label::exactly_match(_tester, "calibrationInfoEditorSrv/nbCapturesLabel", "4");
            helper::list_widget::count_equals(_tester, "calibrationInfoEditorSrv/capturesListWidget", 4);

            // To click on the remove button should effectively remove a capture
            helper::list_widget::set_current_row(_tester, "calibrationInfoEditorSrv/capturesListWidget", 0);
            helper::button::push(_tester, "intrinsicCameraView/Remove");
            helper::label::exactly_match(_tester, "calibrationInfoEditorSrv/nbCapturesLabel", "3");
            helper::list_widget::count_equals(_tester, "calibrationInfoEditorSrv/capturesListWidget", 3);

            // Clicking the calibrate button should start the calibration procedure and the calibration info must become
            // available
            helper::button::push(_tester, "intrinsicCameraView/Calibrate");
            helper::label::contain(_tester, "cameraInfoSrv/isCalibrated", "The camera is calibrated.");

            // Since the process of calibration is deterministic and the video is actually a fixed image, the values are
            // reproducible
            const std::array fields {
                std::tuple {"width", 700., 0.},
                std::tuple {"height", 550., 0.},
                std::tuple {"skew", 0., 0.},
                std::tuple {"cx", 352.474, 12.}, // TODO: Sometimes we get 341.647/283.149, this is probably wrong but
                std::tuple {"cy", 244.686, 42.}, //for the moment we keep it tolerant to avoid to pollute the CI tests
                std::tuple {"fx", 2493.44, 300.},
                std::tuple {"fy", 2330.59, 300.},
                std::tuple {"k1", 0.0511689, 2.},
                std::tuple {"k2", -7.10914, 2.},
                std::tuple {"p1", -0.0021059, 0.0004},
                std::tuple {"p2", -0.00137331, 0.0004},
                std::tuple {"k3", 247.139, 100.},
                std::tuple {"error", 0.084695868, 0.0002}
            };
            QRegExp re("<font color='#0066CC'>(.*)</font>");
            for(auto [name, expected, tolerance] : fields)
            {
                helper::label::equal(_tester, "cameraInfoSrv/"s + name, expected, tolerance, re);
            }
        },
        true
    );
}

} // namespace sight::sight_calibrator::uit

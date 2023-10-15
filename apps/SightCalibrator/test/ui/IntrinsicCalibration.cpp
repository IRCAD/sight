/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "IntrinsicCalibration.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Label.hpp>
#include <ui/testCore/helper/ListWidget.hpp>
#include <ui/testCore/helper/PreferencesConfiguration.hpp>
#include <ui/testCore/helper/VideoControls.hpp>
#include <ui/testCore/Tester.hpp>

#include <utest_data/Data.hpp>

#include <QLabel>

#include <array>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightcalibrator::test::ui::IntrinsicCalibration);

namespace sight::sightcalibrator::test::ui
{

//------------------------------------------------------------------------------

std::filesystem::path IntrinsicCalibration::getProfilePath()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/SightCalibrator/profile.xml";
}

//------------------------------------------------------------------------------

void IntrinsicCalibration::test()
{
    using namespace std::literals::string_literals;
    namespace helper = sight::ui::testCore::helper;

    const std::filesystem::path videoPath = utest_data::Data::dir()
                                            / "sight/ui/SightCalibrator/chessboard_calibration_test.mp4";

    start(
        "IntrinsicCalibration",
        [&videoPath](sight::ui::testCore::Tester& tester)
        {
            // Access the calibration activity
            helper::Button::push(tester, "activityCreatorSrv/Calibration");

            // Configure the chessboard size (the size of the example chessboard is 10*8)
            helper::Button::push(tester, "toolBarView/Chessboard size");
            helper::PreferencesConfiguration::fill(tester, {{"CHESSBOARD_WIDTH", "10"}, {"CHESSBOARD_HEIGHT", "8"}});

            // We didn't load the chessboard yet: trying to add captures gives no result
            helper::Label::toolTipMatches(tester, "detectionStatusSrv/0", "Points are NOT visible");
            helper::Label::contain(tester, "cameraInfoSrv/isCalibrated", "The camera is not calibrated.");
            for(int i = 0 ; i < 3 ; i++)
            {
                helper::Button::push(tester, "intrinsicCameraView/Add");
            }

            helper::Label::exactlyMatch(tester, "calibrationInfoEditorSrv/nbCapturesLabel", "0");
            helper::ListWidget::countEquals(tester, "calibrationInfoEditorSrv/capturesListWidget", 0);

            // We load the chessboard
            helper::VideoControls::load(tester, "videoToolbarView", videoPath);

            // The chessboard is loaded, trying to add captures effectively add them to the list
            helper::Label::toolTipMatches(tester, "detectionStatusSrv/0", "Point are visible");
            helper::Button::push(tester, "intrinsicCameraView/Add");
            for(int i = 0 ; i < 3 ; i++)
            {
                QTest::qWait(1000);
                helper::Button::push(tester, "intrinsicCameraView/Add");
            }

            helper::Label::exactlyMatch(tester, "calibrationInfoEditorSrv/nbCapturesLabel", "4");
            helper::ListWidget::countEquals(tester, "calibrationInfoEditorSrv/capturesListWidget", 4);

            // To click on the remove button should effectively remove a capture
            helper::ListWidget::setCurrentRow(tester, "calibrationInfoEditorSrv/capturesListWidget", 0);
            helper::Button::push(tester, "intrinsicCameraView/Remove");
            helper::Label::exactlyMatch(tester, "calibrationInfoEditorSrv/nbCapturesLabel", "3");
            helper::ListWidget::countEquals(tester, "calibrationInfoEditorSrv/capturesListWidget", 3);

            // Clicking the calibrate button should start the calibration procedure and the calibration info must become
            // available
            helper::Button::push(tester, "intrinsicCameraView/Calibrate");
            helper::Label::contain(tester, "cameraInfoSrv/isCalibrated", "The camera is calibrated.");
            tester.take("reprojection error label", "errorLabelSrv");
            tester.doubt<QLabel*>(
                "the reprojection error is not empty and is a positive number",
                [](QLabel* obj) -> bool {return !obj->text().isEmpty() && obj->text().toInt() >= 0;});

            // Since the process of calibration is deterministic and the video is actually a fixed image, the values are
            // reproducible
            const std::array fields {
                std::tuple {"width", 700., 0.},
                std::tuple {"height", 550., 0.},
                std::tuple {"skew", 0., 0.},
                std::tuple {"cx", 352.474, 2.},
                std::tuple {"cy", 244.686, 2.},
                std::tuple {"fx", 2493.44, 300.},
                std::tuple {"fy", 2330.59, 300.},
                std::tuple {"k1", 0.0511689, 2.},
                std::tuple {"k2", -7.10914, 2.},
                std::tuple {"p1", -0.0021059, 0.0004},
                std::tuple {"p2", -0.00137331, 0.0004},
                std::tuple {"k3", 247.139, 100.}
            };
            QRegExp re("<font color='#0066CC'>(.*)</font>");
            for(auto [name, expected, tolerance] : fields)
            {
                helper::Label::equal(tester, "cameraInfoSrv/"s + name, expected, tolerance, re);
            }

            helper::Label::equal(tester, "errorLabelSrv", 0.084695868, 0.0002);
        },
        true
    );
}

} // namespace sight::sightcalibrator::test::ui

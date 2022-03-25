/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include <core/runtime/operations.hpp>

#include <utestData/Data.hpp>

#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QToolButton>

#include <ui/test/Tester.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightcalibrator::test::ui::IntrinsicCalibration);

namespace sight::sightcalibrator::test::ui
{

//------------------------------------------------------------------------------

const char* IntrinsicCalibration::getProfilePath()
{
    return "share/sight/SightCalibrator/profile.xml";
}

//------------------------------------------------------------------------------

void IntrinsicCalibration::test()
{
    const std::filesystem::path videoPath = utestData::Data::dir()
                                            / "sight/ui/SightCalibrator/chessboard_calibration_test.mp4";

    sight::ui::test::Tester tester("IntrinsicCalibration", true);
    start(
        "IntrinsicCalibration",
        [&videoPath](sight::ui::test::Tester& tester)
        {
            // Access the calibration activity
            tester.take(
                "Calibration button",
                [&tester]() -> QObject*
            {
                return tester.getMainWindow()->findChild<QPushButton*>("activityCreatorSrv/Calibration");
            });
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());

            // Configure the chessboard size (the size of the example chessboard is 10*8)
            tester.yields(
                "chessboard size button",
                [&tester](QObject*) -> QObject*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Chessboard size"
                    )
                );
            });
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.yields("chessboard size dialog", [](QObject*) -> QObject* {return qApp->activeModalWidget();});
            const QWidget* chessboardSizeWindow = tester.get<QWidget*>();
            tester.yields(
                "chessboard width field",
                [](QObject* old) -> QObject*
            {
                return old->findChild<QLineEdit*>("chessboardSizeAct/CHESSBOARD_WIDTH");
            });
            tester.doSomething<QLineEdit*>([](QLineEdit* obj){return obj->setText("10");});
            tester.take(
                "chessboard height field",
                [chessboardSizeWindow]() -> QObject*
            {
                return chessboardSizeWindow->findChild<QLineEdit*>("chessboardSizeAct/CHESSBOARD_HEIGHT");
            });
            tester.doSomething<QLineEdit*>([](QLineEdit* obj){return obj->setText("8");});
            tester.take(
                "chessboard size dialog ok button",
                [chessboardSizeWindow]() -> QObject*
            {
                return chessboardSizeWindow->findChild<QPushButton*>("chessboardSizeAct/OK");
            });
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.doubt(
                "the chessboard size dialog is closed",
                [chessboardSizeWindow](QObject*) -> bool
            {
                return !chessboardSizeWindow->isVisible();
            });

            // We didn't load the chessboard yet: trying to add captures gives no result
            tester.take(
                "calibration status square",
                [&tester]() -> QObject*
            {
                return tester.getMainWindow()->findChild<QLabel*>("detectionStatusSrv/0");
            });
            QLabel* calibrationStatusSquare = tester.get<QLabel*>();
            tester.doubt<QLabel*>(
                "the calibration status square says that points are not visible",
                [](QLabel* obj) -> bool {return obj->toolTip() == "Points are NOT visible";});
            tester.take(
                "camera is calibrated label",
                [&tester]() -> QObject*
            {
                return tester.getMainWindow()->findChild<QLabel*>("cameraInfoSrv/isCalibrated");
            });
            const QLabel* cameraIsCalibratedLabel = tester.get<QLabel*>();
            tester.doubt<QLabel*>(
                "the label says the camera isn't calibrated",
                [](QLabel* label) -> bool
            {
                return label->text().contains("The camera is not calibrated.");
            });
            tester.take(
                "add capture button",
                [&tester]() -> QObject*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "intrinsicCameraView/Add"
                    )
                );
            });
            QToolButton* addCaptureButton = tester.get<QToolButton*>();
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.take(
                "nb captures label",
                [&tester]() -> QObject*
            {
                return tester.getMainWindow()->findChild<QLabel*>("calibrationInfoEditorSrv/nbCapturesLabel");
            });
            const QLabel* nbCapturesLabel = tester.get<QLabel*>();
            tester.doubt<QLabel*>("nb captures label shows 0", [](QLabel* obj) -> bool {return obj->text() == "0";});
            tester.take(
                "nb captures list widget",
                [&tester]() -> QObject*
            {
                return tester.getMainWindow()->findChild<QListWidget*>("calibrationInfoEditorSrv/capturesListWidget");
            });
            QListWidget* capturesListWidget = tester.get<QListWidget*>();
            tester.doubt<QListWidget*>(
                "nb captures list is empty",
                [](QListWidget* obj) -> bool
            {
                return obj->count() == 0;
            });

            // We load the chessboard
            tester.yields(
                "camera selector combo box",
                [&tester](QObject*) -> QObject*
            {
                return tester.getMainWindow()->findChild<QComboBox*>("videoSelectorSrv");
            });
            tester.doSomething<QComboBox*>([](QComboBox* obj){obj->setCurrentText("File...");});
            tester.yields("choose video file dialog", [](QObject*) -> QObject* {return qApp->activeModalWidget();});
            const QWidget* fileWindow = tester.get<QWidget*>();
            tester.yields(
                "file name field",
                [](QObject* old) -> QObject*
            {
                return old->findChild<QLineEdit*>("fileNameEdit");
            });
            tester.doSomething<QLineEdit*>(
                [&videoPath](QLineEdit* obj)
            {
                obj->setText(QString::fromStdString(videoPath.string()));
            });
            tester.interact(std::make_unique<sight::ui::test::KeyboardClick>(Qt::Key_Enter));
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.doubt(
                "the file window is closed",
                [fileWindow](
                    QObject*) -> bool {return !fileWindow->isVisible();});

            // If RealSense is activated, another dialog appears
            tester.maybeTake(
                "choose video grabber implementation dialog",
                []() -> QObject*
            {
                return qApp->activeModalWidget();
            });
            QWidget* videoGrabberImplementationDialog = tester.get<QWidget*>();
            if(videoGrabberImplementationDialog != nullptr)
            {
                tester.yields(
                    "video grabber implementation list",
                    [](QObject* old) -> QObject*
                {
                    return old->findChild<QListWidget*>();
                });
                tester.doSomethingAsynchronously<QListWidget*>(
                    [](QListWidget* obj)
                {
                    obj->setCurrentItem(obj->findItems("OpenCV player", Qt::MatchExactly)[0]);
                });
                tester.doubt<QListWidget*>(
                    "the video grabber implementation is selected",
                    [](QListWidget* obj) -> bool
                {
                    return obj->currentItem()->text() == "OpenCV player";
                });
                tester.yields(
                    "choose video grabber implementation dialog",
                    [videoGrabberImplementationDialog](
                        QObject*) -> QObject* {return videoGrabberImplementationDialog;});
                tester.yields("ok button", [](QObject* old) -> QObject* {return old->findChildren<QPushButton*>()[0];});
                tester.interact(std::make_unique<sight::ui::test::MouseClick>());
                tester.doubt(
                    "the choose video grabber implementation dialog is closed",
                    [videoGrabberImplementationDialog](QObject*) -> bool
                {
                    return !videoGrabberImplementationDialog->isVisible();
                });
            }

            // The chessboard is loaded, trying to add captures effectively add them to the list
            tester.take("calibration status square", calibrationStatusSquare);
            tester.doubt<QLabel*>(
                "the calibration status square says that point are visible",
                [](QLabel* obj) -> bool
            {
                return obj->toolTip() == "Point are visible";
            });
            tester.take("add capture button", addCaptureButton);
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            for(int i = 0 ; i < 3 ; i++)
            {
                QTest::qWait(1000);
                tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            }

            tester.doubt(
                "nb captures label shows 4",
                [nbCapturesLabel](QObject*) -> bool
            {
                return nbCapturesLabel->text() == "4";
            });
            tester.doubt(
                "nb captures list has 4 elements",
                [capturesListWidget](QObject*) -> bool
            {
                return capturesListWidget->count() == 4;
            });

            // To click on the remove button should effectively remove a capture
            tester.take("nb captures list widget", capturesListWidget);
            tester.doSomethingAsynchronously<QListWidget*>([](QListWidget* obj){obj->setCurrentRow(0);});
            tester.take(
                "remove capture button",
                [&tester]() -> QObject*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "intrinsicCameraView/Remove"
                    )
                );
            });
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.doubt(
                "nb captures label shows 3",
                [nbCapturesLabel](QObject*) -> bool
            {
                return nbCapturesLabel->text() == "3";
            });
            tester.doubt(
                "nb captures list has 3 elements",
                [capturesListWidget](QObject*) -> bool
            {
                return capturesListWidget->count() == 3;
            });

            // Clicking the calibrate button should start the calibration procedure and the calibration info must become
            // available
            tester.take(
                "calibrate button",
                [&tester]() -> QObject*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "intrinsicCameraView/Calibrate"
                    )
                );
            });
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());
            tester.doubt(
                "the label says the camera is calibrated",
                [cameraIsCalibratedLabel](QObject*) -> bool
            {
                return cameraIsCalibratedLabel->text().contains("The camera is calibrated.");
            });
            tester.take(
                "reprojection error label",
                [&tester]() -> QObject*
            {
                return tester.getMainWindow()->findChild<QLabel*>("errorLabelSrv");
            });
            const QLabel* reprojectionErrorLabel = tester.get<QLabel*>();
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
            for(const auto& field : fields)
            {
                const std::string name = std::get<0>(field);
                tester.take(
                    name + " label",
                    [&tester, &name]() -> QObject*
                {
                    return tester.getMainWindow()->findChild<QLabel*>(QString::fromStdString("cameraInfoSrv/" + name));
                });
                tester.doubt<QLabel*>(
                    name + " is equal to " + std::to_string(std::get<1>(field)),
                    [&re, &field](QLabel* obj) -> bool
                {
                    obj->text().indexOf(re);
                    qDebug() << std::get<0>(field) << ": expected=" << std::get<1>(field) << "; actual="
                    << re.cap(1).toDouble();
                    return std::abs(re.cap(1).toDouble() - std::get<1>(field)) <= std::get<2>(field);
                });
            }

            tester.doubt(
                "reprojection error is equal to 0.084695868",
                [&reprojectionErrorLabel](QObject*) -> bool
            {
                qDebug() << "reprojection error : expected= 0.084695868 ; actual="
                << reprojectionErrorLabel->text().toDouble();
                return (reprojectionErrorLabel->text().toDouble() - 0.084695868) <= 0.0002;
            });
        },
        true
    );
}

} // namespace sight::sightcalibrator::test::ui

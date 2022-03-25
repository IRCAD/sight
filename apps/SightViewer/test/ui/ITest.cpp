/************************************************************************
 *
 * Copyright (C) 2022-2022 IRCAD France
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

#include "ITest.hpp"

#include <QAction>
#include <QFileDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSlider>

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

const char* ITest::getProfilePath()
{
    return "share/sight/SightViewer/profile.xml";
}

//------------------------------------------------------------------------------

void ITest::openFile(
    sight::ui::test::Tester& tester,
    const std::string& format,
    const std::filesystem::path& path
)
{
    /* Click on the "Load series" button */
    // First, we must get this button to click on it
    tester.take(
        "Load series button",
        [&tester]() -> QObject*
        {
            // We use the name of the component to get it
            QAction* action = tester.getMainWindow()->findChild<QAction*>("toolBarView/Load series");
            // It is actually an action, which is an "abstract" component we can't interact with, we must get an
            // interactable component
            return sight::ui::test::Tester::getWidgetFromAction(action);
        });
    // The, we can click on the button with the left button of the mouse
    tester.interact(std::make_unique<sight::ui::test::MouseClick>());
    // Once we clicked the button, a modal window should appear
    tester.yields<QWidget*>(
        "format selection window",
        [](QObject*) -> QWidget* {return qApp->activeModalWidget();},
        [](QWidget* obj) -> bool {return obj->isVisible();},
        sight::ui::test::Tester::DEFAULT_TIMEOUT*4
    );
    // We'll need that modal window later, let's keep a reference to it
    QWidget* formatSelectionWindow = tester.get<QWidget*>();

    /* Click on the FORMAT button of the file format selection window */
    // We must first get the list of format buttons, by searching in the modal window
    tester.yields(
        "format list",
        [](QObject* old) -> QObject* {return old->findChild<QListWidget*>();});
    // Click on the format we need using QListWidget::setCurrentItem. This method isn't thread-safe, so we must use
    // doSomethingAsynchronously
    tester.doSomethingAsynchronously<QListWidget*>(
        [&format](QListWidget* old) -> void
        {
            old->setCurrentItem(old->findItems(QString::fromStdString(format), Qt::MatchExactly)[0]);
        });
    // Check if the format we selected is the one we wanted
    tester.doubt<QListWidget*>(
        "the format is selected",
        [&format](QListWidget* obj) -> bool
        {
            return obj->currentItem()->text() == QString::fromStdString(format);
        });
    // We need the modal window again
    tester.take("format selection window", formatSelectionWindow);
    // We must then get the OK button...
    tester.yields(
        "ok button",
        [](QObject* obj) -> QObject* {return obj->findChildren<QPushButton*>()[0];});
    // ...and click on it
    tester.interact(std::make_unique<sight::ui::test::MouseClick>());

    /* Fill the file dialog, tap PATH */
    // Once we clicked the button, another modal window should appear
    tester.yields(
        "open " + format + " file window",
        [](QObject*) -> QObject* {return qobject_cast<QFileDialog*>(qApp->activeModalWidget());});
    // We need that window later
    const QWidget* fileWindow = tester.get<QWidget*>();
    // In that window, there is a text field to enter the filename
    tester.yields(
        "file name field",
        [](QObject* old) -> QObject* {return old->findChild<QLineEdit*>("fileNameEdit");});
    // Put the filename using QLineEdit::setText. This method is thread-safe, so we can use doSomething
    tester.doSomething<QLineEdit*>([&path](QLineEdit* obj){obj->setText(QString::fromStdString(path.string()));});
    // Press the Enter key to confirm
    tester.interact(std::make_unique<sight::ui::test::KeyboardClick>(Qt::Key_Enter));
    // The modal window should now be closed
    tester.doubt("the file window is closed", [fileWindow](QObject*) -> bool {return !fileWindow->isVisible();});

    if(format == "DICOM")
    {
        // When a DICOM files finish loading, a report appears as a dialog. We should close it before continuing.
        tester.take(
            "Reading process over window",
            []() -> QObject* {return qApp->activeModalWidget();},
            sight::ui::test::alwaysTrue,
            sight::ui::test::Tester::DEFAULT_TIMEOUT*4
        );
        tester.yields(
            "Ok button",
            [](QObject* old) -> QObject* {return old->findChild<QPushButton*>("Ok");});
        tester.interact(std::make_unique<sight::ui::test::MouseClick>());
        tester.doubt<QWidget*>("window is closed", [](QWidget* obj) -> bool {return !obj->isVisible();});

        // The Show/Hide volume TF editor button becomes enabled when the image effectively shows up.
        tester.take<QWidget*>(
            "Show/Hide volume TF editor button",
            [&tester]() -> QWidget*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Show/Hide volume TF editor"
                    )
                );
            },
            [](QWidget* obj) -> bool {return obj->isEnabled();},
            sight::ui::test::Tester::DEFAULT_TIMEOUT*2
        );
        QTest::qWait(10000);
    }
    else if(format == "SeriesDB Inrimage Reader (ITK)")
    {
        // The Show/Hide volume TF editor button becomes enabled when the image is loaded.
        tester.take<QWidget*>(
            "Show/Hide volume TF editor button",
            [&tester]() -> QWidget*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Show/Hide volume TF editor"
                    )
                );
            },
            [](QWidget* obj) -> bool {return obj->isEnabled();},
            sight::ui::test::Tester::DEFAULT_TIMEOUT*2
        );
    }
    else if(format == "VTK")
    {
        // The Show/hide mesh button becomes enabled when the image is loaded.
        tester.take<QWidget*>(
            "Show/hide mesh button",
            [&tester]() -> QWidget*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Show/hide mesh"
                    )
                );
            },
            [](QWidget* obj) -> bool {return obj->isEnabled();});
    }
}

//------------------------------------------------------------------------------

void ITest::saveSnapshot(sight::ui::test::Tester& tester, const std::filesystem::path& path)
{
    /* Click on the "snapshot" button */
    // First, we need the main window
    tester.take("main window", tester.getMainWindow());
    // Then, we must get that button
    tester.yields(
        "snapshot button",
        [](QObject* obj) -> QObject*
        {
            // We get the component using its name
            QAction* action = obj->findChild<QAction*>("topToolbarView/Snapshot");
            // This component is actually a QAction, we must get its associated button in order to interact with it.
            return sight::ui::test::Tester::getWidgetFromAction(action);
        });
    // We can finally click that button
    tester.interact(std::make_unique<sight::ui::test::MouseClick>());

    /* Fill the file dialog, tap PATH and press Enter. */
    // Once we clicked that button, a modal window should appear
    tester.yields(
        "save snapshot file window",
        [](QObject*) -> QObject* {return qApp->activeModalWidget();},
        sight::ui::test::alwaysTrue,
        sight::ui::test::Tester::DEFAULT_TIMEOUT*16
    );
    // In that window, there is a text field to enter the file name
    tester.yields(
        "file name field",
        [](QObject* old) -> QObject* {return old->findChild<QLineEdit*>("fileNameEdit");});
    // We can fill this text field using QLineEdit::setText
    tester.doSomething<QLineEdit*>([&path](QLineEdit* obj){obj->setText(QString::fromStdString(path.string()));});
    // We can then press Enter to confirm
    tester.interact(std::make_unique<sight::ui::test::KeyboardClick>(Qt::Key_Enter));
    // Once we have pressed Enter, the path must be created...
    tester.doubt(
        "the snapshot is saved",
        [&path](QObject*) -> bool {return std::filesystem::exists(path);},
        sight::ui::test::Tester::DEFAULT_TIMEOUT*2
    );
    // ...and the image should be valid.
    bool ok = QTest::qWaitFor([&path]() -> bool {return !QImage(QString::fromStdString(path.string())).isNull();});
    CPPUNIT_ASSERT_MESSAGE("The writer didn't finish writing", ok);
}

//------------------------------------------------------------------------------

void ITest::resetNegatos(sight::ui::test::Tester& tester)
{
    const std::array negatos {"axial", "frontal", "sagittal"};
    for(std::string negato : negatos)
    {
        tester.yields(
            negato + " negato slicer",
            [&tester, &negato](QObject*) -> QObject*
            {
                return tester.getMainWindow()->findChild<QObject*>(QString::fromStdString(negato + "NegatoSlicerSrv"));
            });
        QWidget* slicer = tester.get<QWidget*>();
        tester.yields<QSlider*>(
            negato + " negato slider",
            [](QObject* old) -> QSlider* {return old->findChild<QSlider*>();},
            [](QSlider* obj) -> bool {return obj->isEnabled();});
        tester.doSomething<QSlider*>(
            [](QSlider* obj)
            {
                obj->setValue(0);
            });

        tester.doubt(
            negato + " negato value is 0",
            [slicer](QObject*) -> bool
            {
                QRegExp re("(.*) / .*");
                slicer->findChild<QLineEdit*>()->text().indexOf(re);
                return re.cap(1) == "0";
            });
    }
}

} // namespace sight::sightviewer::test::ui

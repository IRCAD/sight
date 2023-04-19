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

#include "DoubleClickOnScene.hpp"

#include <QAction>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::DoubleClickOnScene);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void DoubleClickOnScene::test()
{
    start(
        "DoubleClickOnScene",
        [](sight::ui::testCore::Tester& tester)
        {
            // Initial situation: The negato view button is visible and the volume view button isn't
            tester.take(
                "Negato view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* obj){return obj->isVisible();});
            tester.take(
                "Volume view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* obj){return !obj->isVisible();});

            // Double click on the main scene
            tester.take(
                "ogre scene",
                [&tester]() -> QObject* {return tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseDoubleClick>());

            // Now, the negato view button should be visible and the volume view button shouldn't
            tester.take(
                "Negato view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* obj){return !obj->isVisible();});
            tester.take(
                "Volume view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* obj){return obj->isVisible();});

            // Double click again on the main scene
            tester.take(
                "ogre scene",
                [&tester]() -> QObject* {return tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseDoubleClick>());

            // We should return to the initial situation
            tester.take(
                "Negato view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* obj){return obj->isVisible();});
            tester.take(
                "Volume view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* obj){return !obj->isVisible();});

            // Simple click on the main scene
            tester.take(
                "ogre scene",
                [&tester]() -> QObject* {return tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseClick>());

            // It's a no-op; we should stay in the same situation
            tester.take(
                "Negato view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* obj){return obj->isVisible();});
            tester.take(
                "Volume view button",
                [&tester]
            {
                return sight::ui::testCore::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* obj){return !obj->isVisible();});
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui

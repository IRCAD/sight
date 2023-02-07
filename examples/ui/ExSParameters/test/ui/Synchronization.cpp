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

// cspell:ignore exsparameters

#include "Synchronization.hpp"

#include <core/runtime/path.hpp>

#include <ui/qt/widget/NonLinearSlider.hpp>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>

#include <ui/testCore/Tester.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::examples::ui::exsparameters::test::ui::Synchronization);

namespace sight::examples::ui::exsparameters::test::ui
{

//------------------------------------------------------------------------------

std::filesystem::path Synchronization::getProfilePath()
{
    const std::filesystem::path cwd = sight::core::runtime::getWorkingPath();
    return cwd / "share/sight/ExSParameters/profile.xml";
}

//------------------------------------------------------------------------------

void Synchronization::test()
{
    start(
        "Synchronization",
        [](sight::ui::testCore::Tester& tester)
        {
            tester.take(
                "parameters1Srv",
                [&tester]
            {
                return tester.getMainWindow()->findChild<QWidget*>("parameters1Srv");
            });
            auto* parameters1Srv = tester.get<QWidget*>();
            tester.take(
                "parameters2Srv",
                [&tester]
            {
                return tester.getMainWindow()->findChild<QWidget*>("parameters2Srv");
            });
            auto* parameters2Srv = tester.get<QWidget*>();

            // Check synchronization between "Enable" checkboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/enable", [](QObject* obj){return obj->findChild<QWidget*>("enable");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseClick>());
            tester.doubt<QCheckBox*>(
                "parameters1Srv/enable is unchecked",
                [](
                    QCheckBox* obj){return !obj->isChecked();});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/enable", [](QObject* obj){return obj->findChild<QWidget*>("enable");});
            tester.doubt<QCheckBox*>(
                "parameters2Srv/enable is unchecked",
                [](
                    QCheckBox* obj){return !obj->isChecked();});

            // Check that "Boolean" checkbox doesn't do anything because it is disabled
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/bool_1", [](QObject* obj){return obj->findChild<QWidget*>("bool_1");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseClick>());
            tester.doubt<QCheckBox*>(
                "parameters1Srv/bool_1 is still unchecked",
                [](QCheckBox* obj)
            {
                return !obj->isChecked();
            });

            // Re-enable the parameters by re-checking the "Enable" checkboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/enable", [](QObject* obj){return obj->findChild<QWidget*>("enable");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseClick>());
            tester.doubt<QCheckBox*>("parameters1Srv/enable is checked", [](QCheckBox* obj){return obj->isChecked();});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/enable", [](QObject* obj){return obj->findChild<QWidget*>("enable");});
            tester.doubt<QCheckBox*>("parameters2Srv/enable is checked", [](QCheckBox* obj){return obj->isChecked();});

            // Check synchronization between "Boolean" checkboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/bool_1", [](QObject* obj){return obj->findChild<QWidget*>("bool_1");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseClick>());
            tester.doubt<QCheckBox*>("parameters1Srv/bool_1 is checked", [](QCheckBox* obj){return obj->isChecked();});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/bool_1", [](QObject* obj){return obj->findChild<QWidget*>("bool_1");});
            tester.doubt<QCheckBox*>("parameters2Srv/bool_1 is checked", [](QCheckBox* obj){return obj->isChecked();});

            // Check synchronization between "Color" color selectors
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/color_1", [](QObject* obj){return obj->findChild<QWidget*>("color_1");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseClick>());
            tester.take("color select dialog", []{return qobject_cast<QColorDialog*>(qApp->activeModalWidget());});
            tester.doSomethingAsynchronously<QColorDialog*>(
                [](QColorDialog* obj)
            {
                obj->setCurrentColor({0, 0, 0});
                obj->accept();
            });
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/color_1", [](QObject* obj){return obj->findChild<QWidget*>("color_1");});
            tester.doubt<QPushButton*>(
                "parameters1Srv/color_1 selected color is black",
                [](QPushButton* obj)
            {
                return obj->icon().pixmap(1).toImage().pixelColor(0, 0) == QColor({0, 0, 0});
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/color_1", [](QObject* obj){return obj->findChild<QWidget*>("color_1");});
            tester.doubt<QPushButton*>(
                "parameters2Srv/color_1 selected color is black",
                [](QPushButton* obj)
            {
                return obj->icon().pixmap(1).toImage().pixelColor(0, 0) == QColor({0, 0, 0});
            });

            // Check synchronization between "Double 1" spinboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/double_1", [](QObject* obj){return obj->findChild<QWidget*>("double_1");});
            auto* double1 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(double1)
                    )
                );
            }

            tester.doubt<QDoubleSpinBox*>(
                "parameters1Srv/double_1 value is 0.40",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "0.40";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/double_1", [](QObject* obj){return obj->findChild<QWidget*>("double_1");});
            tester.doubt<QDoubleSpinBox*>(
                "parameters2Srv/double_1 value is 0.40",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "0.40";
            });

            // Check synchronization between "Double 2" sliders
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/double_2", [](QObject* obj){return obj->findChild<QWidget*>("double_2");});
            auto* double2 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(double2)
                    )
                );
            }

            tester.take("parameters1Srv", parameters1Srv);
            tester.yields(
                "parameters1Srv/double_2/valueLabel",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double_2/valueLabel");
            });
            tester.doubt<QLabel*>(
                "parameters1Srv/double_2 value is 1.00",
                [](QLabel* obj)
            {
                return obj->text() == "1.00";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields(
                "parameters2Srv/double_2/valueLabel",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double_2/valueLabel");
            });
            tester.doubt<QLabel*>(
                "parameters2Srv/double_2 value is 1.00",
                [](QLabel* obj)
            {
                return obj->text() == "1.00";
            });

            // Check synchronization between "2 Double" spinboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/double2_1", [](QObject* obj){return obj->findChild<QWidget*>("double2_1");});
            auto* double21 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(double21)
                    )
                );
            }

            tester.doubt<QDoubleSpinBox*>(
                "parameters1Srv/double2_1 value is 55.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "55.000";
            });
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields(
                "parameters1Srv/double2_1/1",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double2_1/1");
            });
            auto* double211 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::leftOf(double211)
                    )
                );
            }

            tester.doubt<QDoubleSpinBox*>(
                "parameters1Srv/double2_1/1 value is 45.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "45.000";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/double2_1", [](QObject* obj){return obj->findChild<QWidget*>("double2_1");});
            tester.doubt<QDoubleSpinBox*>(
                "parameters2Srv/double2_1 value is 55.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "55.000";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields(
                "parameters2Srv/double2_1/1",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double2_1/1");
            });
            tester.doubt<QDoubleSpinBox*>(
                "parameters2Srv/double2_1/1 value is 45.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "45.000";
            });

            // Check synchronization between "3 Double" spinboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/double3_1", [](QObject* obj){return obj->findChild<QWidget*>("double3_1");});
            auto* double31 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 4 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(double31)
                    )
                );
            }

            tester.doubt<QDoubleSpinBox*>(
                "parameters1Srv/double3_1 value is 30.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "29.000";
            });
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields(
                "parameters1Srv/double3_1/1",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double3_1/1");
            });
            auto* double311 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 4 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::leftOf(double311)
                    )
                );
            }

            tester.doubt<QDoubleSpinBox*>(
                "parameters1Srv/double3_1/1 value is 20.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "21.000";
            });
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields(
                "parameters1Srv/double3_1/2",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double3_1/2");
            });
            auto* double312 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 4 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(double312)
                    )
                );
            }

            tester.doubt<QDoubleSpinBox*>(
                "parameters1Srv/double3_1/2 value is 30.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "29.000";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/double3_1", [](QObject* obj){return obj->findChild<QWidget*>("double3_1");});
            tester.doubt<QDoubleSpinBox*>(
                "parameters2Srv/double3_1 value is 30.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "29.000";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields(
                "parameters2Srv/double3_1/1",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double3_1/1");
            });
            tester.doubt<QDoubleSpinBox*>(
                "parameters2Srv/double3_1/1 value is 20.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "21.000";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields(
                "parameters2Srv/double3_1/2",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("double3_1/2");
            });
            tester.doubt<QDoubleSpinBox*>(
                "parameters2Srv/double3_1/1 value is 30.000",
                [](QDoubleSpinBox* obj)
            {
                return obj->cleanText() == "29.000";
            });

            // Check synchronization between "Integer 1" spinboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/int_1", [](QObject* obj){return obj->findChild<QWidget*>("int_1");});
            auto* int1 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::leftOf(int1)
                    )
                );
            }

            tester.doubt<QSpinBox*>("parameters1Srv/int_1 value is 15", [](QSpinBox* obj){return obj->value() == 15;});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/int_1", [](QObject* obj){return obj->findChild<QWidget*>("int_1");});
            tester.doubt<QSpinBox*>("parameters2Srv/int_1 value is 15", [](QSpinBox* obj){return obj->value() == 15;});

            // Check synchronization between "Integer 2" sliders
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/int_2", [](QObject* obj){return obj->findChild<QWidget*>("int_2");});
            auto* int2 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 2 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::leftOf(int2)
                    )
                );
            }

            tester.take("parameters1Srv", parameters1Srv);
            tester.yields(
                "parameters1Srv/int_2/valueLabel",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("int_2/valueLabel");
            });
            tester.doubt<QLabel*>("parameters1Srv/int_2 value is 0", [](QLabel* obj){return obj->text() == "0";});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields(
                "parameters2Srv/int_2/valueLabel",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("int_2/valueLabel");
            });
            tester.doubt<QLabel*>("parameters2Srv/int_2 value is 0", [](QLabel* obj){return obj->text() == "0";});

            // Check synchronization between "2 Integer" spinboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/int2_1", [](QObject* obj){return obj->findChild<QWidget*>("int2_1");});
            auto* int21 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(int21)
                    )
                );
            }

            tester.doubt<QSpinBox*>("parameters1Srv/int2_1 value is 13", [](QSpinBox* obj){return obj->value() == 13;});
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/int2_1/1", [](QObject* obj){return obj->findChild<QWidget*>("int2_1/1");});
            auto* int211 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::leftOf(int211)
                    )
                );
            }

            tester.doubt<QSpinBox*>("parameters1Srv/int2_1/1 value is 3", [](QSpinBox* obj){return obj->value() == 3;});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/int2_1", [](QObject* obj){return obj->findChild<QWidget*>("int2_1");});
            tester.doubt<QSpinBox*>("parameters2Srv/int2_1 value is 13", [](QSpinBox* obj){return obj->value() == 13;});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/int2_1/1", [](QObject* obj){return obj->findChild<QWidget*>("int2_1/1");});
            tester.doubt<QSpinBox*>("parameters2Srv/int2_1/1 value is 3", [](QSpinBox* obj){return obj->value() == 3;});

            // Check synchronization between "3 Integer" spinboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/int3_1", [](QObject* obj){return obj->findChild<QWidget*>("int3_1");});
            auto* int31 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(int31)
                    )
                );
            }

            tester.doubt<QSpinBox*>("parameters1Srv/int3_1 value is 17", [](QSpinBox* obj){return obj->value() == 17;});
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/int3_1/1", [](QObject* obj){return obj->findChild<QWidget*>("int3_1/1");});
            auto* int311 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::leftOf(int311)
                    )
                );
            }

            tester.doubt<QSpinBox*>("parameters1Srv/int3_1/1 value is 7", [](QSpinBox* obj){return obj->value() == 7;});
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/int3_1/2", [](QObject* obj){return obj->findChild<QWidget*>("int3_1/2");});
            auto* int312 = tester.get<QWidget*>();
            for(std::uint8_t i = 0 ; i < 5 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::MouseButton::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::rightOf(int312)
                    )
                );
            }

            tester.doubt<QSpinBox*>(
                "parameters1Srv/int3_1/2 value is 17",
                [](
                    QSpinBox* obj){return obj->value() == 17;});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/int3_1", [](QObject* obj){return obj->findChild<QWidget*>("int3_1");});
            tester.doubt<QSpinBox*>("parameters2Srv/int3_1 value is 17", [](QSpinBox* obj){return obj->value() == 17;});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/int3_1/1", [](QObject* obj){return obj->findChild<QWidget*>("int3_1/1");});
            tester.doubt<QSpinBox*>("parameters2Srv/int3_1/1 value is 7", [](QSpinBox* obj){return obj->value() == 7;});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/int3_1/2", [](QObject* obj){return obj->findChild<QWidget*>("int3_1/2");});
            tester.doubt<QSpinBox*>(
                "parameters2Srv/int3_1/1 value is 17",
                [](
                    QSpinBox* obj){return obj->value() == 17;});

            // Check synchronization between "Enum" comboboxes
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/enum_1", [](QObject* obj){return obj->findChild<QWidget*>("enum_1");});
            tester.doSomething<QComboBox*>([](QComboBox* obj){return obj->setCurrentText("V1");});
            tester.doubt<QComboBox*>(
                "parameters1Srv/enum_1 value is V1",
                [](QComboBox* obj)
            {
                return obj->currentText() == "V1";
            });
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/enum_1", [](QObject* obj){return obj->findChild<QWidget*>("enum_1");});
            tester.doubt<QComboBox*>(
                "parameters2Srv/enum_1 value is V1",
                [](QComboBox* obj)
            {
                return obj->currentText() == "V1";
            });

            // Check synchronization from "Nonlinear" non-linear slider to combobox
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields("parameters1Srv/nonlin", [](QObject* obj){return obj->findChild<QWidget*>("nonlin");});
            tester.doSomething<sight::ui::qt::widget::NonLinearSlider*>(
                [](sight::ui::qt::widget::NonLinearSlider* obj)
            {
                obj->setValue(120);
            });
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields(
                "parameters1Srv/nonlin/valueLabel",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("nonlin/valueLabel");
            });
            tester.doubt<QLabel*>("parameters1Srv/nonlin value is 120", [](QLabel* obj){return obj->text() == "120";});
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/nonlin", [](QObject* obj){return obj->findChild<QWidget*>("nonlin");});
            tester.doubt<QComboBox*>(
                "parameters2Srv/nonlin value is 120",
                [](QComboBox* obj)
            {
                return obj->currentText() == "120";
            });

            // Check synchronization from "Nonlinear" combobox to non-linear slider
            tester.take("parameters2Srv", parameters2Srv);
            tester.yields("parameters2Srv/nonlin", [](QObject* obj){return obj->findChild<QWidget*>("nonlin");});
            tester.doSomething<QComboBox*>([](QComboBox* obj){obj->setCurrentText("150");});
            tester.doubt<QComboBox*>(
                "parameters2Srv/nonlin value is 150",
                [](QComboBox* obj)
            {
                return obj->currentText() == "150";
            });
            tester.take("parameters1Srv", parameters1Srv);
            tester.yields(
                "parameters1Srv/nonlin/valueLabel",
                [](QObject* obj)
            {
                return obj->findChild<QWidget*>("nonlin/valueLabel");
            });
            tester.doubt<QLabel*>("parameters1Srv/nonlin value is 150", [](QLabel* obj){return obj->text() == "150";});
        },
        true
    );
}

} // namespace sight::examples::ui::exsparameters::test::ui

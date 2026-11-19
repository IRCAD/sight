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

#include "../ut/gui_test_support.hpp"

#include <ui/test/detail/gui_dispatch.hpp>
#include <ui/test/detail/window_input.hpp>

#include <doctest/doctest.h>

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QString>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QWidgetAction>

#include <QtTest/qtestmouse.h>

#include <limits>
#include <stdexcept>
#include <string>

namespace sight::ui::test::detail
{

TEST_SUITE("sight::ui::test::window_input")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "routes_a_click_to_the_widget_at_the_window_position")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target_button");
        target.setGeometry(20, 20, 120, 60);
        window.show();

        int click_count = 0;
        QObject::connect(&target, &QPushButton::clicked, [&click_count]{++click_count;});
        dispatch_ack acknowledgement {};

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                if(!wait_for_exposure(*this, window, deadline))
                {
                    throw std::runtime_error("test window was not exposed");
                }

                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_click(window, target.mapTo(&window, target.rect().center()));});
                acknowledgement = dispatcher.acknowledge_until(ticket, deadline);
            });

        CHECK(acknowledgement == dispatch_ack::returned);
        CHECK(click_count == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_point_its_parent_clips")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        // A scroll area of sorts: the target's lower part lies below its parent's edge.
        QWidget clip(&window);
        clip.setGeometry(20, 20, 120, 40);
        press_counter target(&clip);
        target.setObjectName("clipped_target");
        target.setGeometry(0, 0, 120, 100);
        window.show();

        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_mouse_click(target, QPoint(60, 20));
                    failure = failure_of<input_failure>([&]{inject_mouse_click(target, QPoint(60, 80));});
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(target.presses == 1);
        CHECK(failure.find("'clipped_target' is not visible at") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_sibling_overlay_before_injecting")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target_button");
        target.setGeometry(20, 20, 120, 60);

        QWidget overlay(&window);
        overlay.setObjectName("notification_overlay");
        overlay.setGeometry(target.geometry());
        overlay.show();
        window.show();
        overlay.raise();

        int target_clicks = 0;
        QObject::connect(&target, &QPushButton::clicked, [&target_clicks]{++target_clicks;});
        std::string failure;

        try
        {
            run_scenario(
                *this,
                [&]
                {
                    const auto deadline = operation_deadline();
                    if(!wait_for_exposure(*this, window, deadline))
                    {
                        throw std::runtime_error("test window was not exposed");
                    }

                    const auto ticket = dispatcher.post_until(
                        deadline,
                        [&]{inject_mouse_click(target, target.rect().center());});
                    static_cast<void>(dispatcher.acknowledge_until(ticket, deadline));
                });
        }
        catch(const std::runtime_error& error)
        {
            failure = error.what();
        }

        CHECK(failure.find("notification_overlay") != std::string::npos);
        CHECK(target_clicks == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "clicks_a_covered_target_again_once_the_overlay_is_gone")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target_button");
        target.setGeometry(20, 20, 120, 60);

        QWidget overlay(&window);
        overlay.setObjectName("notification_overlay");
        overlay.setGeometry(target.geometry());
        overlay.show();
        window.show();
        overlay.raise();

        int target_clicks = 0;
        QObject::connect(&target, &QPushButton::clicked, [&target_clicks]{++target_clicks;});
        int attempts           = 0;
        bool failure_retryable = false;
        bool clicked           = false;

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                clicked = wait_for(
                    *this,
                    deadline,
                    [&]
                {
                    ++attempts;
                    try
                    {
                        inject_mouse_click(target, target.rect().center());
                        return true;
                    }
                    catch(const input_failure& failure)
                    {
                        if(!failure.retryable())
                        {
                            throw;
                        }

                        // The overlay leaves once an attempt failed on it, whatever the machine's speed.
                        failure_retryable = true;
                        overlay.hide();
                        return false;
                    }
                });
            });

        CHECK(clicked);
        CHECK(failure_retryable);
        CHECK(attempts == 2);
        CHECK(target_clicks == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "acknowledges_a_modal_opened_by_a_window_click")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        QPushButton target("open", &window);
        target.setObjectName("open_dialog");
        target.setGeometry(20, 20, 120, 60);

        QDialog dialog(&window);
        dialog.setObjectName("modal_dialog");
        dialog.setModal(true);
        QPushButton close_button("close", &dialog);
        close_button.setObjectName("close_dialog");
        close_button.setGeometry(20, 20, 120, 60);
        dialog.resize(180, 80);

        QObject::connect(&target, &QPushButton::clicked, [&dialog]{dialog.exec();});
        QObject::connect(&close_button, &QPushButton::clicked, [&dialog]{dialog.accept();});
        window.show();

        dispatch_ack modal_acknowledgement {};
        dispatch_ack close_acknowledgement {};
        bool click_returned = false;

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                if(!wait_for_exposure(*this, window, deadline))
                {
                    throw std::runtime_error("test window was not exposed");
                }

                const auto click = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_click(target, target.rect().center());});
                modal_acknowledgement = dispatcher.acknowledge_until(click, deadline);

                // Visible is not exposed: a loaded X server shows the dialog later.
                if(!wait_for_exposure(*this, dialog, deadline))
                {
                    throw std::runtime_error("modal dialog was not exposed");
                }

                const auto close = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_click(close_button, close_button.rect().center());});
                close_acknowledgement = dispatcher.acknowledge_until(close, deadline);
                dispatcher.wait_returned_until(click, deadline);
                click_returned = true;
            });

        CHECK(modal_acknowledgement == dispatch_ack::modal_opened);
        CHECK(close_acknowledgement == dispatch_ack::returned);
        CHECK(click_returned);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "keeps_two_clicks_at_the_same_place_two_clicks")
    {
        press_counter window;
        window.setObjectName("counter_window");
        window.resize(240, 120);
        window.show();

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                for(int click = 0 ; click < 2 ; ++click)
                {
                    const auto ticket = dispatcher.post_until(
                        deadline,
                        [&]{inject_mouse_click(window, window.rect().center());});
                    dispatcher.wait_returned_until(ticket, deadline);
                }
            });

        CHECK(window.presses == 2);
        CHECK(window.double_clicks == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "routes_a_double_click_to_the_target")
    {
        press_counter window;
        window.setObjectName("counter_window");
        window.resize(240, 120);
        window.show();

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_double_click(window, window.rect().center());});
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(window.presses == 1);
        CHECK(window.double_clicks == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "routes_a_drag_to_the_target_with_the_button_held")
    {
        drag_recorder window;
        window.setObjectName("drag_window");
        window.resize(240, 120);
        window.show();

        const QPoint from(30, 30);
        const QPoint to(180, 80);

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(deadline, [&]{inject_mouse_drag(window, from, to);});
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(window.m_presses == 1);
        CHECK(window.m_moves == 1);
        CHECK(window.m_releases == 1);
        CHECK(window.m_last_position == to);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "routes_wheel_notches_through_the_target_window")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        wheel_recorder target(&window);
        target.setObjectName("wheel_target");
        target.setGeometry(20, 20, 120, 60);
        window.show();

        CHECK_THROWS_AS(
            inject_mouse_wheel(target, target.rect().center(), 0),
            std::invalid_argument
        );
        CHECK_THROWS_AS(
            inject_mouse_wheel(target, target.rect().center(), 256),
            std::invalid_argument
        );
        CHECK_THROWS_AS(
            inject_mouse_wheel(target, target.rect().center(), -256),
            std::invalid_argument
        );
        CHECK_THROWS_AS(
            inject_mouse_wheel(target, target.rect().center(), std::numeric_limits<int>::min()),
            std::invalid_argument
        );

        dispatch_ack acknowledgement {};
        bool notches_were_synchronous = false;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    // One event per notch, as a mouse wheel sends them: widgets may count events, not angles.
                    inject_mouse_wheel(target, target.rect().center(), 255);
                    notches_were_synchronous = target.m_wheel_events == 255
                                               && target.m_angle_delta == QPoint(0, 120);
                    inject_mouse_wheel(target, target.rect().center(), -1);
                });
                acknowledgement = dispatcher.acknowledge_until(ticket, deadline);
            });

        CHECK(acknowledgement == dispatch_ack::returned);
        CHECK(notches_were_synchronous);
        CHECK(target.m_wheel_events == 256);
        CHECK(target.m_angle_delta == QPoint(0, -120));
        CHECK(target.m_phase == Qt::NoScrollPhase);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "propagates_a_wheel_ignored_by_a_child_to_the_target")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        wheel_recorder target(&window);
        target.setObjectName("wheel_target");
        target.setGeometry(20, 20, 120, 60);

        // QWidget ignores wheel events: a real wheel then reaches its parent, a non-spontaneous one does not.
        QWidget child(&target);
        child.setObjectName("ignoring_child");
        child.setGeometry(10, 10, 60, 30);
        window.show();

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_wheel(target, child.geometry().center(), 1);});
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(target.m_wheel_events == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "routes_a_key_to_the_focused_widget")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        key_recorder target(&window);
        target.setObjectName("key_target");
        target.setGeometry(20, 20, 120, 40);
        window.show();

        bool key_was_synchronous = false;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for_active_window(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_mouse_click(target, target.rect().center());
                    inject_key_click(target, Qt::Key_A, Qt::NoModifier, QStringLiteral("a"));
                    key_was_synchronous = target.m_key_presses == 1
                                          && target.m_key_releases == 1
                                          && target.m_last_key == Qt::Key_A
                                          && target.text() == QStringLiteral("a");
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(key_was_synchronous);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "gives_the_focus_back_after_another_window_took_it")
    {
        QWidget window;
        window.setObjectName("target_window");
        window.resize(240, 120);

        key_recorder target(&window);
        target.setObjectName("key_target");
        target.setGeometry(20, 20, 120, 40);

        QWidget other;
        other.setObjectName("other_window");
        other.setGeometry(300, 0, 240, 120);
        window.show();
        other.show();

        std::string inactive_failure;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for_exposure(*this, other, deadline));
                REQUIRE(retry_input(*this, deadline, [&]{give_focus(target);}));

                // Another window takes the focus, as a concurrent test or another application would.
                auto ticket = dispatcher.post_until(deadline, [&]{other.activateWindow();});
                dispatcher.wait_returned_until(ticket, deadline);
                REQUIRE(wait_for(*this, deadline, [&]{return QApplication::activeWindow() == &other;}));

                ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    try
                    {
                        inject_key_click(target, Qt::Key_A, Qt::NoModifier, QStringLiteral("a"));
                    }
                    catch(const input_failure& error)
                    {
                        inactive_failure = error.what();
                    }
                });
                dispatcher.wait_returned_until(ticket, deadline);

                REQUIRE(retry_input(*this, deadline, [&]{give_focus(target);}));
                ticket = dispatcher.post_until(
                    deadline,
                    [&]{inject_key_click(target, Qt::Key_A, Qt::NoModifier, QStringLiteral("a"));});
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(inactive_failure.find("target_window") != std::string::npos);
        CHECK(inactive_failure.find("is not active") != std::string::npos);
        CHECK(target.m_key_presses == 1);
        CHECK(target.m_key_releases == 1);
        CHECK(target.text() == QStringLiteral("a"));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_the_focus_to_a_widget_a_user_cannot_focus")
    {
        QWidget window;
        window.setObjectName("target_window");
        window.resize(240, 120);

        QWidget target(&window);
        target.setObjectName("plain_widget");
        target.setGeometry(20, 20, 120, 40);
        window.show();

        std::string failure;
        bool retryable = true;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    try
                    {
                        give_focus(target);
                    }
                    catch(const input_failure& error)
                    {
                        failure   = error.what();
                        retryable = error.retryable();
                    }
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(failure.find("'plain_widget' cannot take the focus") != std::string::npos);
        CHECK_FALSE(retryable);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_give_the_focus_through_another_windows_popup")
    {
        QWidget window;
        window.setObjectName("target_window");
        window.resize(240, 120);

        key_recorder target(&window);
        target.setObjectName("key_target");
        target.setGeometry(20, 20, 120, 40);
        window.show();

        QWidget other;
        other.setObjectName("other_window");
        other.setGeometry(300, 0, 240, 120);
        other.show();

        QMenu menu(&other);
        menu.setObjectName("open_menu");
        menu.addAction("item");

        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for_exposure(*this, other, deadline));
                auto ticket = dispatcher.post_until(deadline, [&]{other.activateWindow();});
                dispatcher.wait_returned_until(ticket, deadline);
                REQUIRE(wait_for(*this, deadline, [&]{return QApplication::activeWindow() == &other;}));

                ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    menu.popup(other.mapToGlobal(QPoint(20, 20)));
                    failure = failure_of<input_failure>([&]{give_focus(target);});

                    menu.close();
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(failure.find("keyboard target is blocked by popup 'open_menu'") != std::string::npos);
        CHECK(target.m_key_presses == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "delivers_a_key_bound_to_a_shortcut_to_the_shortcut")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        key_recorder target(&window);
        target.setObjectName("key_target");
        target.setGeometry(20, 20, 120, 40);

        // Qt's shortcut map consumes the key before the focus widget sees it, as for a user's key.
        QAction save("save", &window);
        save.setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
        window.addAction(&save);
        int saves = 0;
        QObject::connect(&save, &QAction::triggered, [&saves]{++saves;});
        window.show();

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for_active_window(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_mouse_click(target, target.rect().center());
                    inject_key_click(target, Qt::Key_S, Qt::ControlModifier);
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(saves == 1);
        CHECK(target.m_key_presses == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_key_while_a_popup_is_open")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        key_recorder target(&window);
        target.setObjectName("key_target");
        target.setGeometry(20, 20, 120, 40);
        window.show();

        QMenu menu(&window);
        menu.setObjectName("open_menu");
        menu.addAction("item");

        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_mouse_click(target, target.rect().center());
                    menu.popup(window.mapToGlobal(QPoint(150, 20)));
                    try
                    {
                        inject_key_click(target, Qt::Key_A, Qt::NoModifier, QStringLiteral("a"));
                    }
                    catch(const std::runtime_error& error)
                    {
                        failure = error.what();
                    }

                    menu.close();
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(failure.find("open_menu") != std::string::npos);
        CHECK(target.text().isEmpty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "names_the_popup_blocking_a_key_of_an_inactive_window")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.setGeometry(0, 0, 240, 120);

        key_recorder target(&window);
        target.setObjectName("key_target");
        target.setGeometry(20, 20, 120, 40);
        window.show();

        // Another window is active and its menu is open: the popup, not the inactive window, takes the key.
        QWidget other;
        other.setObjectName("other_window");
        other.setGeometry(300, 0, 240, 120);
        other.show();

        QMenu menu(&other);
        menu.setObjectName("open_menu");
        menu.addAction("item");

        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for_exposure(*this, other, deadline));
                auto ticket = dispatcher.post_until(deadline, [&]{other.activateWindow();});
                dispatcher.wait_returned_until(ticket, deadline);
                REQUIRE(wait_for(*this, deadline, [&]{return QApplication::activeWindow() == &other;}));

                ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    menu.popup(other.mapToGlobal(QPoint(20, 20)));
                    try
                    {
                        inject_key_click(target, Qt::Key_A, Qt::NoModifier, QStringLiteral("a"));
                    }
                    catch(const std::runtime_error& error)
                    {
                        failure = error.what();
                    }

                    menu.close();
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(failure.find("open_menu") != std::string::npos);
        CHECK(target.text().isEmpty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "routes_a_key_to_an_open_menu")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);
        window.show();

        QMenu menu(&window);
        menu.setObjectName("menu_target");
        QAction* const first_action = menu.addAction("first");
        menu.addAction("second");

        bool menu_received_key = false;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    menu.popup(window.mapToGlobal(QPoint(150, 20)));
                });
                dispatcher.wait_returned_until(ticket, deadline);

                REQUIRE(wait_for_exposure(*this, menu, deadline));
                ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_key_click(menu, Qt::Key_Down);
                    menu_received_key = menu.activeAction() == first_action;
                    menu.close();
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(menu_received_key);
    }

    TEST_CASE_FIXTURE(
        gui_dispatch_fixture,
        "selects_an_item_from_a_toolbar_extension"
    )
    {
        QMainWindow window;
        window.setObjectName("test_window");
        // Too low for the toolbar to expand, so that its extension opens a menu on every platform.
        window.setFixedSize(240, 60);

        QToolBar toolbar("test_toolbar", &window);
        toolbar.setObjectName("test_toolbar");
        toolbar.setToolButtonStyle(Qt::ToolButtonTextOnly);
        window.addToolBar(&toolbar);

        QAction* target_action = nullptr;
        for(int index = 0 ; index < 8 ; ++index)
        {
            target_action = toolbar.addAction(QStringLiteral("Toolbar action %1").arg(index));
        }

        auto* const extension = toolbar.findChild<QToolButton*>("qt_toolbar_ext_button");
        REQUIRE(extension != nullptr);

        bool target_action_triggered = false;
        QObject::connect(
            target_action,
            &QAction::triggered,
            [&target_action_triggered]
            {
                target_action_triggered = true;
            });
        window.show();

        dispatch_ack open_acknowledgement {};
        QMenu* overflow_menu = nullptr;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for(*this, deadline, [&]{return extension->isVisible();}));

                // The window is too small to expand the toolbar: the extension opens its menu with QMenu::exec().
                const auto open = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_click(*extension, extension->rect().center());});
                open_acknowledgement = dispatcher.acknowledge_until(open, deadline);
                REQUIRE(open_acknowledgement == dispatch_ack::popup_opened);

                // Windows fades a menu in, and clicks go through it until then: as a user does, the test waits
                // for the menu to be what lies under its item. A verb retries instead.
                REQUIRE(
                    wait_for(
                        *this,
                        deadline,
                        [&]
                {
                    overflow_menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
                    return overflow_menu != nullptr && overflow_menu->actions().contains(target_action)
                           && overflow_menu->windowHandle() != nullptr && overflow_menu->windowHandle()->isExposed()
                           && QApplication::topLevelAt(
                        overflow_menu->mapToGlobal(overflow_menu->actionGeometry(target_action).center())
                           ) == overflow_menu;
                })
                );

                const auto select = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_click(*overflow_menu, overflow_menu->actionGeometry(target_action).center());});
                dispatcher.wait_returned_until(select, deadline);
                dispatcher.wait_returned_until(open, deadline);
            });

        CHECK(open_acknowledgement == dispatch_ack::popup_opened);
        CHECK(target_action_triggered);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "routes_a_two_finger_pinch_through_the_target_window")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(360, 220);

        touch_recorder target(&window);
        target.setObjectName("touch_target");
        target.setGeometry(20, 20, 320, 180);
        target.setAttribute(Qt::WA_AcceptTouchEvents);
        window.show();

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_touch_pinch(
                        target,
                        {QPoint(100, 90), QPoint(60, 90)},
                        {QPoint(220, 90), QPoint(260, 90)
                        });
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(target.m_touch_begins == 1);
        CHECK(target.m_touch_updates > 0);
        CHECK(target.m_touch_ends == 1);
        CHECK(target.m_last_point_count == 2);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "delivers_a_pinch_to_a_widget_that_grabs_the_gesture")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(360, 220);

        pinch_recorder target(&window);
        target.setObjectName("pinch_target");
        target.setGeometry(20, 20, 320, 180);
        window.show();

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    // From 120 to 200 pixels apart: a 5/3 zoom.
                    inject_touch_pinch(target, {QPoint(100, 90), QPoint(60, 90)}, {QPoint(220, 90), QPoint(260, 90)});
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(target.m_finished);
        CHECK(target.m_total_scale_factor == doctest::Approx(200.0 / 120.0).epsilon(0.01));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "selects_a_nested_tree_item_with_mouse_and_keyboard")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(320, 200);

        QTreeWidget tree(&window);
        tree.setObjectName("tree_target");
        tree.setGeometry(20, 20, 260, 160);
        tree.setHeaderHidden(true);

        auto* const group = new QTreeWidgetItem(&tree, {QStringLiteral("Group")});
        auto* const point = new QTreeWidgetItem(group, {QStringLiteral("Point")});
        window.show();

        bool point_selected = false;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for_active_window(*this, window, deadline));

                auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    // The view itself, as its locator names it: its opaque viewport covers it whole.
                    inject_mouse_click(tree, tree.viewport()->mapTo(&tree, tree.visualItemRect(group).center()));
                });
                dispatcher.wait_returned_until(ticket, deadline);

                ticket = dispatcher.post_until(deadline, [&]{inject_key_click(tree, Qt::Key_Right);});
                dispatcher.wait_returned_until(ticket, deadline);
                REQUIRE(
                    wait_for(
                        *this,
                        deadline,
                        [&]
                {
                    return group->isExpanded() && tree.visualItemRect(point).isValid();
                })
                );

                ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_mouse_click(*tree.viewport(), tree.visualItemRect(point).center());
                    point_selected = tree.currentItem() == point && tree.selectedItems().contains(point);
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(point_selected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_key_for_an_unfocused_widget_in_a_popup")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);
        window.show();

        QMenu menu(&window);
        menu.setObjectName("menu_target");

        auto* const focused_widget = new key_recorder;
        focused_widget->setObjectName("focused_popup_widget");
        auto* const focused_action = new QWidgetAction(&menu);
        focused_action->setDefaultWidget(focused_widget);
        menu.addAction(focused_action);

        auto* const target = new key_recorder;
        target->setObjectName("unfocused_popup_widget");
        auto* const target_action = new QWidgetAction(&menu);
        target_action->setDefaultWidget(target);
        menu.addAction(target_action);

        bool popup_widget_got_focus = false;
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                auto ticket = dispatcher.post_until(deadline, [&]{menu.popup(window.mapToGlobal(QPoint(150, 20)));});
                dispatcher.wait_returned_until(ticket, deadline);

                // Windows fades a menu in, and clicks go through it until then: wait for the menu to be what lies
                // under the widget, as a user does.
                REQUIRE(wait_for_exposure(*this, menu, deadline));
                REQUIRE(
                    wait_for(
                        *this,
                        deadline,
                        [&]
                {
                    return QApplication::topLevelAt(focused_widget->mapToGlobal(focused_widget->rect().center()))
                           == &menu;
                })
                );
                ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_mouse_click(*focused_widget, focused_widget->rect().center());
                    popup_widget_got_focus = QApplication::focusWidget() == focused_widget;
                    try
                    {
                        inject_key_click(*target, Qt::Key_A, Qt::NoModifier, QStringLiteral("a"));
                    }
                    catch(const std::runtime_error& error)
                    {
                        failure = error.what();
                    }

                    menu.close();
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(popup_widget_got_focus);
        CHECK(failure.find("does not have focus") != std::string::npos);
        CHECK(focused_widget->m_key_presses == 0);
        CHECK(target->m_key_presses == 0);
        CHECK(target->text().isEmpty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_key_when_focus_is_elsewhere")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        key_recorder target(&window);
        target.setObjectName("key_target");
        target.setGeometry(20, 20, 120, 40);

        key_recorder other(&window);
        other.setObjectName("other_key_target");
        other.setGeometry(20, 70, 120, 40);
        window.show();

        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                REQUIRE(wait_for_active_window(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    inject_mouse_click(other, other.rect().center());
                    try
                    {
                        inject_key_click(target, Qt::Key_A, Qt::NoModifier, QStringLiteral("a"));
                    }
                    catch(const std::runtime_error& error)
                    {
                        failure = error.what();
                    }
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(failure.find("does not have focus") != std::string::npos);
        CHECK(target.m_key_presses == 0);
        CHECK(target.text().isEmpty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "keeps_a_drag_released_outside_the_target_on_the_target")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        drag_recorder handle(&window);
        handle.setObjectName("drag_handle");
        handle.setGeometry(20, 20, 40, 40);

        QWidget sibling(&window);
        sibling.setObjectName("sibling");
        sibling.setGeometry(120, 20, 100, 80);
        window.show();

        // The release lands on the sibling, 150 pixels right of the handle's origin.
        const QPoint from(20, 20);
        const QPoint to(150, 30);

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(deadline, [&]{inject_mouse_drag(handle, from, to);});
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(handle.m_presses == 1);
        CHECK(handle.m_moves == 1);
        CHECK(handle.m_releases == 1);
        CHECK(handle.m_last_position == to);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "checks_the_target_again_before_the_second_press_of_a_double_click")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target_button");
        target.setGeometry(20, 20, 120, 60);

        QWidget overlay(&window);
        overlay.setObjectName("notification_overlay");
        overlay.setGeometry(target.geometry());
        overlay.hide();
        window.show();

        int target_clicks = 0;
        QObject::connect(
            &target,
            &QPushButton::clicked,
            [&]
            {
                ++target_clicks;
                overlay.show();
                overlay.raise();
            });
        std::string failure;
        bool retryable = true;

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_double_click(target, target.rect().center());});
                try
                {
                    dispatcher.wait_returned_until(ticket, deadline);
                }
                catch(const input_failure& error)
                {
                    failure   = error.what();
                    retryable = error.retryable();
                }
            });

        CHECK(failure.find("notification_overlay") != std::string::npos);
        CHECK(failure.find("after 1 click(s) reached the target") != std::string::npos);
        CHECK_FALSE(retryable);
        CHECK(target_clicks == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "keeps_a_click_after_a_qt_test_click_a_single_click")
    {
        press_counter window;
        window.setObjectName("counter_window");
        window.resize(240, 120);
        window.show();

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto ticket = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    // QTest advances its clock by the double-click interval after a click: on a separate clock,
                    // the next press would follow QTest's press within that interval.
                    const QPoint center = window.rect().center();
                    QTest::mouseClick(window.windowHandle(), Qt::LeftButton, Qt::NoModifier, center, 10);
                    inject_mouse_click(window, center);
                });
                dispatcher.wait_returned_until(ticket, deadline);
            });

        CHECK(window.presses == 2);
        CHECK(window.double_clicks == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_window_blocked_by_a_modal_dialog")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target_button");
        target.setGeometry(20, 20, 120, 60);
        window.show();

        QDialog dialog;
        dialog.setObjectName("blocking_dialog");
        dialog.setModal(true);
        dialog.resize(100, 50);

        int target_clicks = 0;
        QObject::connect(&target, &QPushButton::clicked, [&target_clicks]{++target_clicks;});
        std::string failure;

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                const auto modal = dispatcher.post_until(deadline, [&]{dialog.exec();});
                REQUIRE(dispatcher.acknowledge_until(modal, deadline) == dispatch_ack::modal_opened);

                const auto click = dispatcher.post_until(
                    deadline,
                    [&]{inject_mouse_click(target, target.rect().center());});
                failure = failure_of([&]{dispatcher.wait_returned_until(click, deadline);});

                const auto close = dispatcher.post_until(deadline, [&]{dialog.reject();});
                dispatcher.wait_returned_until(close, deadline);
                dispatcher.wait_returned_until(modal, deadline);
            });

        CHECK(failure.find("blocked by modal dialog 'blocking_dialog'") != std::string::npos);
        CHECK(target_clicks == 0);
    }
}

} // namespace sight::ui::test::detail

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

#include <ui/test/detail/deadline.hpp>
#include <ui/test/detail/locator.hpp>
#include <ui/test/detail/test_window.hpp>
#include <ui/test/detail/widget_visibility.hpp>
#include <ui/test/locator.hpp>
#include <ui/test/scenario.hpp>

#include <doctest/doctest.h>

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QCompleter>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QFile>
#include <QIODevice>
#include <QImage>
#include <QIntValidator>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMetaMethod>
#include <QObject>
#include <QPoint>
#include <QPushButton>
#include <QRadioButton>
#include <QResizeEvent>
#include <QScopeGuard>
#include <QScrollArea>
#include <QSize>
#include <QSlider>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTemporaryDir>
#include <QTest>
#include <QThread>
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <latch>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

namespace sight::ui::test::detail
{

/// The failure of an expectation that timed out after observing its widget.
static std::string timeout_after_observing(const std::function<void(std::chrono::milliseconds)>& _expectation)
{
    return conclusive_failure(
        _expectation,
        [](const std::string& _message)
        {
            return _message.find("no visible widget") == std::string::npos
                   && _message.find("no widget named") == std::string::npos;
        });
}

//------------------------------------------------------------------------------

/// The failure of an operation that saw the hidden matches of its target.
static std::string timeout_with_hidden_matches(const std::function<void(std::chrono::milliseconds)>& _operation)
{
    return conclusive_failure(
        _operation,
        [](const std::string& _message)
        {
            return _message.find("hidden match") != std::string::npos
                   || _message.find("timed out:") == std::string::npos;
        });
}

//------------------------------------------------------------------------------

namespace
{

/// Counts the times a widget is shown.
class show_counter final : public QObject
{
public:

    int shows {0};

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _watched, QEvent* _event) override
    {
        if(_event->type() == QEvent::Show)
        {
            ++shows;
        }

        return QObject::eventFilter(_watched, _event);
    }
};

//------------------------------------------------------------------------------

/// A window with a combo named "target" holding "Alpha" and "Beta", which records what a user's choice emits.
struct combo_window final
{
    combo_window()
    {
        window.resize(320, 180);
        target.setObjectName("target");
        target.setGeometry(20, 20, 180, 32);
        target.addItems({QStringLiteral("Alpha"), QStringLiteral("Beta")});
        QObject::connect(&target, &QComboBox::activated, [this](int _row){activated_rows.push_back(_row);});
        QObject::connect(&target, &QComboBox::textActivated, [this](const QString&){++activations;});
        QObject::connect(&target, &QComboBox::currentIndexChanged, [this](int){++index_changes;});
        target.view()->window()->installEventFilter(&popup);
        window.show();
    }

    QWidget window;
    QComboBox target {&window};
    int activations {0};
    std::vector<int> activated_rows;
    int index_changes {0};
    show_counter popup;
};

//------------------------------------------------------------------------------

/// Puts itself back after the first request, as a window manager may do.
class contested_window final : public QWidget
{
public:

    bool moved_back {false};

protected:

    //------------------------------------------------------------------------------

    void resizeEvent(QResizeEvent* /*_event*/) override
    {
        if(!moved_back && size() == QSize(1280, 800))
        {
            moved_back = true;
            resize(640, 480);
        }
    }
};

/// Goes back to its size, whatever is asked.
class stubborn_window final : public QWidget
{
protected:

    //------------------------------------------------------------------------------

    void resizeEvent(QResizeEvent* /*_event*/) override
    {
        if(size() != QSize(640, 480))
        {
            resize(640, 480);
        }
    }
};

/// A window holding the "target" push button the signal watch tests click.
struct button_window
{
    QWidget window;
    QPushButton target {"target", &window};

    button_window()
    {
        window.resize(240, 120);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        window.show();
    }
};

/// A push button whose connections to clicked wait for release in a nested event loop, as a call opening a dialog
/// does, and that counts the connections still in place.
class held_connection_button final : public QPushButton
{
public:

    using QPushButton::QPushButton;

    std::atomic_int connections {0};
    std::atomic_bool held {false};
    std::atomic_bool released {false};
    std::atomic_bool resumed {false};

protected:

    //------------------------------------------------------------------------------

    void connectNotify(const QMetaMethod& _signal) override
    {
        if(_signal != QMetaMethod::fromSignal(&QPushButton::clicked))
        {
            return;
        }

        ++connections;
        held = true;
        // Bounded: a test that never releases fails instead of hanging.
        static_cast<void>(QTest::qWaitFor([this]{return released.load();}, 60000));
        resumed = true;
    }

    //------------------------------------------------------------------------------

    void disconnectNotify(const QMetaMethod& _signal) override
    {
        if(_signal == QMetaMethod::fromSignal(&QPushButton::clicked))
        {
            --connections;
        }
    }
};

/// A scroll area taller inside than out: "top" in view, "hidden" and "bottom" below the fold.
struct scroll_panel
{
    QScrollArea area;
    QLabel* hidden {nullptr};

    scroll_panel()
    {
        area.resize(200, 100);
        area.setWidgetResizable(true);

        auto* const content = new QWidget();
        auto* const layout  = new QVBoxLayout(content);
        for(const char* const name : {"top", "hidden", "bottom"})
        {
            auto* const label = new QLabel(name);
            label->setObjectName(name);
            layout->addWidget(label);
            for(int row = 0 ; row < 15 ; ++row)
            {
                layout->addWidget(new QLabel(QString("row %1").arg(row)));
            }

            if(std::string_view(name) == "hidden")
            {
                hidden = label;
            }
        }

        area.setWidget(content);
    }
};

} // namespace

//------------------------------------------------------------------------------

TEST_SUITE("sight::ui::test::scenario")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "normalizes_the_main_window_client_area")
    {
        QWidget window;
        window.setMinimumSize(1280, 720);
        window.showMaximized();
        run_scenario(*this, [&]{prepare_test_window(gateway, &window, operation_deadline());});
        CHECK(window.size() == QSize(1280, 800));
        CHECK_FALSE(window.isMaximized());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "places_the_main_window_at_the_top_left_of_its_screen")
    {
        // Wherever the system put it: a tall window placed low has its bottom off screen.
        QWidget window;
        window.show();
        const QRect available = window.screen()->availableGeometry();
        window.move(available.bottomRight() - QPoint(100, 100));
        run_scenario(*this, [&]{prepare_test_window(gateway, &window, operation_deadline());});
        CHECK(window.pos() == available.topLeft());
        CHECK(window.size() == QSize(1280, 800));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "asks_again_when_the_window_manager_moves_the_main_window_back")
    {
        contested_window window;
        window.show();
        run_scenario(*this, [&]{prepare_test_window(gateway, &window, operation_deadline());});
        CHECK(window.moved_back);
        CHECK(window.size() == QSize(1280, 800));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_a_destroyed_main_window_during_setup")
    {
        QPointer<QWidget> window = new QWidget();
        delete window.data();
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_WITH_AS(
                    prepare_test_window(gateway, window, operation_deadline()),
                    doctest::Contains("destroyed during setup"),
                    std::runtime_error
                );
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "keeps_the_minimum_size_of_the_main_window")
    {
        QWidget window;
        QSize minimum;
        QSize expected;
        SUBCASE("too tall")
        {
            minimum  = {800, 1068};
            expected = {1280, 1068};
        }
        SUBCASE("too wide and too tall")
        {
            minimum  = {1400, 900};
            expected = {1400, 900};
        }
        window.setMinimumSize(minimum);
        window.show();
        run_scenario(*this, [&]{prepare_test_window(gateway, &window, operation_deadline());});
        CHECK(window.size() == expected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_a_main_window_that_never_takes_its_size")
    {
        stubborn_window window;
        window.show();
        run_scenario(
            *this,
            [&]
            {
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout)
                {
                    prepare_test_window(gateway, &window, std::chrono::steady_clock::now() + _timeout);
                },
                    [](const std::string& _message){return _message.find("640x480") != std::string::npos;});
                INFO(failure);
                CHECK(failure.find("1280x800") != std::string::npos);
                CHECK(failure.find("640x480") != std::string::npos);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "tells_the_size_of_a_main_window_when_the_gui_is_busy_at_the_deadline")
    {
        stubborn_window window;
        window.show();
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                // The window is seen at its own size first, then its last poll meets a busy GUI.
                const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
                std::thread busy(
                    [this, deadline]
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    QMetaObject::invokeMethod(
                        &application,
                        [deadline]
                    {
                        std::this_thread::sleep_until(deadline + std::chrono::seconds(1));
                    },
                        Qt::QueuedConnection
                    );
                });
                failure = failure_of([&]{prepare_test_window(gateway, &window, deadline);});

                busy.join();
            });

        INFO(failure);
        CHECK(failure.find("stays at 640x480") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_invalid_image_operations_before_running_the_action")
    {
        button_window button;
        scenario actions(application);
        CHECK_THROWS_AS(static_cast<void>(actions.image("target")), std::logic_error);
        int calls = 0;
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_AS(
                    static_cast<void>(actions.image("target", std::chrono::seconds::zero())),
                    std::invalid_argument
                );
                CHECK_THROWS_AS(static_cast<void>(actions.image("target")), std::invalid_argument);
                CHECK_THROWS_AS(actions.expect_image_changes("target", [&]{++calls;}), std::invalid_argument);
            });
        CHECK(calls == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "observes_action_enabled_state_without_opening_its_menu")
    {
        button_window button;
        QMenu menu(&button.window);
        QAction action("Load", &menu);
        action.setObjectName("load_action");
        action.setEnabled(false);
        menu.addAction(&action);
        QObject::connect(&button.target, &QPushButton::clicked, [&]{action.setEnabled(true);});

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.expect("load_action").disabled();
                actions.click("target");
                actions.expect("load_action").enabled();
                const auto failure = timeout_after_observing(
                    [&](std::chrono::milliseconds _timeout)
                {
                    actions.expect("load_action").within(_timeout).disabled();
                });
                CHECK(failure.find("last value: enabled") != std::string::npos);
            });
        CHECK_FALSE(menu.isVisible());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_and_observes_a_checkable_toolbar_action")
    {
        QMainWindow window;
        QToolBar toolbar(&window);
        window.addToolBar(&toolbar);
        QAction* const volume = toolbar.addAction("volume");
        volume->setObjectName("volume");
        volume->setCheckable(true);
        int toggles = 0;
        QObject::connect(volume, &QAction::toggled, [&toggles]{++toggles;});
        toolbar.addAction("plain")->setObjectName("plain");
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.expect("volume").unchecked();
                actions.set("volume", true);
                actions.expect("volume").checked();
                actions.set("volume", true);
                actions.set("volume", false);
                actions.expect("volume").unchecked();
                const auto failure = timeout_after_observing(
                    [&](std::chrono::milliseconds _timeout)
                {
                    actions.expect("volume").within(_timeout).checked();
                });
                CHECK(failure.find("last value: unchecked") != std::string::npos);
                CHECK_THROWS_AS(actions.expect("plain").checked(), std::invalid_argument);
                CHECK_THROWS_AS(actions.set("plain", true), std::invalid_argument);
            });

        CHECK(toggles == 2);
        CHECK_FALSE(volume->isChecked());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "observes_whether_a_toolbar_action_is_shown")
    {
        // As video controls do: Pause takes the place of Start.
        QMainWindow window;
        QToolBar toolbar(&window);
        window.addToolBar(&toolbar);
        QAction* const start = toolbar.addAction("start");
        start->setObjectName("start");
        QAction* const pause = toolbar.addAction("pause");
        pause->setObjectName("pause");
        pause->setVisible(false);
        QObject::connect(
            start,
            &QAction::triggered,
            [start, pause]
            {
                start->setVisible(false);
                pause->setVisible(true);
            });
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.expect("pause").hidden();
                actions.click("start");
                actions.expect("start").hidden();
                actions.expect("pause").visible();
                const auto failure = timeout_after_observing(
                    [&](std::chrono::milliseconds _timeout)
                {
                    actions.expect("start").within(_timeout).visible();
                });
                CHECK(failure.find("last value: not visible") != std::string::npos);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "watches_widget_signals_before_and_after_waiting")
    {
        button_window button;

        scenario actions(application);
        bool duplicate_wait_rejected = false;
        run_scenario(
            *this,
            [&]
            {
                // The click may finish the signal before wait starts.
                auto first = actions.watch_signal("target", &QPushButton::clicked);
                actions.click("target");
                first.wait();
                duplicate_wait_rejected = throws<std::logic_error>([&]{first.wait();});

                auto second = actions.watch_signal("target", &QPushButton::clicked);
                gateway.call_until(
                    operation_deadline(),
                    [&application = application, & target = button.target]
                {
                    QTimer::singleShot(0, &application, [&target]{target.click();});
                });
                second.wait();
            });

        CHECK(duplicate_wait_rejected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_count_a_signal_emitted_before_a_watch")
    {
        button_window button;

        scenario actions(application);
        std::string failure;
        bool wrong_type_rejected     = false;
        bool invalid_signal_rejected = false;
        run_scenario(
            *this,
            [&]
            {
                actions.click("target");
                auto watch = actions.watch_signal("target", &QPushButton::clicked);
                failure    = failure_of([&]{watch.wait(std::chrono::milliseconds(200));});

                // A timeout does not consume the watch; a later emission can still satisfy it.
                actions.click("target");
                watch.wait();

                try
                {
                    static_cast<void>(actions.watch_signal("target", &QLineEdit::textChanged));
                }
                catch(const std::invalid_argument&)
                {
                    wrong_type_rejected = true;
                }

                try
                {
                    static_cast<void>(actions.watch_signal("target", &QAbstractButton::setText));
                }
                catch(const std::invalid_argument&)
                {
                    invalid_signal_rejected = true;
                }
            });

        CHECK(failure == "signal on 'target' timed out");
        CHECK(wrong_type_rejected);
        CHECK(invalid_signal_rejected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "watches_each_repeated_click_once")
    {
        button_window button;

        scenario actions(application);
        int observed = 0;
        run_scenario(
            *this,
            [&]
            {
                for(int index = 0 ; index < 20 ; ++index)
                {
                    auto watch = actions.watch_signal("target", &QPushButton::clicked);
                    actions.click("target");
                    watch.wait();
                    ++observed;
                }
            });

        CHECK(observed == 20);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "releases_a_watch_whose_connection_ends_after_its_deadline")
    {
        QWidget window;
        window.resize(240, 120);
        held_connection_button target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        window.show();

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                // The connection is released once watch_signal has returned: it ends after the deadline.
                failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout)
                {
                    static_cast<void>(actions.watch_signal("target", &QPushButton::clicked, _timeout));
                },
                    [&](const std::string&){return target.held.load();});
                target.released     = true;
                const auto deadline = operation_deadline();
                while(!target.resumed && std::chrono::steady_clock::now() < deadline)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }

                // Out of the nested loop, this call runs once the held one and its closure are gone.
                gateway.call_until(operation_deadline(), []{});
            });

        CHECK(failure.starts_with("read on 'target' timed out"));
        REQUIRE(target.resumed);
        // The watch nobody received disconnected its signal when the call let it go.
        CHECK(target.connections == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_destroyed_signal_target")
    {
        QWidget window;
        window.resize(240, 120);
        auto* const target = new QPushButton("target", &window);
        target->setObjectName("target");
        target->setGeometry(20, 20, 120, 60);
        window.show();

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                auto watch = actions.watch_signal("target", &QPushButton::clicked);
                gateway.call_until(operation_deadline(), [target]{delete target;});
                failure = failure_of([&]{watch.wait();});
            });

        CHECK(failure == "signal target 'target' was destroyed");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "allows_only_one_concurrent_signal_waiter_to_consume_an_emission")
    {
        button_window button;

        scenario actions(application);
        std::atomic_int received {0};
        std::atomic_int rejected {0};
        std::atomic_int unexpected {0};
        run_scenario(
            *this,
            [&]
            {
                auto watch = actions.watch_signal("target", &QPushButton::clicked);
                std::latch ready(2);
                const auto wait = [&]
                                  {
                                      ready.count_down();
                                      try
                                      {
                                          watch.wait();
                                          ++received;
                                      }
                                      catch(const std::logic_error&)
                                      {
                                          ++rejected;
                                      }
                                      catch(const std::exception&)
                                      {
                                          ++unexpected;
                                      }
                                  };

                std::jthread first(wait);
                std::jthread second(wait);
                ready.wait();
                actions.click("target");
            });

        CHECK(received.load() == 1);
        CHECK(rejected.load() == 1);
        CHECK(unexpected.load() == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_invalid_signal_waits")
    {
        button_window button;

        scenario actions(application);
        bool bad_timeout = false;
        bool moved_watch = false;
        bool gui_thread  = false;
        run_scenario(
            *this,
            [&]
            {
                auto original = actions.watch_signal("target", &QPushButton::clicked);
                auto watch    = std::move(original);
                try
                {
                    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
                    original.wait();
                }
                catch(const std::logic_error&)
                {
                    moved_watch = true;
                }

                try
                {
                    watch.wait(std::chrono::steady_clock::duration::zero());
                }
                catch(const std::invalid_argument&)
                {
                    bad_timeout = true;
                }

                gui_thread = gateway.call_until(
                    operation_deadline(),
                    [&watch]
                {
                    try
                    {
                        watch.wait();
                    }
                    catch(const std::logic_error&)
                    {
                        return true;
                    }

                    return false;
                });
            });

        CHECK(moved_watch);
        CHECK(bad_timeout);
        CHECK(gui_thread);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "signal_wait_fails_when_the_event_loop_stops")
    {
        button_window button;

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                auto watch        = actions.watch_signal("target", &QPushButton::clicked);
                const bool queued = QMetaObject::invokeMethod(
                    &application,
                    []{QCoreApplication::exit(0);},
                    Qt::QueuedConnection
                );
                if(!queued)
                {
                    throw std::runtime_error("could not stop the event loop");
                }

                failure = failure_of([&]{watch.wait();});
            });

        CHECK(failure == "the application event loop has stopped");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "clicks_a_widget_once_it_becomes_visible")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        window.show();

        QTimer::singleShot(20, &application, [&target]{target.show();});

        int click_count = 0;
        QObject::connect(&target, &QPushButton::clicked, [&click_count]{++click_count;});
        scenario actions(application);

        run_scenario(
            *this,
            [&]
            {
                actions.click("target");
                actions.finish();
            });

        CHECK(click_count == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "double_clicks_a_widget")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        window.show();

        int click_count = 0;
        QObject::connect(&target, &QPushButton::clicked, [&click_count]{++click_count;});
        scenario actions(application);

        run_scenario(
            *this,
            [&]
            {
                actions.double_click("target");
                actions.finish();
            });

        CHECK(click_count == 2);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_retry_a_double_click_after_its_first_click")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);

        QWidget overlay(&window);
        overlay.setGeometry(target.geometry());
        overlay.hide();
        window.show();

        int click_count = 0;
        QObject::connect(
            &target,
            &QPushButton::clicked,
            [&]
            {
                ++click_count;
                overlay.show();
                overlay.raise();
            });
        scenario actions(application);

        bool failed = false;
        run_scenario(
            *this,
            [&]
            {
                try
                {
                    actions.double_click("target");
                    actions.finish();
                }
                catch(const std::runtime_error&)
                {
                    failed = true;
                }
            });

        CHECK(failed);
        CHECK(click_count == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "right_clicks_a_widget")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        target.setContextMenuPolicy(Qt::CustomContextMenu);
        window.show();

        int context_menu_requests = 0;
        QObject::connect(
            &target,
            &QWidget::customContextMenuRequested,
            [&context_menu_requests](const QPoint&){++context_menu_requests;});
        scenario actions(application);

        run_scenario(
            *this,
            [&]
            {
                actions.right_click("target");
                actions.finish();
            });

        CHECK(context_menu_requests == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "drags_from_a_widget_to_a_point_outside_it")
    {
        QWidget window;
        window.resize(240, 120);

        drag_recorder target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 40, 40);

        drag_recorder sibling(&window);
        sibling.setGeometry(80, 20, 120, 80);
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.drag("target", at(0.5, 0.5), at(2.0, 0.5));
                actions.finish();
            });

        CHECK(target.m_presses == 1);
        CHECK(target.m_moves == 1);
        CHECK(target.m_releases == 1);
        CHECK(target.m_last_position == QPoint(78, 19));
        CHECK(sibling.m_presses == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "wheels_over_a_widget_by_notches")
    {
        QWidget window;
        window.resize(240, 120);

        wheel_recorder target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.wheel("target", 3);
                actions.wheel("target", -2);
                actions.finish();
            });

        CHECK(target.m_wheel_events == 5);
        CHECK(target.m_angle_delta == QPoint(0, -120));
        CHECK(target.m_phase == Qt::NoScrollPhase);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "scrolls_a_widget_into_view_and_back")
    {
        scroll_panel panel;
        panel.area.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.scroll_to("bottom");
                actions.expect("bottom").visible();
                actions.expect("top").hidden();

                actions.scroll_to("top");
                actions.expect("top").visible();
                actions.expect("bottom").hidden();
                actions.finish();
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "scrolls_a_partly_visible_widget_entirely_into_view")
    {
        QScrollArea area;
        area.resize(200, 100);
        auto* const content = new QWidget();
        content->setFixedSize(180, 400);

        // Its centre, where a click goes, is below the viewport while its top shows.
        auto* const partly = new QPushButton("partly", content);
        partly->setObjectName("partly");
        partly->setGeometry(10, 80, 100, 40);
        int clicks = 0;
        QObject::connect(partly, &QPushButton::clicked, [&clicks]{++clicks;});

        // Taller than the viewport: as much of it as fits, never a scroll that hides its top.
        auto* const tall = new QLabel("tall", content);
        tall->setObjectName("tall");
        tall->setGeometry(10, 160, 100, 220);
        area.setWidget(content);
        area.show();

        const auto fully_in_view = [](const QWidget& _widget)
                                   {
                                       return sight::ui::test::detail::visible_rect(_widget) == _widget.rect();
                                   };

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.scroll_to("partly");
                actions.expect("partly").within(std::chrono::seconds(2)).that<QPushButton>(
                    "is entirely in view",
                    fully_in_view
                );
                actions.click("partly");

                actions.scroll_to("tall");
                actions.expect("tall").within(std::chrono::seconds(2)).that<QLabel>(
                    "fills the viewport",
                    [viewport_height = area.viewport()->height()](const QLabel& _label)
                {
                    return sight::ui::test::detail::visible_rect(_label).height() == viewport_height;
                });
                actions.finish();
            });

        CHECK(clicks == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "scrolls_to_the_toolbar_button_of_an_action")
    {
        QScrollArea area;
        area.resize(200, 100);
        auto* const content = new QWidget();
        content->setFixedSize(180, 400);

        // An action has no widget of its own: the button its toolbar shows is what a user scrolls to.
        auto* const toolbar = new QToolBar(content);
        toolbar->setGeometry(10, 340, 160, 40);
        QAction* const action = toolbar->addAction("Far");
        action->setObjectName("toolbar/Far");
        int triggers = 0;
        QObject::connect(action, &QAction::triggered, [&triggers]{++triggers;});
        area.setWidget(content);
        area.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.scroll_to("toolbar/Far");
                actions.click("toolbar/Far");
                actions.finish();
            });

        CHECK(triggers == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "fails_to_scroll_to_a_hidden_widget_or_without_a_bar")
    {
        scroll_panel panel;
        panel.hidden->hide();
        // Without its bar, a user cannot scroll the area with the wheel.
        panel.area.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        panel.area.show();

        scenario actions(application);
        std::string hidden_failure;
        std::string stuck_failure;
        run_scenario(
            *this,
            [&]
            {
                hidden_failure = timeout_after_observing(
                    [&](std::chrono::milliseconds _timeout){actions.scroll_to("hidden", _timeout);});
                stuck_failure = timeout_after_observing(
                    [&](std::chrono::milliseconds _timeout){actions.scroll_to("bottom", _timeout);});
                actions.finish();
            });

        CHECK(hidden_failure == "scroll to 'hidden' timed out: it is hidden, not scrolled out of view");
        CHECK(stuck_failure == "scroll to 'bottom' timed out: its scroll area cannot scroll towards it");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_retry_wheel_notches_after_partial_input")
    {
        QWidget window;
        window.resize(240, 120);

        wheel_recorder target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);

        QWidget overlay(&window);
        overlay.setGeometry(target.geometry());
        overlay.hide();
        window.show();

        target.m_on_wheel = [&]
                            {
                                if(target.m_wheel_events == 1)
                                {
                                    overlay.show();
                                    overlay.raise();
                                }
                            };

        scenario actions(application);
        bool failed = false;
        run_scenario(
            *this,
            [&]
            {
                try
                {
                    actions.wheel("target", 3);
                    actions.finish();
                }
                catch(const std::runtime_error&)
                {
                    failed = true;
                }
            });

        CHECK(failed);
        CHECK(target.m_wheel_events == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "pinches_a_widget_by_the_requested_scale")
    {
        QWidget window;
        window.resize(360, 220);

        pinch_recorder target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 320, 180);
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.pinch("target", 5.0 / 3.0);
                actions.finish();
            });

        CHECK(target.m_finished);
        CHECK(target.m_total_scale_factor == doctest::Approx(5.0 / 3.0).epsilon(0.01));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "pinches_a_covered_widget_once_its_overlay_is_gone")
    {
        QWidget window;
        window.resize(360, 220);

        pinch_recorder target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 320, 180);

        QWidget overlay(&window);
        overlay.setGeometry(target.geometry());
        window.show();
        overlay.show();
        overlay.raise();

        // Like a notification, the overlay leaves on its own: the pinch waits for it, whatever the machine's speed.
        QTimer::singleShot(200, &application, [&overlay]{overlay.hide();});

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.pinch("target", 1.5);
                actions.finish();
            });

        CHECK(target.m_finished);
        CHECK(target.m_total_scale_factor == doctest::Approx(1.5).epsilon(0.01));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "types_unicode_text_without_committing")
    {
        QWidget window;
        window.resize(240, 120);

        QLineEdit target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 40);
        window.show();

        const std::string text("Sight \xC3\xA9 \xF0\x9F\x8C\x8D");
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.type("target", text);
                actions.finish();
            });

        CHECK(target.text() == QString::fromUtf8(text.c_str()));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "types_into_its_target_even_when_the_application_moves_the_focus")
    {
        QWidget window;
        window.resize(240, 120);

        QLineEdit target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 40);

        QLineEdit other(&window);
        other.setObjectName("other");
        other.setGeometry(20, 70, 120, 30);
        window.show();

        QObject::connect(
            &target,
            &QLineEdit::textChanged,
            [&]
            {
                if(target.text().size() == 1)
                {
                    other.setFocus();
                }
            });

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.type("target", "ab");});

                actions.finish();
            });

        // The keys go to the target itself, whatever has the focus: no key is lost to a focus change.
        INFO(failure);
        CHECK(failure.empty());
        CHECK(target.text() == QStringLiteral("ab"));
        CHECK(other.text().isEmpty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "presses_key_sequences_and_window_shortcuts")
    {
        QWidget window;
        window.resize(240, 120);

        QLineEdit target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 40);
        target.setText(QStringLiteral("initial"));

        QAction save("save", &window);
        save.setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
        window.addAction(&save);
        bool saved = false;
        QObject::connect(&save, &QAction::triggered, [&saved]{saved = true;});
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.press("target", "Ctrl+A, Backspace");
                actions.press("target", "Ctrl+S");
                actions.finish();
            });

        CHECK(target.text().isEmpty());
        CHECK(saved);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_retry_a_key_sequence_after_partial_input")
    {
        class focus_stealing_line_edit final : public QLineEdit
        {
        public:

            focus_stealing_line_edit(QWidget& _other, QWidget* _parent) :
                QLineEdit(_parent),
                m_other(_other)
            {
            }

            //------------------------------------------------------------------------------

            [[nodiscard]] int key_presses() const
            {
                return m_key_presses;
            }

        protected:

            //------------------------------------------------------------------------------

            void keyPressEvent(QKeyEvent* _event) override
            {
                ++m_key_presses;
                QLineEdit::keyPressEvent(_event);
                if(_event->key() == Qt::Key_A && _event->modifiers().testFlag(Qt::ControlModifier))
                {
                    m_other.setFocus();
                }
            }

        private:

            int m_key_presses {0};
            QWidget& m_other;
        };

        QWidget window;
        window.resize(240, 120);

        QLineEdit other(&window);
        other.setObjectName("other");
        other.setGeometry(20, 70, 120, 30);

        focus_stealing_line_edit target(other, &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 40);
        target.setText(QStringLiteral("initial"));
        window.show();

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.press("target", "Ctrl+A, Backspace");});

                actions.finish();
            });

        CHECK(target.text() == QStringLiteral("initial"));
        CHECK(target.key_presses() == 1);
        CHECK(failure.find("after 1 key(s) reached the target") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_a_checkable_button_only_when_its_state_changes")
    {
        QWidget window;
        window.resize(360, 120);

        // Wider than its indicator and label: its centre takes no click, on any style.
        QCheckBox target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 300, 40);
        window.show();

        int state_changes = 0;
        QObject::connect(&target, &QCheckBox::toggled, [&state_changes](bool){++state_changes;});

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.set("target", false);
                actions.set("target", true);
                actions.set("target", true);
                actions.set("target", false);
                actions.finish();
            });

        CHECK(!target.isChecked());
        CHECK(state_changes == 2);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_a_radio_button_through_its_click_rectangle")
    {
        QWidget window;
        window.resize(360, 120);

        QRadioButton first("first", &window);
        first.setObjectName("first");
        first.setGeometry(20, 20, 300, 30);
        first.setChecked(true);

        QRadioButton second("second", &window);
        second.setObjectName("second");
        second.setGeometry(20, 60, 300, 30);
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.set("second", true);
                actions.finish();
            });

        CHECK(second.isChecked());
        CHECK_FALSE(first.isChecked());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_setting_a_non_checkable_button")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 40);
        window.show();

        scenario actions(application);
        bool rejected = false;
        run_scenario(
            *this,
            [&]
            {
                rejected = throws<std::invalid_argument>([&]{actions.set("target", true);});

                actions.finish();
            });

        CHECK(rejected);
        CHECK(!target.isChecked());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_a_slider_value_through_qt_without_its_focus")
    {
        QWidget window;
        window.resize(240, 120);

        QSlider target(Qt::Horizontal, &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 180, 40);
        target.setRange(0, 100);
        target.setPageStep(10);
        target.setSingleStep(1);
        target.setValue(20);
        target.setFocusPolicy(Qt::NoFocus);
        window.show();

        std::vector<int> values;
        int presses = 0;
        QObject::connect(&target, &QSlider::valueChanged, [&values](int _value){values.push_back(_value);});
        QObject::connect(&target, &QSlider::sliderPressed, [&presses]{++presses;});
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.set("target", 73);
                target.setRange(0, 101);
                target.setSingleStep(4);
                actions.set("target", 49);
                actions.finish();
            });

        // One valueChanged per value, as the end of a user's keys, without the focus and without a drag.
        CHECK(target.value() == 49);
        CHECK(values == std::vector<int> {73, 49});
        CHECK(presses == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_a_slider_by_a_real_drag_when_requested")
    {
        QWidget window;
        window.resize(240, 120);

        QSlider target(Qt::Horizontal, &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 180, 40);
        target.setRange(0, 100);
        target.setValue(20);
        window.show();

        int presses  = 0;
        int releases = 0;
        QObject::connect(&target, &QSlider::sliderPressed, [&presses]{++presses;});
        QObject::connect(&target, &QSlider::sliderReleased, [&releases]{++releases;});
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.set("target", 35, by_drag);
                actions.finish();
            });

        // A real drag, for a service waiting for the release of the handle.
        CHECK(target.value() == 35);
        CHECK(presses == 1);
        CHECK(releases == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_a_slider_over_a_list_of_values")
    {
        QWidget window;
        window.resize(240, 120);

        // As ui::qt::widget::non_linear_slider: a "values" property over a QSlider of their indices.
        QWidget target(&window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 180, 40);
        target.setProperty("values", QVariant::fromValue(QList<int> {25, 50, 90, 120, 150}));
        QSlider indices(Qt::Horizontal, &target);
        indices.setGeometry(0, 0, 180, 40);
        indices.setRange(0, 4);
        indices.setValue(2);
        window.show();

        scenario actions(application);
        std::string not_a_value;
        run_scenario(
            *this,
            [&]
            {
                actions.set("target", 120);
                not_a_value = failure_of<std::invalid_argument>([&]{actions.set("target", 100);});

                actions.finish();
            });

        CHECK(indices.value() == 3);
        CHECK(not_a_value == "value is not one of the slider's values");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_line_edit_and_spin_box_values")
    {
        QWidget window;
        window.resize(320, 180);

        QLineEdit text_target(&window);
        text_target.setObjectName("text");
        text_target.setGeometry(20, 20, 180, 30);
        text_target.setText(QStringLiteral("old value"));

        QSpinBox int_target(&window);
        int_target.setObjectName("integer");
        int_target.setGeometry(20, 60, 120, 30);
        int_target.setRange(-100, 100);
        int_target.setValue(5);

        QDoubleSpinBox double_target(&window);
        double_target.setObjectName("double");
        double_target.setGeometry(20, 100, 120, 30);
        double_target.setRange(-100., 100.);
        double_target.setDecimals(2);
        double_target.setValue(1.25);
        window.show();

        int return_presses    = 0;
        int editing_finished  = 0;
        int text_edits        = 0;
        int spin_box_finished = 0;
        std::vector<int> int_values;
        QObject::connect(&text_target, &QLineEdit::returnPressed, [&return_presses]{++return_presses;});
        QObject::connect(&text_target, &QLineEdit::editingFinished, [&editing_finished]{++editing_finished;});
        QObject::connect(&text_target, &QLineEdit::textEdited, [&text_edits]{++text_edits;});
        QObject::connect(&int_target, &QSpinBox::editingFinished, [&spin_box_finished]{++spin_box_finished;});
        QObject::connect(
            &int_target,
            &QSpinBox::valueChanged,
            [&int_values](int _value){int_values.push_back(_value);});
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.set("text", "new value");
                actions.set("integer", 42);
                actions.set("double", 12.5);
                actions.finish();
            });

        // What a user's edit emits, computed by the fields themselves.
        CHECK(text_target.text() == QStringLiteral("new value"));
        CHECK(int_target.value() == 42);
        CHECK(double_target.value() == doctest::Approx(12.5));
        CHECK(return_presses == 1);
        CHECK(editing_finished == 1);
        CHECK(text_edits > 0);
        CHECK(spin_box_finished == 1);
        CHECK(int_values.back() == 42);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "fails_a_text_set_that_the_validator_of_a_line_edit_refuses")
    {
        QWidget window;
        window.resize(320, 140);

        QLineEdit target(&window);
        target.setObjectName("text");
        target.setGeometry(20, 20, 180, 30);
        QIntValidator validator(0, 10);
        target.setValidator(&validator);
        window.show();

        int return_presses = 0;
        QObject::connect(&target, &QLineEdit::returnPressed, [&return_presses]{++return_presses;});
        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                actions.set("text", "7");
                failure = failure_of([&]{actions.set("text", "42");});
            });

        // "42" can be typed, as an intermediate value, but Return does not commit it: the application never gets it.
        INFO(failure);
        CHECK(return_presses == 1);
        CHECK(failure.find("the line edit does not accept '42'") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "commits_a_text_set_even_when_the_application_moves_the_focus")
    {
        QWidget window;
        window.resize(320, 140);

        QLineEdit target(&window);
        target.setObjectName("text");
        target.setGeometry(20, 20, 180, 30);

        QLineEdit thief(&window);
        thief.setObjectName("thief");
        thief.setGeometry(20, 70, 180, 30);
        window.show();

        // The application moves the focus once the text is typed, before the Return.
        QObject::connect(
            &target,
            &QLineEdit::textChanged,
            [&thief](const QString& _text)
            {
                if(_text == QStringLiteral("abc"))
                {
                    thief.setFocus();
                }
            });
        int return_presses = 0;
        QObject::connect(&target, &QLineEdit::returnPressed, [&return_presses]{++return_presses;});

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.set("text", "abc");});
            });

        INFO(failure);
        CHECK(failure.empty());
        CHECK(target.text() == QStringLiteral("abc"));
        CHECK(return_presses == 1);
        CHECK(thief.text().isEmpty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "clears_a_line_edit")
    {
        QWidget window;
        window.resize(240, 100);

        QLineEdit target(&window);
        target.setObjectName("text");
        target.setGeometry(20, 20, 180, 30);
        target.setText(QStringLiteral("old value"));
        window.show();

        int return_presses = 0;
        QObject::connect(&target, &QLineEdit::returnPressed, [&return_presses]{++return_presses;});
        scenario actions(application);
        run_scenario(*this, [&]{actions.set("text", "");});

        CHECK(target.text().isEmpty());
        CHECK(return_presses == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_a_whole_value_on_a_double_spin_box")
    {
        QWidget window;
        window.resize(240, 120);

        QDoubleSpinBox target(&window);
        target.setObjectName("double");
        target.setGeometry(20, 20, 120, 30);
        target.setRange(-100., 100.);
        target.setValue(1.25);
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                // An int literal must not need a ".0" to reach a double spin box.
                actions.set("double", 3);
                actions.finish();
            });

        CHECK(target.value() == doctest::Approx(3.));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_a_combo_box_item_as_a_user_choice")
    {
        combo_window combo;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.set("target", "Beta");
                actions.finish();
            });

        CHECK(combo.target.currentText() == QStringLiteral("Beta"));
        CHECK(combo.activated_rows == std::vector<int> {1});
        CHECK(combo.activations == 1);
        CHECK(combo.index_changes == 1);
        CHECK(combo.popup.shows == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "selects_a_combo_item_as_a_user_choice")
    {
        combo_window combo;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.select("target", "Beta");
                actions.finish();
            });

        // What Qt emits once a user picked an item, without depending on the timing of its popup.
        CHECK(combo.target.currentText() == QStringLiteral("Beta"));
        CHECK(combo.activated_rows == std::vector<int> {1});
        CHECK(combo.activations == 1);
        CHECK(combo.index_changes == 1);
        CHECK(combo.popup.shows == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_disabled_combo_item_then_fails")
    {
        combo_window combo;
        auto* const model = qobject_cast<QStandardItemModel*>(combo.target.model());
        REQUIRE(model != nullptr);
        model->item(1)->setEnabled(false);

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.select("target", "Beta", std::chrono::milliseconds(500));});
            });

        INFO(failure);
        CHECK(failure.find("combo item 'Beta' is disabled") != std::string::npos);
        CHECK(combo.target.currentText() == QStringLiteral("Alpha"));
        CHECK(combo.activations == 0);
        CHECK(combo.index_changes == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "keeps_a_current_combo_item_without_opening_its_popup")
    {
        combo_window combo;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.select("target", "Alpha");
                actions.finish();
            });

        CHECK(combo.target.currentText() == QStringLiteral("Alpha"));
        CHECK(combo.activations == 0);
        CHECK(combo.popup.shows == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reads_a_copy_on_the_gui_thread")
    {
        QWidget window;
        window.setObjectName("target");
        window.resize(240, 120);
        window.show();

        scenario actions(application);
        std::optional<std::pair<bool, QSize> > observed;
        run_scenario(
            *this,
            [&]
            {
                observed = actions.read(
                    "target",
                    [](const QWidget& _widget)
                {
                    return std::pair {
                        QThread::currentThread() == _widget.thread(),
                        _widget.size()
                    };
                });
            });

        REQUIRE(observed.has_value());
        CHECK(observed->first);
        CHECK(observed->second == QSize(240, 120));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reads_a_widget_once_it_is_shown")
    {
        QWidget window;
        window.resize(240, 120);
        window.show();

        QLabel label(QStringLiteral("ready"), &window);
        label.setObjectName("label");
        label.hide();
        QTimer::singleShot(100, &label, &QWidget::show);

        scenario actions(application);
        QString text;
        run_scenario(*this, [&]{text = actions.read<QLabel>("label", &QLabel::text);});

        CHECK(text == QStringLiteral("ready"));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "looks_once_however_short_its_timeout")
    {
        QWidget window;
        QVBoxLayout layout(&window);
        QLabel label(QStringLiteral("ready"), &window);
        label.setObjectName("label");
        QPushButton toggle(QStringLiteral("toggle"), &window);
        toggle.setObjectName("toggle");
        toggle.setCheckable(true);
        layout.addWidget(&label);
        layout.addWidget(&toggle);
        window.show();

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                ui.expect("label").visible();
                ui.expect("toggle").visible();

                // The GUI answers after the timeout: the step still acts on, or tells, what the GUI shows.
                QMetaObject::invokeMethod(
                    &application,
                    []{std::this_thread::sleep_for(std::chrono::milliseconds(200));},
                    Qt::QueuedConnection
                );
                const auto timeout = std::chrono::milliseconds(1);
                SUBCASE("read")
                {
                    CHECK(ui.read<QLabel>("label", &QLabel::text, timeout) == QStringLiteral("ready"));
                }
                SUBCASE("expect")
                {
                    CHECK_NOTHROW(ui.expect("label").within(timeout).text("ready"));
                }
                SUBCASE("click")
                {
                    CHECK_NOTHROW(ui.click("toggle", {}, timeout));
                    CHECK_NOTHROW(ui.expect("toggle").checked());
                }
                SUBCASE("scroll_to")
                {
                    CHECK_NOTHROW(ui.scroll_to("label", timeout));
                }
                SUBCASE("eventually")
                {
                    CHECK_NOTHROW(ui.eventually("the label exists", [&]{return ui.exists("label", timeout);}, timeout));
                }
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "looks_again_when_the_gui_is_busy_at_the_deadline")
    {
        QWidget window;
        QVBoxLayout layout(&window);
        QLabel label(QStringLiteral("ready"), &window);
        label.setObjectName("label");
        QPushButton toggle(QStringLiteral("toggle"), &window);
        toggle.setObjectName("toggle");
        toggle.setCheckable(true);
        layout.addWidget(&label);
        layout.addWidget(&toggle);
        toggle.hide();
        window.show();

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                ui.expect("label").visible();

                // As an application starting a heavy view: the target shows after the first look, then the GUI is busy
                // past the timeout of the step, which still acts on what the GUI shows once it answers.
                QMetaObject::invokeMethod(
                    &application,
                    [&]
                {
                    QTimer::singleShot(
                        100,
                        &toggle,
                        [&]
                    {
                        toggle.show();
                        QMetaObject::invokeMethod(
                            &application,
                            []{std::this_thread::sleep_for(std::chrono::seconds(1));},
                            Qt::QueuedConnection
                        );
                    });
                },
                    Qt::QueuedConnection
                );
                CHECK_NOTHROW(ui.click("toggle", {}, std::chrono::milliseconds(500)));
                CHECK_NOTHROW(ui.expect("toggle").checked());
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reads_through_a_member_getter")
    {
        QWidget window;
        window.setObjectName("target");
        window.resize(240, 120);
        window.show();

        scenario actions(application);
        QSize size;
        run_scenario(*this, [&]{size = actions.read("target", &QWidget::size);});

        CHECK(size == QSize(240, 120));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_read_of_another_widget_type")
    {
        QWidget window;
        window.resize(240, 120);
        QPushButton button("Apply", &window);
        button.setObjectName("apply");
        QWidget plain_widget(&window);
        plain_widget.setObjectName("plain");
        press_counter custom_widget(&window);
        custom_widget.setObjectName("custom");
        custom_widget.presses = 4;
        window.show();

        scenario actions(application);
        bool getter_called        = false;
        bool custom_getter_called = false;
        std::string message;
        std::string plain_widget_message;
        int custom_value = 0;
        run_scenario(
            *this,
            [&]
            {
                try
                {
                    static_cast<void>(actions.read<QLabel>(
                                          "apply",
                                          [&getter_called](const QLabel&){getter_called = true; return 1;}));
                }
                catch(const std::invalid_argument& exception)
                {
                    message = exception.what();
                }

                custom_value = actions.read<press_counter>(
                    "custom",
                    [&custom_getter_called](const press_counter& _widget)
                {
                    custom_getter_called = true;
                    return _widget.presses;
                });

                try
                {
                    static_cast<void>(actions.read<press_counter>("plain", [](const press_counter&){return 1;}));
                }
                catch(const std::invalid_argument& exception)
                {
                    plain_widget_message = exception.what();
                }
            });

        CHECK(message == "read on 'apply' found a QPushButton, not a QLabel");
        CHECK_FALSE(getter_called);
        CHECK(custom_value == 4);
        CHECK(custom_getter_called);
        CHECK(plain_widget_message == "read on 'plain' found a QWidget, not the requested subclass");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_condition_on_the_gui_thread")
    {
        QWidget target;
        target.setObjectName("target");
        target.setEnabled(false);
        target.show();
        QTimer::singleShot(100, &target, [&target]{target.setEnabled(true);});

        scenario actions(application);
        bool predicate_on_gui_thread = false;
        run_scenario(
            *this,
            [&]
            {
                actions.expect("target").that(
                    "is enabled",
                    [&predicate_on_gui_thread](const QWidget& _widget)
                {
                    predicate_on_gui_thread = QThread::currentThread() == _widget.thread();
                    return _widget.isEnabled();
                });
            });

        CHECK(predicate_on_gui_thread);
        CHECK(target.isEnabled());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_typed_condition")
    {
        QWidget window;
        window.resize(240, 120);
        QSlider slider(Qt::Horizontal, &window);
        slider.setObjectName("slider");
        QPushButton button("Apply", &window);
        button.setObjectName("apply");
        window.show();
        QTimer::singleShot(100, &slider, [&slider]{slider.setValue(35);});

        scenario actions(application);
        std::string wrong_type;
        run_scenario(
            *this,
            [&]
            {
                actions.expect("slider").that<QSlider>(
                    "is at 35",
                    [](const QSlider& _slider){return _slider.value() == 35;});
                try
                {
                    actions.expect("apply").that<QSlider>("is at 35", [](const QSlider&){return true;});
                }
                catch(const std::invalid_argument& exception)
                {
                    wrong_type = exception.what();
                }
            });

        CHECK(slider.value() == 35);
        CHECK(wrong_type == "expect on 'apply' found a QPushButton, not a QSlider");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_an_expectation_timeout")
    {
        QWidget target;
        target.setObjectName("target");
        target.setEnabled(false);
        target.show();

        scenario actions(application);
        std::string missing;
        std::string last_value;
        std::string invalid_timeout;
        run_scenario(
            *this,
            [&]
            {
                try
                {
                    actions.expect("missing").within(std::chrono::milliseconds(100)).that(
                        "exists",
                        [](const QWidget&){return true;});
                }
                catch(const std::runtime_error& exception)
                {
                    missing = exception.what();
                }

                last_value = timeout_after_observing(
                    [&actions](std::chrono::milliseconds _deadline)
                {
                    actions.expect("target").within(_deadline).that(
                        "is enabled",
                        [](const QWidget& _widget){return _widget.isEnabled();});
                });

                try
                {
                    static_cast<void>(actions.expect("target").within(std::chrono::steady_clock::duration::zero()));
                }
                catch(const std::invalid_argument& exception)
                {
                    invalid_timeout = exception.what();
                }
            });

        CHECK(missing == "expect on 'missing' timed out: no visible widget named 'missing'");
        CHECK(last_value == "expect on 'target' timed out: is enabled");
        CHECK(invalid_timeout == "GUI expectation timeout must be positive");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "checks_text_numeric_and_native_value_matchers")
    {
        QWidget window;
        window.resize(320, 240);

        QLabel label("Signal 42", &window);
        label.setObjectName("label");

        QLineEdit numeric_text("42.5", &window);
        numeric_text.setObjectName("numeric_text");

        QDoubleSpinBox precise_value(&window);
        precise_value.setObjectName("precise_value");
        precise_value.setValue(37.5);

        QSpinBox integer_value(&window);
        integer_value.setObjectName("integer_value");
        integer_value.setValue(12);

        QSlider slider(Qt::Horizontal, &window);
        slider.setObjectName("slider");
        slider.setValue(40);

        QComboBox combo(&window);
        combo.setObjectName("combo");
        combo.addItems({"first", "second", "third"});
        combo.setCurrentIndex(1);

        QCheckBox checked(&window);
        checked.setObjectName("checked");
        checked.setChecked(true);

        QCheckBox unchecked(&window);
        unchecked.setObjectName("unchecked");

        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.expect("label").text("Signal 42");
                actions.expect("label").text_contains("42");
                actions.expect("label").text_matches("S.*");
                actions.expect("numeric_text").number(42.49, 0.02);
                actions.expect("precise_value").number(37.5, 0.001);
                actions.expect("precise_value").value(37.5);
                actions.expect("integer_value").value(12);
                actions.expect("slider").value(40);
                actions.expect("combo").value(1);
                actions.expect("checked").checked();
                actions.expect("unchecked").unchecked();
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "checks_widget_state_tooltip_and_root_rows")
    {
        QWidget window;
        window.resize(320, 240);

        QPushButton visible(&window);
        visible.setObjectName("visible");
        visible.setToolTip("Green");

        QPushButton disabled(&window);
        disabled.setObjectName("disabled");
        disabled.setEnabled(false);

        QPushButton hidden(&window);
        hidden.setObjectName("hidden");
        hidden.hide();

        QTreeWidget tree(&window);
        tree.setObjectName("tree");
        auto* const parent = new QTreeWidgetItem(&tree, QStringList {"parent"});
        new QTreeWidgetItem(parent, QStringList {"child"});
        new QTreeWidgetItem(&tree, QStringList {"second"});

        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.expect("visible").enabled();
                actions.expect("visible").visible();
                actions.expect("visible").tooltip("Green");
                actions.expect("disabled").disabled();
                actions.expect("hidden").hidden();
                actions.expect("tree").rows(2);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "requires_seen_locators_for_gone")
    {
        QWidget window;
        window.resize(240, 120);
        window.show();

        auto* const target = new QDialog(&window);
        target->setObjectName("target");
        target->setAttribute(Qt::WA_DeleteOnClose);
        target->show();

        scenario actions(application);
        std::string unseen_error;
        run_scenario(
            *this,
            [&]
            {
                actions.close("target");
                actions.expect("target").gone();

                unseen_error = failure_of<std::invalid_argument>([&]{actions.expect("typo").gone();});
                actions.expect("typo").never_existed_ok().gone();
            });

        CHECK(
            unseen_error
            == "gone on 'typo' requires a previous match; use never_existed_ok() to allow none"
        );
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_until_every_match_is_gone")
    {
        QWidget window;
        window.resize(240, 120);
        window.show();

        // Two hidden widgets of one name, never named by the scenario before: no ambiguity, just not gone yet. They
        // outlive the second of grace given to several matches.
        auto* const first  = new QWidget(&window);
        auto* const second = new QWidget(&window);
        first->setObjectName("item");
        second->setObjectName("item");
        QTimer::singleShot(1200, first, &QObject::deleteLater);
        QTimer::singleShot(1200, second, &QObject::deleteLater);

        scenario actions(application);
        run_scenario(*this, [&]{actions.expect("item").gone();});

        CHECK(window.findChildren<QWidget*>("item").empty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_observed_values_as_written")
    {
        QWidget window;
        QDoubleSpinBox spin_box(&window);
        spin_box.setObjectName("spin_box");
        spin_box.setValue(0.1);
        QLabel label("prefix ready suffix", &window);
        label.setObjectName("label");
        window.show();

        scenario actions(application);
        std::string value;
        run_scenario(
            *this,
            [&]
            {
                // The whole text against each alternative, not only the first that matches its start.
                actions.expect("label").text_matches("prefix|prefix ready suffix");
                value = timeout_after_observing(
                    [&actions](std::chrono::milliseconds _deadline)
                {
                    actions.expect("spin_box").within(_deadline).value(0.2);
                });
            });

        CHECK(value == "expect on 'spin_box' timed out: value is 0.2 (last value: 0.1)");
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "counts_an_eventually_predicate_that_succeeds_after_its_deadline")
    {
        scenario actions(application);
        int attempts = 0;
        run_scenario(
            *this,
            [&]
            {
                // What the predicate saw holds, however late it answers.
                constexpr auto timeout = std::chrono::milliseconds(500);
                const auto late_true   = [&attempts, timeout]
                                         {
                                             ++attempts;
                                             std::this_thread::sleep_until(deadline_after(timeout));
                                             return true;
                                         };
                CHECK_NOTHROW(actions.eventually("late result", late_true, timeout));
            });

        CHECK(attempts == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reads_a_png_once_its_writer_has_finished")
    {
        const QTemporaryDir folder;
        REQUIRE(folder.isValid());
        const QString file = folder.filePath("written.png");
        const std::filesystem::path path(file.toStdU16String());

        QImage written(4, 3, QImage::Format_RGB32);
        written.fill(Qt::red);
        QByteArray bytes;
        {
            QBuffer buffer(&bytes);
            REQUIRE(buffer.open(QIODevice::WriteOnly));
            REQUIRE(written.save(&buffer, "PNG"));
        }

        // A writer that has not finished yet: the file lacks its last chunk.
        {
            QFile partial(file);
            REQUIRE(partial.open(QIODevice::WriteOnly));
            REQUIRE(partial.write(bytes.left(bytes.size() - 12)) == bytes.size() - 12);
        }

        scenario actions(application);
        std::string truncated;
        QImage read;
        run_scenario(
            *this,
            [&]
            {
                try
                {
                    std::ignore = actions.read_png(path, std::chrono::milliseconds(300));
                }
                catch(const std::exception& e)
                {
                    truncated = e.what();
                }

                QFile complete(file);
                REQUIRE(complete.open(QIODevice::WriteOnly));
                REQUIRE(complete.write(bytes) == bytes.size());
                complete.close();
                read = actions.read_png(path);
            });

        CHECK(truncated.find("a complete PNG") != std::string::npos);
        CHECK(read.size() == QSize(4, 3));
        CHECK(read.pixelColor(0, 0) == QColor(Qt::red));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "retries_non_gui_expectations_and_rejects_bad_matchers")
    {
        QWidget window;
        QLabel label("prefix ready suffix", &window);
        label.setObjectName("label");
        window.show();

        scenario actions(application);
        std::string full_match_timeout;
        std::string case_sensitive_timeout;
        std::string invalid_text_type;
        std::string invalid_value_type;
        int attempts = 0;
        run_scenario(
            *this,
            [&]
            {
                actions.eventually("second probe", [&attempts]{return ++attempts == 2;});
                full_match_timeout = timeout_after_observing(
                    [&actions](std::chrono::milliseconds _deadline)
                {
                    actions.expect("label").within(_deadline).text_matches("ready");
                });
                case_sensitive_timeout = timeout_after_observing(
                    [&actions](std::chrono::milliseconds _deadline)
                {
                    actions.expect("label").within(_deadline).text_matches("s.*");
                });

                invalid_text_type = failure_of<std::invalid_argument>([&]{actions.expect("label").text_matches("[");});

                invalid_value_type = failure_of<std::invalid_argument>([&]{actions.expect("label").value(4);});
            });

        CHECK(attempts == 2);
        const std::string observed = R"( (last value: "prefix ready suffix"))";
        CHECK(full_match_timeout == "expect on 'label' timed out: text matches /ready/" + observed);
        CHECK(case_sensitive_timeout == "expect on 'label' timed out: text matches /s.*/" + observed);
        CHECK(invalid_text_type.find("invalid regular expression") != std::string::npos);
        CHECK(invalid_value_type.find("requires a slider, spin box or combo box") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "fails_a_read_of_a_missing_or_ambiguous_widget")
    {
        QWidget first;
        first.setObjectName("duplicate");
        first.show();

        QWidget second;
        second.setObjectName("duplicate");
        second.show();

        scenario actions(application);
        bool getter_called = false;
        std::string missing;
        std::string ambiguous;
        run_scenario(
            *this,
            [&]
            {
                const auto getter = [&getter_called](const QWidget&){getter_called = true; return 1;};
                try
                {
                    static_cast<void>(actions.read("missing", getter, std::chrono::milliseconds(200)));
                }
                catch(const std::runtime_error& exception)
                {
                    missing = exception.what();
                }

                // Several matches fail after the grace given to a transition, before the deadline.
                ambiguous = failure_of([&]{static_cast<void>(actions.read("duplicate", getter));});
            });

        CHECK(missing == "read on 'missing' timed out: no visible widget named 'missing'");
        CHECK(ambiguous.starts_with("several GUI objects named 'duplicate'"));
        CHECK_FALSE(getter_called);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sends_a_platform_close_request")
    {
        close_recorder window;
        window.setObjectName("target");
        window.resize(240, 120);
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.close("target");
                actions.finish();
            });

        CHECK(window.close_events == 1);
        CHECK_FALSE(window.isVisible());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "answers_the_close_confirmation_of_a_window")
    {
        close_recorder window;
        window.setObjectName("target");
        window.resize(240, 120);
        window.confirm = true;
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.close("target");
                actions.click("keep_open");
                actions.finish();
            });

        CHECK(window.close_events == 1);
        CHECK(window.isVisible());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "ends_with_an_unanswered_close_confirmation")
    {
        close_recorder window;
        window.setObjectName("target");
        window.resize(240, 120);
        window.confirm = true;
        window.show();

        // The confirmation stays open: the scenario fails at its deadline, and the test still ends.
        bool timed_out = false;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.close("target");
                timed_out = throws<std::runtime_error>([&]{actions.finish(std::chrono::milliseconds(200));});
            });

        CHECK(timed_out);
        CHECK(window.close_events == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_modal_dialog_before_closing_its_parent")
    {
        close_recorder window;
        window.setObjectName("target");
        window.resize(240, 120);
        window.show();

        // Qt would drop a close request sent while the dialog blocks the window.
        QDialog dialog(&window);
        dialog.setModal(true);
        dialog.show();
        QTimer::singleShot(300, &dialog, &QDialog::reject);
        window.closing_under_modal = [&dialog]{return dialog.isVisible();};

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.close("target");
                actions.finish();
            });

        CHECK(window.close_events == 1);
        CHECK_FALSE(window.closed_under_modal);
        CHECK_FALSE(window.isVisible());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sets_an_item_color_through_its_real_dialog")
    {
        QWidget window;
        window.resize(320, 180);

        QTreeWidget tree(&window);
        tree.setObjectName("tree");
        tree.setColumnCount(2);
        tree.setHeaderHidden(true);
        tree.setGeometry(10, 10, 300, 150);
        tree.setColumnWidth(0, 170);
        tree.setColumnWidth(1, 100);

        const auto make_color_button = [&window](QWidget* _parent)
                                       {
                                           auto* const button = new QPushButton(_parent);
                                           button->setProperty("color", QColor(10, 20, 30));
                                           QObject::connect(
                                               button,
                                               &QPushButton::clicked,
                                               [&window, button]
                {
                    QColorDialog dialog(button->property("color").value<QColor>(), &window);
                    dialog.setOption(QColorDialog::DontUseNativeDialog);
                    if(dialog.exec() == QDialog::Accepted)
                    {
                        button->setProperty("color", dialog.selectedColor());
                    }
                });
                                           return button;
                                       };

        auto* const group = new QTreeWidgetItem(&tree);
        group->setText(0, QStringLiteral("Group_0"));
        auto* const color_button = make_color_button(&tree);
        tree.setItemWidget(group, 1, color_button);

        // The same button, alone, as a setting shows it.
        auto* const setting = make_color_button(&window);
        setting->setObjectName("setting");
        setting->setGeometry(10, 165, 100, 12);
        window.resize(320, 190);
        window.show();

        const QColor expected_color(40, 80, 120);
        const QColor setting_color(200, 100, 0);
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.set(item("tree", "Group_0").column(1), expected_color);
                actions.set("setting", setting_color);
                actions.finish();
            });

        CHECK(color_button->property("color").value<QColor>() == expected_color);
        CHECK(setting->property("color").value<QColor>() == setting_color);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "selects_a_lazy_nested_item_through_its_branch_indicator")
    {
        QWidget window;
        window.resize(320, 200);

        QTreeWidget tree(&window);
        tree.setObjectName("tree");
        tree.setGeometry(20, 20, 260, 160);
        tree.setHeaderHidden(true);

        auto* const root = new QTreeWidgetItem(&tree, {QStringLiteral("Root")});
        new QTreeWidgetItem(root, {QStringLiteral("Loading")});
        QTreeWidgetItem* child = nullptr;
        window.show();

        int expansions = 0;
        QObject::connect(
            &tree,
            &QTreeWidget::expanded,
            [&tree, root, &child, &expansions](const QModelIndex& _index)
            {
                ++expansions;
                if(_index == tree.indexFromItem(root))
                {
                    // Simulate a lazy branch whose item arrives after it is expanded.
                    QTimer::singleShot(
                        20,
                        &tree,
                        [root, &child]
                    {
                        delete root->takeChild(0);
                        child = new QTreeWidgetItem(root, {QStringLiteral("Child")});
                    });
                }
            });
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.select(item("tree", "Root/Child"));
                actions.finish();
            });

        CHECK(root->isExpanded());
        CHECK_FALSE(root->isSelected());
        CHECK(tree.currentItem() == child);
        CHECK(tree.selectedItems() == QList<QTreeWidgetItem*> {child});
        CHECK(expansions == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "selects_below_the_repeated_parent_label_that_has_the_child")
    {
        QWidget window;
        window.resize(320, 200);

        QTreeWidget tree(&window);
        tree.setObjectName("tree");
        tree.setGeometry(20, 20, 260, 160);
        tree.setHeaderHidden(true);
        new QTreeWidgetItem(&tree, {QStringLiteral("Group")});
        auto* const group = new QTreeWidgetItem(&tree, {QStringLiteral("Group")});
        auto* const child = new QTreeWidgetItem(group, {QStringLiteral("Child")});
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.select(item("tree", "Group/Child"));
                actions.finish();
            });

        CHECK(group->isExpanded());
        CHECK(tree.currentItem() == child);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_an_item_path_below_a_leaf")
    {
        QWidget window;
        window.resize(320, 180);

        QTreeWidget tree(&window);
        tree.setObjectName("tree");
        tree.setGeometry(20, 20, 260, 140);
        tree.setHeaderHidden(true);
        new QTreeWidgetItem(&tree, {QStringLiteral("Leaf")});
        window.show();

        bool rejected = false;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                rejected = throws<std::invalid_argument>([&]{actions.select(item("tree", "Leaf/Child"));});

                actions.finish();
            });

        CHECK(rejected);
        CHECK(tree.selectedItems().empty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "selects_a_list_item_scrolled_out_of_view")
    {
        QWidget window;
        window.resize(320, 180);

        QListWidget list(&window);
        list.setObjectName("list");
        list.setGeometry(20, 20, 260, 140);
        for(int index = 0 ; index < 200 ; ++index)
        {
            list.addItem(QStringLiteral("Item %1").arg(index));
        }

        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.select(item("list", "Item 199"));
                actions.finish();
            });

        REQUIRE(list.currentItem() != nullptr);
        CHECK(list.currentItem()->text() == QStringLiteral("Item 199"));
        CHECK(list.currentItem()->isSelected());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "keeps_a_selected_item_without_clicking_it_again")
    {
        QWidget window;
        window.resize(320, 180);

        QListWidget list(&window);
        list.setObjectName("list");
        list.setGeometry(20, 20, 260, 140);
        list.addItems({QStringLiteral("Alpha"), QStringLiteral("Beta")});
        window.show();

        int clicks        = 0;
        int double_clicks = 0;
        QObject::connect(&list, &QListWidget::itemClicked, [&clicks](QListWidgetItem*){++clicks;});
        QObject::connect(&list, &QListWidget::itemDoubleClicked, [&double_clicks](QListWidgetItem*){++double_clicks;});
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                // A second click on the same row would come within the double-click interval.
                actions.select(item("list", "Beta"));
                actions.select(item("list", "Beta"));
                actions.finish();
            });

        CHECK(list.currentRow() == 1);
        CHECK(clicks == 1);
        CHECK(double_clicks == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_an_ambiguous_item_selection")
    {
        QWidget window;
        window.resize(320, 180);

        QTreeWidget tree(&window);
        tree.setObjectName("tree");
        tree.setGeometry(20, 20, 260, 140);
        tree.setHeaderHidden(true);
        new QTreeWidgetItem(&tree, {QStringLiteral("Duplicate")});
        new QTreeWidgetItem(&tree, {QStringLiteral("Duplicate")});
        window.show();

        bool rejected = false;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                rejected = throws<std::invalid_argument>([&]{actions.select(item("tree", "Duplicate"));});

                actions.finish();
            });

        CHECK(rejected);
        CHECK(tree.selectedItems().empty());
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_a_missing_combo_item_without_opening_the_popup")
    {
        combo_window combo;

        scenario actions(application);
        bool rejected = false;
        run_scenario(
            *this,
            [&]
            {
                rejected = throws<std::invalid_argument>([&]{actions.set("target", "Missing");});

                actions.finish();
            });

        CHECK(rejected);
        CHECK(combo.target.currentText() == QStringLiteral("Alpha"));
        CHECK(combo.popup.shows == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_unrepresentable_numeric_set_values")
    {
        QWidget window;
        window.resize(320, 180);

        QSlider slider(&window);
        slider.setObjectName("slider");
        slider.setGeometry(20, 20, 180, 30);
        slider.setRange(0, 100);

        QSpinBox integer(&window);
        integer.setObjectName("integer");
        integer.setGeometry(20, 60, 120, 30);
        integer.setRange(-10, 10);

        QDoubleSpinBox real(&window);
        real.setObjectName("real");
        real.setGeometry(20, 100, 120, 30);
        real.setRange(-10., 10.);
        real.setDecimals(2);
        window.show();

        scenario actions(application);
        bool rejected_slider_range   = false;
        bool rejected_integer_range  = false;
        bool rejected_real_precision = false;
        run_scenario(
            *this,
            [&]
            {
                rejected_slider_range = throws<std::invalid_argument>([&]{actions.set("slider", 101);});

                rejected_integer_range = throws<std::invalid_argument>([&]{actions.set("integer", 11);});

                rejected_real_precision = throws<std::invalid_argument>([&]{actions.set("real", 1.234);});

                actions.finish();
            });

        CHECK(rejected_slider_range);
        CHECK(rejected_integer_range);
        CHECK(rejected_real_precision);
        CHECK(slider.value() == 0);
        CHECK(integer.value() == 0);
        CHECK(real.value() == doctest::Approx(0.));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "retries_a_click_until_its_overlay_is_gone")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);

        QWidget overlay(&window);
        overlay.setGeometry(target.geometry());
        overlay.show();
        window.show();
        overlay.raise();

        QTimer::singleShot(20, &application, [&overlay]{overlay.hide();});

        int click_count = 0;
        QObject::connect(&target, &QPushButton::clicked, [&click_count]{++click_count;});
        scenario actions(application);

        run_scenario(
            *this,
            [&]
            {
                actions.click("target");
                actions.finish();
            });

        CHECK(click_count == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "continues_after_a_click_opens_a_modal_dialog")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton open_button("open", &window);
        open_button.setObjectName("open");
        open_button.setGeometry(20, 20, 120, 60);

        QDialog dialog(&window);
        dialog.setModal(true);
        QPushButton close_button("close", &dialog);
        close_button.setObjectName("close");
        close_button.setGeometry(20, 20, 120, 60);
        dialog.resize(180, 80);

        QObject::connect(&open_button, &QPushButton::clicked, [&dialog]{static_cast<void>(dialog.exec());});
        QObject::connect(&close_button, &QPushButton::clicked, [&dialog]{dialog.accept();});
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("open");
                actions.click("close");
                actions.finish();
            });

        CHECK(dialog.result() == QDialog::Accepted);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_a_missing_widget_at_the_deadline")
    {
        QWidget window;
        window.resize(240, 120);
        window.show();

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.click("missing", {}, std::chrono::milliseconds(50));});

                actions.finish();
            });

        CHECK(failure.find("no visible widget named 'missing'") != std::string::npos);
        CHECK(failure.find("hidden match") == std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_hidden_widget_matches_in_scope")
    {
        QWidget window;
        window.resize(320, 160);
        QWidget left(&window);
        left.setObjectName("left");
        left.setGeometry(0, 0, 150, 150);
        QWidget right(&window);
        right.setObjectName("right");
        right.setGeometry(160, 0, 150, 150);

        QPushButton hidden(&left);
        hidden.setObjectName("only");
        QWidget hidden_parent(&left);
        hidden_parent.setGeometry(0, 40, 100, 40);
        QPushButton hidden_by_parent(&hidden_parent);
        hidden_by_parent.setObjectName("only");
        QPushButton other_scope(&right);
        other_scope.setObjectName("only");
        QPushButton visible_other_scope(&right);
        visible_other_scope.setObjectName("only");
        visible_other_scope.setGeometry(0, 60, 100, 30);

        // The ordinal is assigned to visible matches, not to the hidden ones.
        QPushButton visible_role(&left);
        visible_role.setObjectName("role");
        visible_role.setGeometry(0, 90, 100, 30);
        QPushButton hidden_role1(&left);
        hidden_role1.setObjectName("role");
        QPushButton hidden_role2(&left);
        hidden_role2.setObjectName("role");

        window.show();
        hidden.hide();
        hidden_parent.hide();
        other_scope.hide();
        hidden_role1.hide();
        hidden_role2.hide();

        scenario actions(application);
        std::string click_failure;
        std::string read_failure;
        std::string expect_failure;
        std::string ordinal_failure;
        std::size_t all_hidden = 0;
        bool getter_called     = false;
        run_scenario(
            *this,
            [&]
            {
                if(!wait_for_exposure(*this, window, operation_deadline()))
                {
                    throw std::runtime_error("test window was not exposed");
                }

                all_hidden = gateway.call_until(
                    operation_deadline(),
                    []{return locator("only").count_hidden_widgets();});
                click_failure = timeout_with_hidden_matches(
                    [&actions](std::chrono::milliseconds _deadline){actions.click("left >> only", {}, _deadline);});
                read_failure = timeout_with_hidden_matches(
                    [&actions, &getter_called](std::chrono::milliseconds _deadline)
                {
                    static_cast<void>(actions.read(
                                          "left >> only",
                                          [&getter_called](const QWidget&){getter_called = true; return 1;},
                                          _deadline
                    ));
                });
                expect_failure = timeout_with_hidden_matches(
                    [&actions](std::chrono::milliseconds _deadline)
                {
                    actions.expect("left >> only").within(_deadline).enabled();
                });
                ordinal_failure = timeout_with_hidden_matches(
                    [&actions](std::chrono::milliseconds _deadline)
                {
                    actions.expect("left >> role#2").within(_deadline).enabled();
                });
            });

        CHECK(all_hidden == 3);
        CHECK(click_failure.find("no visible widget named 'left >> only'; 2 hidden matches") != std::string::npos);
        CHECK(read_failure.find("no visible widget named 'left >> only'; 2 hidden matches") != std::string::npos);
        CHECK(expect_failure.find("no visible widget named 'left >> only'; 2 hidden matches") != std::string::npos);
        CHECK(ordinal_failure.find("no visible widget named 'left >> role#2'; 2 hidden matches") != std::string::npos);
        CHECK_FALSE(getter_called);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_a_clipped_widget_and_finds_it_after_scrolling")
    {
        scroll_panel panel;
        panel.area.show();
        scenario actions(application);
        std::string failure;

        run_scenario(
            *this,
            [&]
            {
                if(!wait_for_exposure(*this, panel.area, operation_deadline()))
                {
                    throw std::runtime_error("test scroll area was not exposed");
                }

                failure = timeout_with_hidden_matches(
                    [&actions](std::chrono::milliseconds _deadline)
                {
                    actions.expect("hidden").within(_deadline).enabled();
                });
                // Once brought into view, the same locator must succeed without another name.
                actions.scroll_to("hidden");
                actions.expect("hidden").visible();
            });

        CHECK(failure.find("no visible widget named 'hidden'; 1 hidden match") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_scope_paths_for_ambiguous_widgets")
    {
        QWidget window;
        window.resize(320, 140);

        QWidget left_view(&window);
        left_view.setObjectName("left_view");
        left_view.setGeometry(0, 0, 150, 140);
        QPushButton left_button(&left_view);
        left_button.setObjectName("repeated/button");
        left_button.setGeometry(10, 10, 120, 40);

        QWidget right_view(&window);
        right_view.setObjectName("right_view");
        right_view.setGeometry(160, 0, 150, 140);
        QPushButton right_button(&right_view);
        right_button.setObjectName("repeated/button");
        right_button.setGeometry(10, 10, 120, 40);

        window.show();

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.click("repeated/button");});

                actions.finish();
            });

        CHECK(failure.find("several GUI objects named 'repeated/button'") != std::string::npos);
        CHECK(failure.find("left_view >> repeated/button") != std::string::npos);
        CHECK(failure.find("right_view >> repeated/button") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_scope_paths_for_ambiguous_actions")
    {
        QMainWindow window;

        QMenu* const left_menu = window.menuBar()->addMenu("Left");
        left_menu->setObjectName("left_menu");
        QAction* const left_action = left_menu->addAction("Run");
        left_action->setObjectName("run");

        QMenu* const right_menu = window.menuBar()->addMenu("Right");
        right_menu->setObjectName("right_menu");
        QAction* const right_action = right_menu->addAction("Run");
        right_action->setObjectName("run");

        window.show();

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.click("run");});

                actions.finish();
            });

        CHECK(failure.find("several GUI objects named 'run'") != std::string::npos);
        CHECK(failure.find("left_menu >> run") != std::string::npos);
        CHECK(failure.find("right_menu >> run") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "clicks_at_relative_and_pixel_positions")
    {
        QWidget window;
        window.resize(160, 100);

        position_recorder target(&window);
        target.setObjectName("target");
        target.setGeometry(10, 10, 100, 60);
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("target", at(0.5, 0.5));
                actions.click("target", px(10, 11));
                actions.finish();
            });

        REQUIRE(target.positions.size() == 2);
        CHECK(target.positions[0] == QPoint(49, 29));
        CHECK(target.positions[1] == QPoint(10, 11));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "clicks_a_toolbar_action")
    {
        core::runtime::config_t configuration;
        core::runtime::config_t toolbar_config;
        toolbar_config.put("<xmlattr>.uid", "toolbar");
        toolbar_config.put("<xmlattr>.type", "sight::module::ui::toolbar");

        core::runtime::config_t menu_item_config;
        menu_item_config.put("<xmlattr>.name", "run");
        toolbar_config.add_child("gui.layout.menu_item", menu_item_config);
        configuration.add_child("service", toolbar_config);
        core::runtime::config_t view_config;
        view_config.put("<xmlattr>.sid", "test_window");
        configuration.add_child("view", view_config);

        locator_catalog catalog;
        catalog.add_configuration("test_config", configuration);

        QMainWindow window;
        window.setObjectName("test_window");
        QToolBar toolbar(&window);
        window.addToolBar(&toolbar);

        QAction* const action = toolbar.addAction("run");
        action->setObjectName("toolbar/run");
        bool triggered = false;
        QObject::connect(action, &QAction::triggered, [&triggered]{triggered = true;});
        window.show();

        scenario actions(application, catalog);
        run_scenario(
            *this,
            [&]
            {
                actions.click("test_window >> toolbar/run");
                actions.finish();
            });

        CHECK(triggered);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_an_unknown_menu_item_from_the_catalog")
    {
        core::runtime::config_t configuration;
        core::runtime::config_t toolbar;
        toolbar.put("<xmlattr>.uid", "toolbar");
        toolbar.put("<xmlattr>.type", "sight::module::ui::toolbar");

        core::runtime::config_t menu_item;
        menu_item.put("<xmlattr>.name", "Run");
        toolbar.add_child("gui.layout.menu_item", menu_item);
        configuration.add_child("service", toolbar);

        locator_catalog catalog;
        catalog.add_configuration("test_config", configuration);
        scenario actions(application, catalog);

        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                try
                {
                    actions.click("toolbar/Rnu", {}, std::chrono::seconds(2));
                }
                catch(const std::invalid_argument& error)
                {
                    failure = error.what();
                }

                actions.finish();
            });

        CHECK(failure.find("unknown XML name 'toolbar/Rnu'") != std::string::npos);
        CHECK(failure.find("toolbar/Run") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "resolves_an_xml_name_by_what_is_launched")
    {
        const auto make_launcher =
            [](std::string_view _parent)
            {
                core::runtime::config_t launcher;
                launcher.put("<xmlattr>.type", "sight::app::config_launcher");
                launcher.put("config.<xmlattr>.id", "child");

                core::runtime::config_t parameter;
                parameter.put("<xmlattr>.name", "WID_PARENT");
                parameter.put("<xmlattr>.value", _parent);
                launcher.add_child("param", parameter);
                return launcher;
            };

        core::runtime::config_t parent;
        parent.add_child("service", make_launcher("left_view"));
        parent.add_child("service", make_launcher("right_view"));

        core::runtime::config_t child;
        core::runtime::config_t repeated_service;
        repeated_service.put("<xmlattr>.uid", "repeated_srv");
        repeated_service.put("<xmlattr>.type", "sight::module::ui::qt::widget");
        child.add_child("service", repeated_service);

        core::runtime::config_t views;
        views.put("<xmlattr>.uid", "views");
        views.put("<xmlattr>.type", "sight::module::ui::view");
        for(const auto* const wid : {"left_view", "right_view"})
        {
            core::runtime::config_t view;
            view.put("<xmlattr>.wid", wid);
            views.add_child("registry.view", view);
        }

        parent.add_child("service", views);

        locator_catalog catalog;
        catalog.add_configurations({{"parent", parent}, {"child", child}});

        // Two activities may each launch the configuration: only what runs tells whether a name is ambiguous.
        QWidget window;
        QWidget left_view(&window);
        left_view.setObjectName("left_view");
        QPushButton left("left", &left_view);
        left.setObjectName("repeated_srv");
        QWidget right_view(&window);
        right_view.setObjectName("right_view");
        right_view.move(200, 0);
        QPushButton right("right", &right_view);
        right.setObjectName("repeated_srv");
        right_view.hide();
        int left_clicks = 0;
        QObject::connect(&left, &QPushButton::clicked, [&left_clicks]{++left_clicks;});
        window.resize(400, 100);
        window.show();

        scenario actions(application, catalog);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                actions.click("repeated_srv");

                gateway.call_until(operation_deadline(), [&right_view]{right_view.show();});
                failure = timeout_after_observing(
                    [&](std::chrono::milliseconds _timeout)
                {
                    actions.click("repeated_srv", {}, _timeout);
                });
                actions.click("left_view >> repeated_srv");
            });

        CHECK(left_clicks == 2);
        CHECK(failure.find("several GUI objects named 'repeated_srv'") != std::string::npos);
        CHECK(failure.find("left_view") != std::string::npos);
        CHECK(failure.find("right_view") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "triggers_an_action_of_nested_menus_once_without_opening_them")
    {
        QMainWindow window;
        QMenu* const file_menu = window.menuBar()->addMenu("File");
        file_menu->menuAction()->setObjectName("menu_bar/File");
        QMenu* const nested_menu = file_menu->addMenu("More");
        nested_menu->menuAction()->setObjectName("menu_bar/File/More");
        QAction* const action = nested_menu->addAction("run");
        action->setObjectName("menu_bar/File/More/run");
        int triggers   = 0;
        int menu_shows = 0;
        QObject::connect(action, &QAction::triggered, [&triggers]{++triggers;});
        QObject::connect(file_menu, &QMenu::aboutToShow, [&menu_shows]{++menu_shows;});
        QObject::connect(nested_menu, &QMenu::aboutToShow, [&menu_shows]{++menu_shows;});
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("menu_bar/File/More/run");
                actions.finish();
            });

        // Triggered once through the Qt API, once the menu bar entry is reachable: no popup to time.
        CHECK(triggers == 1);
        CHECK(menu_shows == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "triggers_an_action_of_a_folded_toolbar_once")
    {
        // With room for it, the extension expands the toolbar, and a second click on it would fold it again.
        QMainWindow window;
        window.resize(240, 400);
        QToolBar toolbar(&window);
        toolbar.setToolButtonStyle(Qt::ToolButtonTextOnly);
        window.addToolBar(&toolbar);

        QAction* target_action = nullptr;
        for(int index = 0 ; index < 8 ; ++index)
        {
            target_action = toolbar.addAction(QStringLiteral("Toolbar action %1").arg(index));
        }

        target_action->setObjectName("toolbar/last");
        int triggers = 0;
        QObject::connect(target_action, &QAction::triggered, [&triggers]{++triggers;});
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("toolbar/last");
                actions.finish();
            });

        CHECK(triggers == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "triggers_an_action_of_a_toolbar_overflow_once")
    {
        // Too low for the toolbar to expand: its extension opens a menu.
        QMainWindow window;
        window.resize(240, 60);
        QToolBar toolbar(&window);
        toolbar.setToolButtonStyle(Qt::ToolButtonTextOnly);
        window.addToolBar(&toolbar);

        QAction* target_action = nullptr;
        for(int index = 0 ; index < 8 ; ++index)
        {
            target_action = toolbar.addAction(QStringLiteral("Toolbar action %1").arg(index));
        }

        REQUIRE(target_action != nullptr);
        target_action->setObjectName("toolbar/last");
        int triggers = 0;
        QObject::connect(target_action, &QAction::triggered, [&triggers]{++triggers;});
        window.show();

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("toolbar/last");
                actions.finish();
            });

        CHECK(triggers == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "chooses_an_item_of_a_context_menu_run_by_exec")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        target.setContextMenuPolicy(Qt::CustomContextMenu);
        window.show();

        // As an application does: exec() returns the chosen action.
        bool remove_chosen = false;
        int triggers       = 0;
        QObject::connect(
            &target,
            &QWidget::customContextMenuRequested,
            [&](const QPoint& _position)
            {
                QMenu menu(&target);
                QAction* const remove = menu.addAction("Remove");
                remove->setObjectName("context/remove");
                menu.addAction("Keep")->setObjectName("context/keep");
                QObject::connect(remove, &QAction::triggered, [&triggers]{++triggers;});
                remove_chosen = menu.exec(target.mapToGlobal(_position)) == remove;
            });

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.right_click("target");
                actions.click("context/remove");
                actions.finish();
            });

        CHECK(remove_chosen);
        CHECK(triggers == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "clicks_a_button_with_the_signals_of_a_user_click")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setCheckable(true);
        target.setGeometry(20, 20, 120, 60);
        QButtonGroup group;
        group.addButton(&target, 7);
        window.show();

        int presses          = 0;
        int releases         = 0;
        int clicks           = 0;
        int toggles          = 0;
        std::vector<int> ids = {};
        QObject::connect(&target, &QPushButton::pressed, [&presses]{++presses;});
        QObject::connect(&target, &QPushButton::released, [&releases]{++releases;});
        QObject::connect(&target, &QPushButton::clicked, [&clicks]{++clicks;});
        QObject::connect(&target, &QPushButton::toggled, [&toggles]{++toggles;});
        QObject::connect(&group, &QButtonGroup::idClicked, [&ids](int _id){ids.push_back(_id);});

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("target");
                actions.finish();
            });

        CHECK(presses == 1);
        CHECK(releases == 1);
        CHECK(clicks == 1);
        CHECK(toggles == 1);
        CHECK(target.isChecked());
        CHECK(ids == std::vector<int> {7});
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_covered_button_then_fails_without_clicking_it")
    {
        QWidget window;
        window.resize(240, 120);

        QPushButton target("target", &window);
        target.setObjectName("target");
        target.setGeometry(20, 20, 120, 60);
        QWidget overlay(&window);
        overlay.setObjectName("overlay");
        overlay.setGeometry(target.geometry());
        window.show();
        overlay.raise();

        int clicks = 0;
        QObject::connect(&target, &QPushButton::clicked, [&clicks]{++clicks;});
        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.click("target", {}, std::chrono::milliseconds(500));});
            });

        // The Qt API is only used where a user's click would land.
        INFO(failure);
        CHECK(failure.find("covered") != std::string::npos);
        CHECK(failure.find("overlay") != std::string::npos);
        CHECK(clicks == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_disabled_action_of_a_closed_menu_then_fails")
    {
        QMainWindow window;
        QMenu* const file_menu = window.menuBar()->addMenu("File");
        file_menu->menuAction()->setObjectName("menu_bar/File");
        QAction* const action = file_menu->addAction("run");
        action->setObjectName("menu_bar/File/run");
        action->setEnabled(false);
        int triggers = 0;
        QObject::connect(action, &QAction::triggered, [&triggers]{++triggers;});
        window.show();

        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.click("menu_bar/File/run", {}, std::chrono::milliseconds(500));});
            });

        INFO(failure);
        CHECK(failure.find("disabled") != std::string::npos);
        CHECK(triggers == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "selects_a_tree_item_with_the_signals_of_a_user_click")
    {
        QWidget window;
        window.resize(320, 240);

        QTreeWidget tree(&window);
        tree.setObjectName("tree");
        tree.setGeometry(10, 10, 300, 220);
        tree.setHeaderHidden(true);
        auto* const parent = new QTreeWidgetItem(&tree, {QStringLiteral("parent")});
        auto* const child  = new QTreeWidgetItem(parent, {QStringLiteral("child")});
        // A view given the focus makes its first item current: one is set first.
        tree.setCurrentItem(parent);
        window.show();

        int clicks     = 0;
        int currents   = 0;
        int expansions = 0;
        QObject::connect(
            &tree,
            &QTreeWidget::itemClicked,
            [&clicks, child](QTreeWidgetItem* _item, int)
            {
                clicks += _item == child ? 1 : 100;
            });
        QObject::connect(&tree, &QTreeWidget::currentItemChanged, [&currents]{++currents;});
        QObject::connect(&tree, &QTreeWidget::itemExpanded, [&expansions]{++expansions;});

        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.select(item("tree", "parent/child"));
                actions.finish();
            });

        CHECK(tree.currentItem() == child);
        CHECK(child->isSelected());
        CHECK(clicks == 1);
        CHECK(currents == 1);
        CHECK(expansions == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "types_into_a_line_edit_while_its_completer_popup_is_open")
    {
        QWidget window;
        window.resize(320, 140);

        QLineEdit target(&window);
        target.setObjectName("text");
        target.setGeometry(20, 20, 180, 30);
        QCompleter completer({QStringLiteral("alpha"), QStringLiteral("alpine"), QStringLiteral("beta")});
        target.setCompleter(&completer);
        window.show();

        bool popup_seen = false;
        QObject::connect(
            &target,
            &QLineEdit::textEdited,
            [&]{popup_seen = popup_seen || completer.popup()->isVisible();});
        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                failure = failure_of([&]{actions.type("text", "alpine");});

                gateway.call_until(operation_deadline(), [&]{completer.popup()->hide(); return true;});
            });

        // The keys go to the field itself: the completer's popup does not take them.
        INFO(failure);
        CHECK(failure.empty());
        CHECK(popup_seen);
        CHECK(target.text() == QStringLiteral("alpine"));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_invalid_input_parameters")
    {
        scenario actions(application);
        bool rejected_relative           = false;
        bool rejected_pixel              = false;
        bool rejected_drag_start         = false;
        bool rejected_non_finite_end     = false;
        bool rejected_zero_wheel         = false;
        bool rejected_large_wheel        = false;
        bool rejected_zero_pinch         = false;
        bool rejected_non_finite_pinch   = false;
        bool rejected_far_drag_end       = false;
        bool rejected_large_pinch        = false;
        bool rejected_invalid_utf8       = false;
        bool rejected_empty_key_sequence = false;
        bool rejected_unknown_key        = false;

        run_scenario(
            *this,
            [&]
            {
                try
                {
                    actions.click("unused", at(1.1, 0.5), std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_relative = true;
                }

                try
                {
                    actions.click("unused", px(-1, 0), std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_pixel = true;
                }

                try
                {
                    actions.drag("unused", at(1.1, 0.5), at(0.5, 0.5), std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_drag_start = true;
                }

                try
                {
                    actions.drag(
                        "unused",
                        at(0.5, 0.5),
                        at(std::numeric_limits<double>::infinity(), 0.5),
                        std::chrono::seconds(1)
                    );
                }
                catch(const std::invalid_argument&)
                {
                    rejected_non_finite_end = true;
                }

                try
                {
                    actions.wheel("unused", 0, std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_zero_wheel = true;
                }

                try
                {
                    actions.wheel("unused", 256, std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_large_wheel = true;
                }

                try
                {
                    actions.pinch("unused", 0., std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_zero_pinch = true;
                }

                try
                {
                    actions.pinch(
                        "unused",
                        std::numeric_limits<double>::quiet_NaN(),
                        std::chrono::seconds(1)
                    );
                }
                catch(const std::invalid_argument&)
                {
                    rejected_non_finite_pinch = true;
                }

                try
                {
                    actions.drag("unused", at(0.5, 0.5), at(11., 0.5), std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_far_drag_end = true;
                }

                try
                {
                    actions.pinch("unused", 11., std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_large_pinch = true;
                }

                try
                {
                    actions.type("unused", std::string("\xC3", 1), std::chrono::seconds(1));
                }
                catch(const std::invalid_argument&)
                {
                    rejected_invalid_utf8 = true;
                }

                rejected_empty_key_sequence = throws<std::invalid_argument>([&]{actions.press("unused", "");});

                rejected_unknown_key = throws<std::invalid_argument>([&]{actions.press("unused", "Ctrl+");});

                actions.finish(std::chrono::seconds(1));
            });

        CHECK(rejected_relative);
        CHECK(rejected_pixel);
        CHECK(rejected_drag_start);
        CHECK(rejected_non_finite_end);
        CHECK(rejected_zero_wheel);
        CHECK(rejected_large_wheel);
        CHECK(rejected_zero_pinch);
        CHECK(rejected_non_finite_pinch);
        CHECK(rejected_far_drag_end);
        CHECK(rejected_large_pinch);
        CHECK(rejected_invalid_utf8);
        CHECK(rejected_empty_key_sequence);
        CHECK(rejected_unknown_key);
    }
}

} // namespace sight::ui::test::detail

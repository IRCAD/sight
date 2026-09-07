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

#include <ui/test/gui_fixture.hpp>

#include <doctest/doctest.h>

#include <QApplication>
#include <QFontMetrics>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedWidget>

#include <ui/qt/widget/notification_label.hpp>

#include <core/com/slot.hpp>
#include <core/notification/error.hpp>
#include <core/notification/information.hpp>
#include <core/notification/instruction.hpp>
#include <core/notification/monitor.hpp>
#include <core/notification/warning.hpp>
#include <core/thread/worker.hpp>
#include <utest/wait.hpp>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <thread>

//------------------------------------------------------------------------------

/**
 * @brief Build a notification that stays displayed until the test closes it.
 *
 * An information or a warning is displayed for the zone's information_duration / warning_duration, three
 * seconds by default, after which a timer closes its page. A test asserting on such a page would then race
 * against that timer: the widget may already be gone by the time the assertion runs, on a machine that is
 * loaded or stepped through in a debugger. A duration of zero means "no timeout at all", which takes the wall
 * clock out of the picture for everything but the tests that check the auto-dismissal itself.
 */
template<class N>
static std::shared_ptr<N> make_permanent(
    const std::string& _text,
    std::optional<std::filesystem::path> _icon = std::nullopt
)
{
    return std::make_shared<N>(
        sight::core::notification::message::params {
        .text     = _text,
        .icon     = std::move(_icon),
        .duration = std::chrono::milliseconds::zero()
    });
}

//------------------------------------------------------------------------------

static void send_mouse_release(QWidget* _widget)
{
    QMouseEvent mouse_event(
        QEvent::MouseButtonRelease,
        QPointF(0, 0),
        QPointF(0, 0),
        Qt::LeftButton,
        Qt::LeftButton,
        Qt::NoModifier
    );

    qApp->sendEvent(_widget, &mouse_event);
    qApp->processEvents();
}

/// The icon a notification label paints inside its own frame, null when it has none.
static QIcon icon_of(const QPointer<QLabel>& _label)
{
    const auto* const label = qobject_cast<sight::ui::qt::widget::notification_label*>(_label.data());

    return label != nullptr ? label->icon() : QIcon();
}

//------------------------------------------------------------------------------

namespace
{

class dummy_job : public sight::core::notification::monitor
{
public:

    explicit dummy_job(const std::string& _name, std::uint64_t _total_work_unit = 100) :
        monitor(_name)
    {
        set_total_work_units(_total_work_unit);
        set_state(sight::core::notification::monitor::running);
    }

    ~dummy_job() override
    {
        set_state(sight::core::notification::monitor::finished);
    }

    using sight::core::notification::monitor::done;
    using sight::core::notification::monitor::finish;
};

/// Neither a monitor nor a message: exercises the "unsupported type" branch of add_notification().
class unsupported_notification final : public sight::core::notification::base
{
};

} // namespace

TEST_SUITE("sight::module::ui::qt::notification_zone")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "basic")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            CHECK(!stacked_widget.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_priority_ordering")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            CHECK(!stacked_widget.isNull());

            // Add in increasing priority order: each one should become the new top.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::instruction>("", "instruction text")
                    )
                );
            }).get();

            const auto instruction_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_instruction");
            CHECK(!instruction_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, instruction_label]
            {
                CHECK_EQ(stacked_widget->currentWidget(), instruction_label->parentWidget());
            }).get();

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::information>("information text")
                    )
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            CHECK(!icon_of(information_label).isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, information_label]
            {
                CHECK_EQ(stacked_widget->currentWidget(), information_label->parentWidget());
            }).get();

            auto job = std::make_shared<dummy_job>("dummy");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, job]
            {
                _service->slot("add_notification")->run(std::static_pointer_cast<sight::core::notification::base>(job));
            }).get();

            const auto progress_page = sight::ui::test::gui_fixture::find_widget("progress_page");
            CHECK(!progress_page.isNull());
            CHECK_EQ(stacked_widget->currentWidget(), progress_page.data());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::warning>("warning text")
                    )
                );
            }).get();

            const auto warning_label = sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_warning");
            CHECK(!warning_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, warning_label]
            {
                CHECK_EQ(stacked_widget->currentWidget(), warning_label->parentWidget());
            }).get();

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::error>("", "error text")
                    )
                );
            }).get();

            const auto error_label = sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_error");
            CHECK(!error_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, error_label]
            {
                CHECK_EQ(stacked_widget->currentWidget(), error_label->parentWidget());
            }).get();

            // All previous pages must still be present (stacking, not replacing).
            sight::core::thread::get_default_worker()->post_task<void>(
                [instruction_label, information_label, warning_label]
            {
                CHECK(!instruction_label.isNull());
                CHECK(!information_label.isNull());
                CHECK(!warning_label.isNull());
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_instruction_is_singleton")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            CHECK(!stacked_widget.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::instruction>("", "first")
                    )
                );
            }).get();

            const auto instruction_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_instruction");
            CHECK(!instruction_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, instruction_label]
            {
                CHECK_EQ(instruction_label->text().toStdString(), std::string("first"));
                CHECK_EQ(stacked_widget->count(), 1);
            }).get();

            const auto* const first_widget = instruction_label->parentWidget();

            // A second instruction must replace the first one in place: same widget, updated text.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::instruction>("", "second")
                    )
                );
            }).get();

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, instruction_label, first_widget]
            {
                CHECK(!instruction_label.isNull());
                CHECK_EQ(instruction_label->text().toStdString(), std::string("second"));
                CHECK_EQ(instruction_label->parentWidget(), first_widget);
                CHECK_EQ(stacked_widget->count(), 1);
            }).get();

            // An empty instruction closes it.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::instruction>()
                    )
                );
            }).get();

            SIGHT_TEST_FAIL_WAIT(instruction_label.isNull());
            CHECK_EQ(stacked_widget->count(), 0);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_error_must_be_acknowledged")
    {
        struct signal_tester
        {
            std::atomic_bool error_emitted {false};
            std::atomic_bool error_acknowledged_emitted {false};

            //------------------------------------------------------------------------------

            void on_error(sight::core::notification::base::sptr _notification)
            {
                error_emitted = std::dynamic_pointer_cast<sight::core::notification::error>(_notification) != nullptr;
            }

            //------------------------------------------------------------------------------

            void on_error_acknowledged(sight::core::notification::base::sptr _notification)
            {
                error_acknowledged_emitted =
                    std::dynamic_pointer_cast<sight::core::notification::error>(_notification) != nullptr;
            }
        } signal_tester;

        test_service(
            "sight::module::ui::qt::notification_zone",
            [&signal_tester](const sight::service::base::sptr& _service)
        {
            const auto error_slot = sight::core::com::new_slot(&signal_tester::on_error, &signal_tester);
            error_slot->set_worker(sight::core::thread::get_default_worker());

            const auto error_acknowledged_slot =
                sight::core::com::new_slot(&signal_tester::on_error_acknowledged, &signal_tester);
            error_acknowledged_slot->set_worker(sight::core::thread::get_default_worker());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, error_slot, error_acknowledged_slot]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();

                _service->signal("error")->connect(error_slot);
                _service->signal("error_acknowledged")->connect(error_acknowledged_slot);
            }).get();

            CHECK(_service->started());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::information>("information text")
                    )
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::error>("", "error text")
                    )
                );
            }).get();

            SIGHT_TEST_FAIL_WAIT(signal_tester.error_emitted);

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            const auto error_label = sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_error");
            CHECK(!error_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, error_label]
            {
                CHECK_EQ(error_label->text().toStdString(), std::string("error text"));
                CHECK_EQ(stacked_widget->currentWidget(), error_label->parentWidget());
            }).get();

            sight::core::thread::get_default_worker()->post_task<void>(
                [error_label]
            {
                send_mouse_release(error_label);
            }).get();

            SIGHT_TEST_FAIL_WAIT(signal_tester.error_acknowledged_emitted);

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget, information_label, error_label]
            {
                CHECK(!information_label.isNull());
                CHECK(error_label.isNull());

                const auto* const current_page = stacked_widget->currentWidget();
                CHECK(current_page == information_label->parentWidget());
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_progress_acknowledge")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            auto job = std::make_shared<dummy_job>("dummy");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, job]
            {
                _service->slot("add_notification")->run(std::static_pointer_cast<sight::core::notification::base>(job));
            }).get();

            const auto progress_page = sight::ui::test::gui_fixture::find_widget("progress_page");
            CHECK(!progress_page.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [progress_page]
            {
                send_mouse_release(progress_page);
            }).get();

            CHECK(!progress_page.isNull());

            // finish() synchronously flips the job's state, but the auto-close driven by
            // set_finished_callback()/on_notification_finished() is only delivered as a task posted on the
            // same default worker (see notifications_store::watch()). Calling finish() and clicking within
            // the very same task guarantees the click is processed first, so it is the acknowledger's own
            // click-to-close (not the auto-close) that closes the page here.
            sight::core::thread::get_default_worker()->post_task<void>(
                [job, progress_page]
            {
                job->done();
                job->finish();
                send_mouse_release(progress_page);
            }).get();

            SIGHT_TEST_FAIL_WAIT(progress_page.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_progress_auto_closes_once_finished")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            auto job = std::make_shared<dummy_job>("dummy");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, job]
            {
                _service->slot("add_notification")->run(std::static_pointer_cast<sight::core::notification::base>(job));
            }).get();

            const auto progress_page = sight::ui::test::gui_fixture::find_widget("progress_page");
            CHECK(!progress_page.isNull());

            // Without any click, finishing the job must still close the page on its own
            // (set_finished_callback() path in create_page()).
            job->done();
            job->finish();

            SIGHT_TEST_FAIL_WAIT(progress_page.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_progress_auto_closes_when_job_is_destroyed")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            auto job = std::make_shared<dummy_job>("dummy");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, job]
            {
                _service->slot("add_notification")->run(std::static_pointer_cast<sight::core::notification::base>(job));
            }).get();

            const auto progress_page = sight::ui::test::gui_fixture::find_widget("progress_page");
            CHECK(!progress_page.isNull());

            // Real callers never call finish(): they just drop their sptr and let the destructor do it
            // (see has_monitors::observe()). notification_zone must not hold its own ref, or this never fires.
            job.reset();

            SIGHT_TEST_FAIL_WAIT(progress_page.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_monitor_is_kept_for_backward_compatibility")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            auto job = std::make_shared<dummy_job>("dummy");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, job]
            {
                _service->slot("add_monitor")->run(std::static_pointer_cast<sight::core::notification::base>(job));
            }).get();

            const auto progress_page = sight::ui::test::gui_fixture::find_widget("progress_page");
            CHECK(!progress_page.isNull());

            job->done();
            job->finish();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_finish_closes_page")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            // The emitter keeps the sptr, like observe()/aggregate() callers already do for monitors, and
            // uses it later to close its own notification's widget. The notification is permanent, so the page
            // can only be closed by finish() below, however long this test happens to take.
            const auto information = make_permanent<sight::core::notification::information>("long lived");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            // Simulate the emitting service closing its own notification's widget from another thread.
            information->finish();

            SIGHT_TEST_FAIL_WAIT(information_label.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_set_text_updates_label")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto information = make_permanent<sight::core::notification::information>("before");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [information_label]
            {
                CHECK_EQ(information_label->text().toStdString(), std::string("before"));
            }).get();

            // Simulate the emitting service updating its own notification's displayed text after the fact.
            information->set_text("after");

            SIGHT_TEST_FAIL_WAIT(information_label->text().toStdString() == std::string("after"));
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_ignores_unsupported_type")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            CHECK(!stacked_widget.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                CHECK_NOTHROW(
                    _service->slot("add_notification")->run(
                        std::static_pointer_cast<sight::core::notification::base>(
                            std::make_shared<unsupported_notification>()
                        )
                    )
                );
            }).get();

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget]
            {
                CHECK_EQ(stacked_widget->count(), 0);
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_monitor_ignores_non_monitor_and_warns")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            CHECK(!stacked_widget.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                CHECK_NOTHROW(
                    _service->slot("add_monitor")->run(
                        std::static_pointer_cast<sight::core::notification::base>(
                            std::make_shared<sight::core::notification::information>("", "not a monitor")
                        )
                    )
                );
            }).get();

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget]
            {
                CHECK_EQ(stacked_widget->count(), 0);
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_second_monitor_merges_into_existing_page")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            CHECK(!stacked_widget.isNull());

            auto job1 = std::make_shared<dummy_job>("job1");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, job1]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        job1
                    )
                );
            }).get();

            const auto progress_page = sight::ui::test::gui_fixture::find_widget("progress_page");
            CHECK(!progress_page.isNull());

            auto job2 = std::make_shared<dummy_job>("job2");

            // A second monitor notification must merge into the existing progress page instead of creating
            // a new one.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, job2]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        job2
                    )
                );
            }).get();

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget]
            {
                CHECK_EQ(stacked_widget->count(), 1);
            }).get();

            job1->done();
            job1->finish();
            job2->done();
            job2->finish();

            SIGHT_TEST_FAIL_WAIT(progress_page.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "updating_and_swapping_are_no_ops")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            CHECK_NOTHROW(_service->update().wait());
            CHECK(_service->started());

            CHECK_NOTHROW(_service->swap_key("unused_key", nullptr).wait());
            CHECK(_service->started());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_auto_closes_after_custom_duration")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            // Explicit duration on the notification itself overrides the type's default duration
            // (resolve_duration_ms()) and drives the QTimer-based auto-dismiss in handle_message().
            const auto information = std::make_shared<sight::core::notification::information>(
                sight::core::notification::message::params {
                .text     = "short lived",
                .duration = std::chrono::milliseconds(50)
            });

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            SIGHT_TEST_FAIL_WAIT(information_label.isNull());
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_reappears_after_timeout")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto information = std::make_shared<sight::core::notification::information>(
                sight::core::notification::message::params {
                .text     = "reappearing",
                .duration = std::chrono::milliseconds(50)
            });

            const auto add_notification = [_service, information]
                                          {
                                              _service->slot("add_notification")->run(
                                                  std::static_pointer_cast<sight::core::notification::base>(information)
                                              );
                                          };

            sight::core::thread::get_default_worker()->post_task<void>(add_notification).get();

            auto information_label = sight::ui::test::gui_fixture::find_widget<QLabel>(
                "notification_label_information"
            );
            CHECK(!information_label.isNull());

            SIGHT_TEST_FAIL_WAIT(information_label.isNull());

            // A later announcement recreates the page after its previous timeout.
            sight::core::thread::get_default_worker()->post_task<void>(add_notification).get();

            information_label = sight::ui::test::gui_fixture::find_widget<QLabel>(
                "notification_label_information"
            );
            CHECK(!information_label.isNull());
            CHECK_EQ(information_label->text().toStdString(), "reappearing");
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_reappears_when_updated_after_timeout")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            // As notification_composer does with a merged notification: announced once, then only updated.
            const auto information = std::make_shared<sight::core::notification::information>(
                sight::core::notification::message::params {
                .text     = "first contribution",
                .duration = std::chrono::milliseconds(50)
            });

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            auto information_label = sight::ui::test::gui_fixture::find_widget<QLabel>(
                "notification_label_information"
            );
            CHECK(!information_label.isNull());

            SIGHT_TEST_FAIL_WAIT(information_label.isNull());

            // The update alone displays it again, with its delay restarted.
            information->set_text("second contribution");

            SIGHT_TEST_FAIL_WAIT(
                !sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information").isNull()
            );

            information_label = sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            REQUIRE(!information_label.isNull());
            CHECK_EQ(information_label->text().toStdString(), "second contribution");

            SIGHT_TEST_FAIL_WAIT(information_label.isNull());
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_with_sound_does_not_throw")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto error                                      = std::make_shared<sight::core::notification::error>(
                sight::core::notification::message::params {.text = "with sound", .sound = true
                });

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, error]
            {
                CHECK_NOTHROW(
                    _service->slot("add_notification")->run(
                        std::static_pointer_cast<sight::core::notification::base>(error)
                    )
                );
            }).get();

            const auto error_label = sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_error");
            CHECK(!error_label.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_without_icon_hides_the_icon_label")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            // An empty path means "no icon", as opposed to an unset one which takes the type's default.
            const auto information = make_permanent<sight::core::notification::information>(
                "no icon",
                std::filesystem::path()
            );

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            CHECK(icon_of(information_label).isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_with_a_zero_icon_size_follows_the_text")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                // A size of 0 draws the icons a text line high, which is also the default.
                config.add("properties.<xmlattr>.notification_icon_size", "0");

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::information>("zero sized icon")
                    )
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            REQUIRE(!information_label.isNull());
            CHECK(!icon_of(information_label).isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [information_label]
            {
                const auto* const label =
                    qobject_cast<sight::ui::qt::widget::notification_label*>(information_label.data());
                REQUIRE(label != nullptr);
                CHECK_EQ(label->icon_side(), QFontMetrics(label->font()).height());
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_updates_its_page_instead_of_stacking")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto information = make_permanent<sight::core::notification::information>("before");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");
            CHECK(!stacked_widget.isNull());

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget]
            {
                CHECK_EQ(stacked_widget->count(), 1);
            }).get();

            // The very same notification handed over a second time, as a service re-announcing the state it
            // just updated would: it takes the page it already has, rather than a second one.
            information->set_text("after");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            SIGHT_TEST_FAIL_WAIT(information_label->text().toStdString() == std::string("after"));

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget]
            {
                CHECK_EQ(stacked_widget->count(), 1);
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_instruction_set_text_updates_label")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto instruction = std::make_shared<sight::core::notification::instruction>("", "before");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, instruction]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(instruction)
                );
            }).get();

            const auto instruction_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_instruction");
            CHECK(!instruction_label.isNull());

            // An update is dispatched by type too: the instruction keeps its singleton page, it does not get
            // a message page of its own.
            instruction->set_text("after");

            SIGHT_TEST_FAIL_WAIT(instruction_label->text().toStdString() == std::string("after"));

            const auto stacked_widget =
                sight::ui::test::gui_fixture::find_widget<QStackedWidget>("notification_stack");

            sight::core::thread::get_default_worker()->post_task<void>(
                [stacked_widget]
            {
                CHECK_EQ(stacked_widget->count(), 1);
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_update_restarts_the_duration")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto information = std::make_shared<sight::core::notification::information>(
                sight::core::notification::message::params {
                .text     = "tick 0",
                .duration = std::chrono::milliseconds(1000)
            });

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, information]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(information)
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

            // Updated every 400ms, it outlives its own 1000ms delay: each update starts it over.
            for(int tick = 1 ; tick <= 3 ; ++tick)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(400));

                information->set_text("tick " + std::to_string(tick));

                SIGHT_TEST_FAIL_WAIT(information_label->text().toStdString() == "tick " + std::to_string(tick));
            }

            // Left alone, it closes on the delay restarted by the last update.
            SIGHT_TEST_FAIL_WAIT(information_label.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_message_with_a_zero_duration_never_closes")
    {
        test_service(
            "sight::module::ui::qt::notification_zone",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t config;

                // Shortened so that the control notification below closes without slowing the test down.
                config.add("properties.<xmlattr>.information_duration", "50");

                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto transient = std::make_shared<sight::core::notification::information>("", "transient");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, transient]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(transient)
                );
            }).get();

            // Without a duration of its own, it is dismissed on the zone's, shortened above.
            const auto transient_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!transient_label.isNull());
            SIGHT_TEST_FAIL_WAIT(transient_label.isNull());

            // A duration of 0, as a merged notification of sight::module::ui::notification_composer carries,
            // opts out of that timeout: only finish()/cancel() closes it.
            const auto pinned = std::make_shared<sight::core::notification::information>(
                sight::core::notification::message::params {
                .text     = "pinned",
                .duration = std::chrono::milliseconds::zero()
            });

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, pinned]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(pinned)
                );
            }).get();

            const auto pinned_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!pinned_label.isNull());

            std::this_thread::sleep_for(std::chrono::milliseconds(300));

            CHECK(!pinned_label.isNull());

            pinned->finish();

            SIGHT_TEST_FAIL_WAIT(pinned_label.isNull());
        });
    }
}

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
#include <QLabel>
#include <QMouseEvent>
#include <QStackedWidget>

#include <core/com/slot.hpp>
#include <core/notification/error.hpp>
#include <core/notification/information.hpp>
#include <core/notification/instruction.hpp>
#include <core/notification/monitor.hpp>
#include <core/notification/warning.hpp>
#include <core/thread/worker.hpp>
#include <utest/wait.hpp>

#include <atomic>

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
                        std::make_shared<sight::core::notification::information>("", "information text")
                    )
                );
            }).get();

            const auto information_label =
                sight::ui::test::gui_fixture::find_widget<QLabel>("notification_label_information");
            CHECK(!information_label.isNull());

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
                        std::make_shared<sight::core::notification::warning>("", "warning text")
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
                        std::make_shared<sight::core::notification::information>("", "information text")
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
            // uses it later to close its own notification's widget - the default information_duration
            // (3000ms) is far longer than this test, so the page can only be closed by finish() below.
            const auto information = std::make_shared<sight::core::notification::information>("", "long lived");

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

            const auto information = std::make_shared<sight::core::notification::information>("", "before");

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
                "",
                "short lived",
                std::filesystem::path {},
                "",
                false,
                nullptr,
                std::chrono::milliseconds(50)
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

            SIGHT_TEST_FAIL_WAIT(information_label.isNull());
        });
    }

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

            const auto error = std::make_shared<sight::core::notification::error>(
                "",
                "with sound",
                std::filesystem::path {},
                "",
                false,
                nullptr,
                std::nullopt,
                true
            );

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
}

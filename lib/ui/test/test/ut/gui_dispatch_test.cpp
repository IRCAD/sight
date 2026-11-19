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

#include "gui_test_support.hpp"

#include <ui/test/detail/gui_dispatch.hpp>

#include <doctest/doctest.h>

#include <QDialog>
#include <QMenu>
#include <QMetaObject>
#include <QPoint>
#include <QString>
#include <QWidget>

#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

namespace sight::ui::test::detail
{

TEST_SUITE("sight::ui::test::gui_dispatch")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "lets_a_test_condition_fail_when_the_gui_is_busy_at_its_deadline")
    {
        std::optional<bool> held;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
                QMetaObject::invokeMethod(
                    &application,
                    [deadline]{std::this_thread::sleep_until(deadline + std::chrono::seconds(1));},
                    Qt::QueuedConnection
                );
                held = wait_for(*this, deadline, []{return false;});
            });

        // A condition that never held, not a timeout of the test helper.
        CHECK(held == false);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "acknowledges_completed_callbacks")
    {
        bool callback_ran = false;
        dispatch_ack acknowledgement {};

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                const auto ticket   = dispatcher.post_until(deadline, [&]{callback_ran = true;});
                acknowledgement     = dispatcher.acknowledge_until(ticket, deadline);
            });

        CHECK(callback_ran);
        CHECK(acknowledgement == dispatch_ack::returned);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "acknowledges_a_modal_dialog_without_waiting_for_its_handler")
    {
        QDialog dialog;
        dialog.setModal(true);
        dispatch_ack modal_acknowledgement {};
        dispatch_ack close_acknowledgement {};
        bool callback_returned = false;

        run_scenario(
            *this,
            [&]
            {
                const auto deadline   = operation_deadline();
                const auto ticket     = dispatcher.post_until(deadline, [&]{dialog.exec();});
                modal_acknowledgement = dispatcher.acknowledge_until(ticket, deadline);

                const auto close_ticket = dispatcher.post_until(deadline, [&]{dialog.accept();});
                close_acknowledgement   = dispatcher.acknowledge_until(close_ticket, deadline);
                dispatcher.wait_returned_until(ticket, deadline);
                callback_returned = true;
            });

        CHECK(modal_acknowledgement == dispatch_ack::modal_opened);
        CHECK(close_acknowledgement == dispatch_ack::returned);
        CHECK(callback_returned);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "acknowledges_a_popup_menu_without_waiting_for_its_handler")
    {
        QWidget window;
        window.setObjectName("test_window");
        window.resize(240, 120);
        window.show();

        QMenu menu(&window);
        menu.setObjectName("context_menu");
        menu.addAction(QStringLiteral("item"));
        dispatch_ack menu_acknowledgement {};
        dispatch_ack close_acknowledgement {};
        bool callback_returned = false;

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
                    menu.exec(window.mapToGlobal(QPoint(20, 20)));
                });
                menu_acknowledgement = dispatcher.acknowledge_until(ticket, deadline);

                const auto close_ticket = dispatcher.post_until(deadline, [&]{menu.close();});
                close_acknowledgement   = dispatcher.acknowledge_until(close_ticket, deadline);
                dispatcher.wait_returned_until(ticket, deadline);
                callback_returned = true;
            });

        CHECK(menu_acknowledgement == dispatch_ack::popup_opened);
        CHECK(close_acknowledgement == dispatch_ack::returned);
        CHECK(callback_returned);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "transports_an_exception_raised_after_a_modal_dialog")
    {
        QDialog dialog;
        dialog.setModal(true);
        std::string failure;

        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                const auto ticket   = dispatcher.post_until(
                    deadline,
                    [&]
                {
                    dialog.exec();
                    throw std::runtime_error("failed after the dialog");
                });
                REQUIRE(dispatcher.acknowledge_until(ticket, deadline) == dispatch_ack::modal_opened);

                const auto close_ticket = dispatcher.post_until(deadline, [&]{dialog.accept();});
                static_cast<void>(dispatcher.acknowledge_until(close_ticket, deadline));
                failure = failure_of([&]{dispatcher.wait_returned_until(ticket, deadline);});
            });

        CHECK(failure == "failed after the dialog");
    }
}

} // namespace sight::ui::test::detail

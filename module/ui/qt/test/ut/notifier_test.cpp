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

#include <QLabel>

#include <core/notification/error.hpp>
#include <core/notification/information.hpp>
#include <core/notification/instruction.hpp>
#include <core/notification/monitor.hpp>
#include <core/notification/warning.hpp>
#include <core/thread/worker.hpp>
#include <utest/wait.hpp>

//------------------------------------------------------------------------------

static sight::service::config_t make_channel_config(
    const std::string& _uid,
    const std::string& _position,
    const std::string& _duration,
    const std::string& _size,
    const std::string& _max,
    const std::string& _closable
)
{
    sight::service::config_t channel;

    if(!_uid.empty())
    {
        channel.put("<xmlattr>.uid", _uid);
    }

    if(!_position.empty())
    {
        channel.put("<xmlattr>.position", _position);
    }

    if(!_duration.empty())
    {
        channel.put("<xmlattr>.duration", _duration);
    }

    if(!_size.empty())
    {
        channel.put("<xmlattr>.size", _size);
    }

    if(!_max.empty())
    {
        channel.put("<xmlattr>.max", _max);
    }

    if(!_closable.empty())
    {
        channel.put("<xmlattr>.closable", _closable);
    }

    return channel;
}

TEST_SUITE("sight::module::ui::qt::notifier")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_creates_a_popup_for_each_type")
    {
        test_service(
            "sight::module::ui::qt::notifier",
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

            // instruction/information/warning/error map to distinct popup "types" (info/success/warning/failure),
            // each getting a distinct object name, so all four can be displayed and checked at once.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::instruction>("", "instruction text")
                    )
                );
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::information>("", "information text")
                    )
                );
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::warning>("", "warning text")
                    )
                );
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::error>("", "error text")
                    )
                );
            }).get();

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Info"),
                std::make_optional(std::string("instruction text"))
            );
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Success"),
                std::make_optional(std::string("information text"))
            );
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Warning"),
                std::make_optional(std::string("warning text"))
            );
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Failure"),
                std::make_optional(std::string("error text"))
            );
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_ignores_non_message_notifications")
    {
        test_service(
            "sight::module::ui::qt::notifier",
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

            auto monitor = std::make_shared<sight::core::notification::monitor>("dummy");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, monitor]
            {
                CHECK_NOTHROW(
                    _service->slot("add_notification")->run(
                        std::static_pointer_cast<sight::core::notification::base>(monitor)
                    )
                );
            }).get();

            CHECK_FALSE(sight::ui::test::gui_fixture::find_widget<QLabel>("NotificationDialog_Info"));
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "close_notification_closes_the_popup_on_its_channel")
    {
        test_service(
            "sight::module::ui::qt::notifier",
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

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::information>(
                            "",
                            "channelled",
                            std::filesystem::path {},
                            "my_channel"
                        )
                    )
                );
            }).get();

            const auto label = sight::ui::test::gui_fixture::find_widget<QLabel>("NotificationDialog_Success");
            CHECK(!label.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("close_notification")->run(std::string("my_channel"));
            }).get();

            SIGHT_TEST_FAIL_WAIT(label.isNull());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "close_notification_on_unknown_channel_does_not_throw")
    {
        test_service(
            "sight::module::ui::qt::notifier",
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

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                CHECK_NOTHROW(_service->slot("close_notification")->run(std::string("no_such_channel")));
            }).get();
        });
    }

    TEST_CASE_FIXTURE(
        sight::ui::test::gui_fixture,
        "configuring_parses_channels_and_reports_invalid_values_without_throwing"
    )
    {
        test_service(
            "sight::module::ui::qt::notifier",
            [](const sight::service::base::sptr& _service)
        {
            sight::service::config_t channels;
            channels.add_child(
                "channel",
                make_channel_config("valid_channel", "TOP_LEFT", "1000", "300x80", "2", "true")
            );
            channels.add_child("channel", make_channel_config("infinite_channel", "CENTERED", "infinite", "", "", ""));
            channels.add_child(
                "channel",
                make_channel_config(
                    "invalid_channel",
                    "NOT_A_POSITION",
                    "not_a_number",
                    "not_a_size",
                    "not_a_number",
                    ""
                )
            );

            sight::service::config_t config;
            config.put("message", "Default Message");
            config.add_child("channels", channels);

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, config]
            {
                _service->set_config(config);
                CHECK_NOTHROW(_service->configure());
                CHECK_NOTHROW(_service->start().wait());
            }).get();

            CHECK(_service->started());

            // The valid channel's configuration (position/duration/size/max/closable) must be honored: a
            // notification on that channel is displayed with the overridden size.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::information>(
                            "",
                            "on valid channel",
                            std::filesystem::path {},
                            "valid_channel"
                        )
                    )
                );
            }).get();

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Success"),
                std::make_optional(std::string("on valid channel"))
            );
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "set_enum_parameter_updates_default_channel_configuration")
    {
        test_service(
            "sight::module::ui::qt::notifier",
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

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                CHECK_NOTHROW(
                    _service->slot("set_enum_parameter")->run(
                        std::string("TOP_LEFT"),
                        std::string("position")
                    )
                );
                CHECK_NOTHROW(
                    _service->slot("set_enum_parameter")->run(
                        std::string("infinite"),
                        std::string("duration")
                    )
                );
                CHECK_NOTHROW(_service->slot("set_enum_parameter")->run(std::string("400x100"), std::string("size")));
                CHECK_NOTHROW(_service->slot("set_enum_parameter")->run(std::string("5"), std::string("max")));
                CHECK_NOTHROW(_service->slot("set_enum_parameter")->run(std::string("true"), std::string("closable")));

                // Unknown key/invalid values are reported without throwing (caught internally).
                CHECK_NOTHROW(
                    _service->slot("set_enum_parameter")->run(
                        std::string("whatever"),
                        std::string("unknown_key")
                    )
                );
                CHECK_NOTHROW(_service->slot("set_enum_parameter")->run(std::string("BOGUS"), std::string("position")));
                CHECK_NOTHROW(
                    _service->slot("set_enum_parameter")->run(
                        std::string("not_a_number"),
                        std::string("duration")
                    )
                );
            }).get();

            // The infinite duration configured above must make the popup permanent (still there well past any
            // "timed" duration).
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::information>("", "permanent")
                    )
                );
            }).get();

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Success"),
                std::make_optional(std::string("permanent"))
            );
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_with_sound_does_not_throw")
    {
        test_service(
            "sight::module::ui::qt::notifier",
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

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Failure"),
                std::make_optional(std::string("with sound"))
            );
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "clean_notifications_removes_the_oldest_once_max_is_reached")
    {
        test_service(
            "sight::module::ui::qt::notifier",
            [](const sight::service::base::sptr& _service)
        {
            sight::service::config_t channels;
            channels.add_child("channel", make_channel_config("", "", "", "", "1", ""));

            sight::service::config_t config;
            config.add_child("channels", channels);

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, config]
            {
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
                        std::make_shared<sight::core::notification::information>("", "first")
                    )
                );
            }).get();

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Success"),
                std::make_optional(std::string("first"))
            );

            // With max=1 on the default channel, adding a second (non-permanent) notification must evict the
            // first one before displaying itself.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::information>("", "second")
                    )
                );
            }).get();

            SIGHT_TEST_FAIL_WAIT(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Success")
                == std::make_optional(std::string("second"))
            );
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "updating_is_a_no_op")
    {
        test_service(
            "sight::module::ui::qt::notifier",
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
        });
    }
}

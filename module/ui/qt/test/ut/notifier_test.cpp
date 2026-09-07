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

#include <QFontMetrics>
#include <QLabel>

#include <ui/qt/widget/notification_label.hpp>

#include <core/notification/error.hpp>
#include <core/notification/information.hpp>
#include <core/notification/instruction.hpp>
#include <core/notification/monitor.hpp>
#include <core/notification/warning.hpp>
#include <core/runtime/path.hpp>
#include <core/thread/worker.hpp>
#include <utest/wait.hpp>

#include <chrono>
#include <thread>

//------------------------------------------------------------------------------

/**
 * @brief Build a notification that stays displayed until the test closes it.
 *
 * An information or a warning defaults to a three seconds display, after which the popup fades out and is
 * destroyed. A test asserting on a popup would then race against that timer: the widget may already be gone by
 * the time the assertion runs, on a machine that is loaded or stepped through in a debugger. A duration of zero
 * means "no timeout at all", which takes the wall clock out of the picture for everything but the tests that
 * check the auto-dismissal itself.
 */
template<class N>
static std::shared_ptr<N> make_permanent(
    const std::string& _text,
    const std::string& _channel                = {},
    std::optional<std::filesystem::path> _icon = std::nullopt
)
{
    return std::make_shared<N>(
        sight::core::notification::message::params {
        .text     = _text,
        .icon     = std::move(_icon),
        .channel  = _channel,
        .duration = std::chrono::milliseconds::zero()
    });
}

//------------------------------------------------------------------------------

/**
 * @brief Whether the given popup label paints an icon, probed in the UI thread which owns it.
 *
 * Takes the popup found beforehand, rather than looking it up again by name, for the tests that must tell a
 * reused popup from a new one. std::nullopt means that popup is gone.
 */
static std::optional<bool> has_icon(const QPointer<QLabel>& _label)
{
    return sight::core::thread::get_default_worker()->post_task<std::optional<bool> >(
        [_label]() -> std::optional<bool>
    {
        const auto* const notification_label =
            qobject_cast<sight::ui::qt::widget::notification_label*>(_label.data());

        if(notification_label == nullptr)
        {
            return std::nullopt;
        }

        return !notification_label->icon().isNull();
    }).get();
}

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

            // instruction/information/warning/error map to distinct popup "types"
            // (instruction/information/warning/error), each getting a distinct object name, so all four
            // can be displayed and checked at once.
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::instruction>("instruction text")
                    )
                );
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::information>("information text")
                    )
                );
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::warning>("warning text")
                    )
                );
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        make_permanent<sight::core::notification::error>("error text")
                    )
                );
            }).get();

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Instruction"),
                std::make_optional(std::string("instruction text"))
            );
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Information"),
                std::make_optional(std::string("information text"))
            );
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Warning"),
                std::make_optional(std::string("warning text"))
            );
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Error"),
                std::make_optional(std::string("error text"))
            );
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_displays_the_notification_icon")
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

            // The warning carries its type's default icon, which the popup must display.
            const auto warning = make_permanent<sight::core::notification::warning>("warning text");

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, warning]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(warning)
                );
            }).get();

            // The popup label paints the icon itself, so that the style sheet frames both as one block.
            const auto has_icon = sight::ui::test::gui_fixture::query_widget<QLabel>(
                "NotificationDialog_Warning",
                [](QLabel* _label)
            {
                const auto* const notification_label =
                    qobject_cast<sight::ui::qt::widget::notification_label*>(_label);

                return notification_label != nullptr && !notification_label->icon().isNull();
            });

            REQUIRE(has_icon.has_value());
            CHECK(*has_icon);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "add_notification_updates_the_icon_on_a_reused_popup")
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

            const auto without_icon = make_permanent<sight::core::notification::information>(
                "without icon",
                "reused_popup",
                std::filesystem::path()
            );

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, without_icon]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(without_icon)
                );
            }).get();

            const auto label = sight::ui::test::gui_fixture::find_widget<QLabel>("NotificationDialog_Information");
            REQUIRE(!label.isNull());

            REQUIRE(has_icon(label) == std::make_optional(false));

            const auto with_icon = make_permanent<sight::core::notification::information>(
                "with icon",
                "reused_popup",
                sight::core::runtime::get_resource_file_path("sight::module::ui::icons/information.svg")
            );

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, with_icon]
            {
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(with_icon)
                );
            }).get();

            // Still the very same popup, now carrying both the new text and the new icon.
            CHECK(has_icon(label) == std::make_optional(true));
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Information"),
                std::make_optional(std::string("with icon"))
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

            CHECK_FALSE(sight::ui::test::gui_fixture::find_widget<QLabel>("NotificationDialog_Instruction"));
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
                        make_permanent<sight::core::notification::information>("channelled", "my_channel")
                    )
                );
            }).get();

            // Permanent, so that the popup disappearing below can only be the doing of close_notification.
            const auto label = sight::ui::test::gui_fixture::find_widget<QLabel>("NotificationDialog_Information");
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

            // The channel the assertions below are made on is configured permanent ("infinite"), so that its
            // popup cannot fade out while the test inspects it. A timed channel still covers the parsing of a
            // numeric duration.
            channels.add_child(
                "channel",
                make_channel_config("valid_channel", "TOP_LEFT", "infinite", "300x80", "2", "true")
            );
            channels.back().second.put("<xmlattr>.icon_size", "0");
            channels.add_child("channel", make_channel_config("timed_channel", "CENTERED", "1000", "", "", ""));
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
                            sight::core::notification::message::params {
                    .text    = "on valid channel",
                    .channel = "valid_channel"
                })
                    )
                );
            }).get();

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Information"),
                std::make_optional(std::string("on valid channel"))
            );

            // icon_size="0" on that channel draws the icon a text line high rather than hiding it. Both the icon
            // and the font it is measured against are read in the UI thread, which owns them.
            struct icon_probe final
            {
                bool has_icon {false};
                int icon_side {0};
                int font_height {0};
            };

            const auto icon = sight::ui::test::gui_fixture::query_widget<QLabel>(
                "NotificationDialog_Information",
                [](QLabel* _label)
            {
                const auto* const notification_label =
                    qobject_cast<sight::ui::qt::widget::notification_label*>(_label);

                if(notification_label == nullptr)
                {
                    return icon_probe {};
                }

                return icon_probe {
                    .has_icon    = !notification_label->icon().isNull(),
                    .icon_side   = notification_label->icon_side(),
                    .font_height = QFontMetrics(notification_label->font()).height()
                };
            });

            REQUIRE(icon.has_value());
            CHECK(icon->has_icon);
            CHECK_EQ(icon->icon_side, icon->font_height);
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
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Information"),
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

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Error"),
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

            // Only timed notifications are evicted, so the default channel keeps a duration, long enough for the
            // popups to outlive the test rather than fade out on their own while it runs.
            channels.add_child("channel", make_channel_config("", "", "600000", "", "1", ""));

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
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Information"),
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
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Information")
                == std::make_optional(std::string("second"))
            );
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "a_default_channel_without_max_keeps_the_default_limit")
    {
        test_service(
            "sight::module::ui::qt::notifier",
            [](const sight::service::base::sptr& _service)
        {
            sight::service::config_t channels;

            // A channel without a uid replaces the default configuration as a whole, so leaving its max out
            // must fall back to the service's own default rather than to no room at all, which would evict
            // every timed popup as soon as another notification is displayed. The notifications stay timed,
            // since permanent ones are never evicted and would not exercise this, but long enough lived to
            // outlive the test.
            channels.add_child("channel", make_channel_config("", "", "600000", "", "", ""));

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
                _service->slot("add_notification")->run(
                    std::static_pointer_cast<sight::core::notification::base>(
                        std::make_shared<sight::core::notification::warning>("", "second")
                    )
                );
            }).get();

            // An evicted popup is closed, then destroyed at the end of its fade out: wait that out, so that
            // the first popup is checked once the eviction, had it happened, would be over and done with.
            std::this_thread::sleep_for(std::chrono::seconds(1));

            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Information"),
                std::make_optional(std::string("first"))
            );
            CHECK_EQ(
                sight::ui::test::gui_fixture::get_widget_text<QLabel>("NotificationDialog_Warning"),
                std::make_optional(std::string("second"))
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

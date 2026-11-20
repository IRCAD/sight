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

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/uuid.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSlider>
#include <QTest>

#include <atomic>
#include <limits>
#include <memory>
#include <string>
#include <string_view>

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

/// The name settings gives the widget of _key, or one of its parts: "<service uid>/<key>[/<part>]".
static std::string name(const service::base::sptr& _service, std::string_view _key)
{
    return _service->base_id() + "/" + std::string(_key);
}

//------------------------------------------------------------------------------

static void add_item(
    service::config_t& _config,
    const std::string& _key,
    const std::string& _type,
    const std::string& _depends       = {},
    const std::string& _depends_value = {})
{
    service::config_t item;
    item.put("<xmlattr>.name", _key);
    item.put("<xmlattr>.key", _key);
    item.put("<xmlattr>.data", _key + "_data");
    item.put("<xmlattr>.reset", "false");
    if(_type == "integer")
    {
        item.put("<xmlattr>.widget", "spin");
    }

    if(_type == "combobox" || _type == "plain_combobox")
    {
        item.put("<xmlattr>.widget", "combobox");
        item.put("<xmlattr>.values", _type == "combobox" ? "Alpha=alpha;Beta=beta;Gamma=gamma" : "Alpha;Beta");
    }

    if(!_depends.empty())
    {
        item.put(
            "<xmlattr>.depends",
            _depends
        );
        if(!_depends_value.empty())
        {
            item.put("<xmlattr>.depends_value", _depends_value);
        }
    }

    _config.add_child("item", item);
}

} // namespace sight::module::ui::qt::ut

TEST_SUITE("sight::module::ui::qt::settings")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "combobox_icons_follow_values_and_external_selection")
    {
        int icon_size = 24;
        SUBCASE("default icon size")
        {
        }
        SUBCASE("custom icon size")
        {
            icon_size = 32;
        }
        test_service(
            "sight::module::ui::qt::settings",
            [&](const sight::service::base::sptr& _service)
        {
            auto mode         = std::make_shared<sight::data::string>("Rotation");
            auto plain        = std::make_shared<sight::data::string>("Beta");
            auto choices      = std::make_shared<sight::data::string>("Off=Off;Free=Free;Rot=Rotation");
            const auto worker = sight::core::thread::get_default_worker();
            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "mode", "plain_combobox");
            config.put("item.<xmlattr>.values", "choices");
            if(icon_size != 24)
            {
                config.put("item.<xmlattr>.icon_size", icon_size);
            }

            for(const auto& [value, icon] : std::vector<std::pair<std::string, std::string> > {
                {"Off", "hand.svg"}, {"Free", "play.svg"}, {"Rotation", "disconnect.svg"
                }
            })
            {
                sight::service::config_t mapping;
                mapping.put("<xmlattr>.value", value);
                mapping.put("<xmlattr>.icon", "sight::module::ui::icons/" + icon);
                config.add_child("item.item", mapping);
            }

            sight::module::ui::qt::ut::add_item(config, "plain", "plain_combobox");
            _service->set_inout(mode, "item.data", true, false, 0);
            _service->set_input(choices, "item.values", true, false, 0);
            _service->set_inout(plain, "item.data", true, false, 1);
            worker->post_task<void>([&]{_service->configure(config); _service->start().get();}).get();
            const std::string mode_name = sight::module::ui::qt::ut::name(_service, "mode");
            const auto combo            = find_widget<QComboBox>(mode_name);
            const auto plain_combo      = find_widget<QComboBox>(sight::module::ui::qt::ut::name(_service, "plain"));
            REQUIRE_FALSE(combo.isNull());
            REQUIRE_FALSE(plain_combo.isNull());
            qint64 off_icon      = 0;
            qint64 free_icon     = 0;
            qint64 rotation_icon = 0;
            worker->post_task<void>(
                [&]
            {
                CHECK(combo->iconSize() == QSize(icon_size, icon_size));
                CHECK(combo->currentText() == "Rot");
                CHECK(combo->currentData().toString() == "Rotation");
                for(int index = 0 ; index < 3 ; ++index)
                {
                    CHECK_FALSE(combo->itemIcon(index).isNull());
                }

                off_icon      = combo->itemIcon(0).cacheKey();
                free_icon     = combo->itemIcon(1).cacheKey();
                rotation_icon = combo->itemIcon(2).cacheKey();
                CHECK(off_icon != free_icon);
                CHECK(rotation_icon != free_icon);
                CHECK(plain_combo->currentText() == "Beta");
                for(int index = 0 ; index < plain_combo->count() ; ++index)
                {
                    CHECK(plain_combo->itemIcon(index).isNull());
                }

                combo->setCurrentIndex(1);
                CHECK(mode->value() == "Free");
                // A controller rejection or Resume updates this same bound value.
                mode->set_value("Off");
                mode->async_emit(sight::data::signals::MODIFIED);
            }).get();
            SIGHT_TEST_FAIL_WAIT(query_widget<QComboBox>(mode_name, [](auto* _w){return _w->currentText();}) == "Off");
            worker->post_task<void>(
                [&]
            {
                choices->set_value("Rot=Rotation;Off=Off;Free=Free;Other=Other");
                choices->async_emit(sight::data::signals::MODIFIED);
            }).get();
            SIGHT_TEST_FAIL_WAIT(query_widget<QComboBox>(mode_name, [](auto* _w){return _w->count();}) == 4);
            worker->post_task<void>(
                [&]
            {
                CHECK(combo->currentData().toString() == "Off");
                CHECK(combo->itemIcon(0).cacheKey() == rotation_icon);
                CHECK(combo->itemIcon(1).cacheKey() == off_icon);
                CHECK(combo->itemIcon(2).cacheKey() == free_icon);
                CHECK(combo->itemIcon(3).isNull());
                combo->setCurrentIndex(0);
                CHECK(mode->value() == "Rotation");
                _service->stop().get();
                _service->start().get();
            }).get();
            CHECK(query_widget<QComboBox>(mode_name, [](auto* _w){return _w->currentText();}) == "Rot");
            CHECK(query_widget<QComboBox>(mode_name, [](auto* _w){return _w->itemIcon(0).isNull();}) == false);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "real_spin_step_precision_and_read_only_survive_restart")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            auto value  = std::make_shared<sight::data::real>(123.4);
            auto locked = std::make_shared<sight::data::boolean>(false);
            auto worker = sight::core::thread::get_default_worker();
            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "length", "real");
            config.put("item.<xmlattr>.min", 0.);
            config.put("item.<xmlattr>.max", 100000.);
            config.put("item.<xmlattr>.step", 1.);
            config.put("item.<xmlattr>.decimals", 1);
            config.put("item.<xmlattr>.read_only", "locked");
            _service->set_inout(value, "item.data", true, false, 0);
            _service->set_input(locked, "item.read_only", true, false, 0);
            worker->post_task<void>([&]{_service->configure(config); _service->start().get();}).get();
            const std::string length_name = sight::module::ui::qt::ut::name(_service, "length");
            auto spin                     = find_widget<QDoubleSpinBox>(length_name);
            REQUIRE_FALSE(spin.isNull());
            worker->post_task<void>(
                [&]
            {
                CHECK(spin->singleStep() == 1.);
                CHECK(spin->decimals() == 1);
                CHECK_FALSE(spin->isReadOnly());
                spin->stepUp();
                CHECK(value->value() == doctest::Approx(124.4));
                locked->set_value(true);
                locked->async_emit(sight::data::signals::MODIFIED);
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QDoubleSpinBox>(
                    length_name,
                    [](auto* _w)
            {
                return _w->isReadOnly();
            }) == true
            );
            worker->post_task<void>(
                [&]
            {
                QTest::keyClick(spin, Qt::Key_Up);
                QTest::keyClicks(spin, "999");
                CHECK(value->value() == doctest::Approx(124.4));
                _service->stop().get();
                _service->start().get();
            }).get();
            spin = find_widget<QDoubleSpinBox>(length_name);
            REQUIRE_FALSE(spin.isNull());
            CHECK(query_widget<QDoubleSpinBox>(length_name, [](auto* _w){return _w->isReadOnly();}) == true);
            worker->post_task<void>(
                [&]
            {
                locked->set_value(false);
                locked->async_emit(sight::data::signals::MODIFIED);
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QDoubleSpinBox>(
                    length_name,
                    [](auto* _w)
            {
                return _w->isReadOnly();
            }) == false
            );
            worker->post_task<void>(
                [&]
            {
                spin->stepDown();
                CHECK(value->value() == doctest::Approx(123.4));
                _service->stop().get();
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "settings_group_observes_enabled_data")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            auto enabled = std::make_shared<sight::data::boolean>(false);
            auto value   = std::make_shared<sight::data::boolean>(true);
            auto worker  = sight::core::thread::get_default_worker();
            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "group_value", "bool");
            _service->set_inout(value, "item.data", true, false, 0);
            _service->set_input(enabled, "config.enabled", true);
            worker->post_task<void>([&]{_service->configure(config); _service->start().get();}).get();
            CHECK(
                query_widget<QCheckBox>(
                    sight::module::ui::qt::ut::name(_service, "group_value"),
                    [](auto* _w)
            {
                return _w->isEnabled();
            }) == false
            );
            for(const bool state : {true, false, true})
            {
                worker->post_task<void>(
                    [&]
                {
                    enabled->set_value(state);
                    enabled->async_emit(sight::data::signals::MODIFIED);
                }).get();
                SIGHT_TEST_FAIL_WAIT(
                    query_widget<QCheckBox>(
                        sight::module::ui::qt::ut::name(_service, "group_value"),
                        [](auto* _w)
                {
                    return _w->isEnabled();
                }) == state
                );
                CHECK(value->value());
            }

            worker->post_task<void>([&]{_service->stop().get();}).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "non_linear_slider_observes_choices_and_external_values")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            auto value   = std::make_shared<sight::data::integer>(90);
            auto choices = std::make_shared<sight::data::string>("25;50;90;120;150");
            auto worker  = sight::core::thread::get_default_worker();
            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "nonlin", "integer");
            config.put("item.<xmlattr>.widget", "comboslider");
            config.put("item.<xmlattr>.use_index", false);
            config.put("item.<xmlattr>.values", "choices");
            _service->set_inout(value, "item.data", true, false, 0);
            _service->set_input(choices, "item.values", true, false, 0);
            worker->post_task<void>([&]{_service->configure(config); _service->start().get();}).get();
            CHECK(
                query_widget<QLabel>(
                    sight::module::ui::qt::ut::name(_service, "nonlin/min_label"),
                    [](auto* _w)
            {
                return _w->text();
            }) == "25"
            );
            CHECK(
                query_widget<QLabel>(
                    sight::module::ui::qt::ut::name(_service, "nonlin/max_label"),
                    [](auto* _w)
            {
                return _w->text();
            }) == "150"
            );
            worker->post_task<void>(
                [&]
            {
                choices->set_value("37;70;105;135;160");
                choices->async_emit(sight::data::signals::MODIFIED);
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QLabel>(
                    sight::module::ui::qt::ut::name(_service, "nonlin/min_label"),
                    [](auto* _w)
            {
                return _w->text();
            }) == "37"
            );
            CHECK(
                query_widget<QLabel>(
                    sight::module::ui::qt::ut::name(_service, "nonlin/max_label"),
                    [](auto* _w)
            {
                return _w->text();
            }) == "160"
            );
            worker->post_task<void>(
                [&]
            {
                value->set_value(135);
                value->async_emit(sight::data::signals::MODIFIED);
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QLabel>(
                    sight::module::ui::qt::ut::name(_service, "nonlin/value_label"),
                    [](auto* _w)
            {
                return _w->text();
            }) == "135"
            );
            worker->post_task<void>([&]{_service->stop().get();}).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "integer_combobox_normalizes_invalid_indices")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            auto selected = std::make_shared<sight::data::integer>(std::numeric_limits<std::int64_t>::max());
            auto choices  = std::make_shared<sight::data::string>("Raw;Left eye;Right eye");
            auto worker   = sight::core::thread::get_default_worker();
            std::atomic<int> notifications {0};
            std::atomic<bool> invalid_notification {false};
            auto observer = sight::core::com::new_slot(
                [&]
            {
                ++notifications;
                invalid_notification = invalid_notification || selected->value() < 0;
            });
            observer->set_worker(worker);
            auto connection = selected->signal(sight::data::signals::MODIFIED)->connect(observer);

            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "preview", "plain_combobox");
            config.put("item.<xmlattr>.values", "choices");
            _service->set_inout(selected, "item.data", true, false, 0);
            _service->set_input(choices, "item.values", true, false, 0);
            worker->post_task<void>([&]{_service->configure(config); _service->start().get();}).get();
            const auto combo =
                sight::ui::test::gui_fixture::find_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(
                        _service,
                        "preview"
                    )
                );
            REQUIRE(combo);
            SIGHT_TEST_FAIL_WAIT(notifications >= 1);
            worker->post_task<void>(
                [&]
            {
                CHECK(selected->value() == 0);
                CHECK(combo->currentIndex() == 0);
                combo->setCurrentIndex(2);
                CHECK(selected->value() == 2);
                choices->set_value("Raw");
                choices->async_emit(sight::data::signals::MODIFIED);
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(
                        _service,
                        "preview"
                    ),
                    [](auto* _w){return _w->count();}) == 1
            );
            worker->post_task<void>([&]{CHECK(selected->value() == 0); CHECK(combo->currentIndex() == 0);}).get();
            CHECK_FALSE(invalid_notification);

            for(const auto value : {
                static_cast<std::int64_t>(-1),
                static_cast<std::int64_t>(4),
                std::numeric_limits<std::int64_t>::max()
            })
            {
                worker->post_task<void>(
                    [&]
                {
                    selected->set_value(value);
                    selected->async_emit(
                        sight::data::signals::MODIFIED
                    );
                }).get();
                SIGHT_TEST_FAIL_WAIT(
                    query_widget<QComboBox>(
                        sight::module::ui::qt::ut::name(_service, "preview"),
                        [&](auto*){return selected->value();}) == 0
                );
            }

            // Restore stereo and retain a valid external selection across repeated choices notifications.
            worker->post_task<void>(
                [&]
            {
                choices->set_value("Raw;Left eye;Right eye");
                choices->async_emit(
                    sight::data::signals::MODIFIED
                );
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(
                        _service,
                        "preview"
                    ),
                    [](auto* _w){return _w->count();}) == 3
            );
            worker->post_task<void>(
                [&]
            {
                selected->set_value(1);
                selected->async_emit(
                    sight::data::signals::MODIFIED
                );
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(
                        _service,
                        "preview"
                    ),
                    [](auto* _w){return _w->currentIndex();}) == 1
            );
            worker->post_task<void>([&]{choices->async_emit(sight::data::signals::MODIFIED);}).get();
            worker->post_task<void>([&]{CHECK(selected->value() == 1); CHECK(combo->currentIndex() == 1);}).get();
            CHECK(notifications < 30);
            worker->post_task<void>(
                [&]
            {
                choices->set_value("");
                choices->async_emit(
                    sight::data::signals::MODIFIED
                );
            }).get();
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(
                        _service,
                        "preview"
                    ),
                    [](auto* _w){return _w->count();}) == 0
            );
            SIGHT_TEST_FAIL_WAIT(
                query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(_service, "preview"),
                    [&](auto*){return selected->value();}) == -1
            );
            worker->post_task<void>([&]{connection.disconnect(); _service->stop().get();}).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "combobox_non_index_values_remain_unchanged")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            auto numeric = std::make_shared<sight::data::integer>(20);
            auto text    = std::make_shared<sight::data::string>("Beta");
            sight::service::config_t config;
            sight::service::config_t item;
            item.put("<xmlattr>.data", "numeric");
            item.put("<xmlattr>.key", "numeric");
            item.put("<xmlattr>.name", "Numeric");
            item.put("<xmlattr>.widget", "combobox");
            item.put("<xmlattr>.values", "10;20");
            item.put("<xmlattr>.use_index", false);
            item.put("<xmlattr>.reset", false);
            config.add_child("item", item);
            sight::module::ui::qt::ut::add_item(config, "text", "plain_combobox");
            _service->set_inout(numeric, "item.data", true, false, 0);
            _service->set_inout(text, "item.data", true, false, 1);
            auto worker = sight::core::thread::get_default_worker();
            worker->post_task<void>([&]{_service->configure(config); _service->start().get();}).get();
            CHECK(
                query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(_service, "numeric"),
                    [](auto* _w)
            {
                return _w->currentText();
            }) == "20"
            );
            CHECK(
                query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(_service, "text"),
                    [](auto* _w)
            {
                return _w->currentText();
            }) == "Beta"
            );
            worker->post_task<void>(
                [&]
            {
                numeric->set_value(999);
                numeric->async_emit(sight::data::signals::MODIFIED);
                text->set_value("Missing");
                text->async_emit(sight::data::signals::MODIFIED);
            }).get();
            worker->post_task<void>(
                [&]
            {
                CHECK(numeric->value() == 999);
                CHECK(text->value() == "Missing");
                _service->stop().get();
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "multi_value_and_chained_dependencies")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            auto mode         = std::make_shared<sight::data::string>();
            mode->value()     = "gamma";
            auto selected     = std::make_shared<sight::data::boolean>();
            selected->value() = true;
            auto nested       = std::make_shared<sight::data::boolean>();
            nested->value()   = true;
            auto leaf         = std::make_shared<sight::data::integer>();
            leaf->value()     = 5;
            auto plain        = std::make_shared<sight::data::string>();
            plain->value()    = "Beta";

            _service->set_inout(mode, "item.data", true, false, 0);
            _service->set_inout(selected, "item.data", true, false, 1);
            _service->set_inout(nested, "item.data", true, false, 2);
            _service->set_inout(leaf, "item.data", true, false, 3);
            _service->set_inout(plain, "item.data", true, false, 4);

            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "mode", "combobox");
            sight::module::ui::qt::ut::add_item(config, "selected", "bool", "mode", "Alpha;Beta");
            sight::module::ui::qt::ut::add_item(config, "nested", "bool", "selected");
            sight::module::ui::qt::ut::add_item(config, "leaf", "integer", "nested");
            sight::module::ui::qt::ut::add_item(config, "plain", "plain_combobox");

            auto worker = sight::core::thread::get_default_worker();
            worker->post_task<void>(
                [_service, config]
            {
                _service->set_config(config);
                _service->configure();
                _service->start().get();
            }).get();

            const auto enabled = [&_service](const std::string& _name)
                                 {
                                     return sight::ui::test::gui_fixture::query_widget<QWidget>(
                                         sight::module::ui::qt::ut::name(_service, _name + "/box"),
                                         [](QWidget* _widget){return _widget->isEnabled();});
                                 };

            CHECK_EQ(
                sight::ui::test::gui_fixture::query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(_service, "plain"),
                    [](QComboBox* _widget){return _widget->currentText().toStdString();}),
                "Beta"
            );
            CHECK_EQ(enabled("selected"), false);
            CHECK_EQ(enabled("nested"), false);
            CHECK_EQ(enabled("leaf"), false);

            const auto combo =
                sight::ui::test::gui_fixture::find_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(
                        _service,
                        "mode"
                    )
                );
            REQUIRE_FALSE(combo.isNull());
            CHECK_EQ(
                sight::ui::test::gui_fixture::query_widget<QComboBox>(
                    sight::module::ui::qt::ut::name(_service, "mode"),
                    [](QComboBox* _widget){return _widget->currentData().toString().toStdString();}),
                "gamma"
            );
            worker->post_task<void>([combo]{combo->setCurrentIndex(0);}).get();
            CHECK_EQ(enabled("selected"), true);
            CHECK_EQ(enabled("nested"), true);
            CHECK_EQ(enabled("leaf"), true);

            const auto check =
                sight::ui::test::gui_fixture::find_widget<QCheckBox>(
                    sight::module::ui::qt::ut::name(
                        _service,
                        "selected"
                    )
                );
            REQUIRE_FALSE(check.isNull());
            worker->post_task<void>([check]{check->setChecked(false);}).get();
            CHECK_EQ(enabled("nested"), false);
            CHECK_EQ(enabled("leaf"), false);

            worker->post_task<void>(
                [check, combo]
            {
                check->setChecked(true);
                combo->setCurrentIndex(1);
            }).get();
            CHECK_EQ(enabled("selected"), true);
            CHECK_EQ(enabled("leaf"), true);
            worker->post_task<void>([combo]{combo->setCurrentIndex(2);}).get();
            CHECK_EQ(enabled("selected"), false);
            CHECK_EQ(enabled("leaf"), false);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "refuses_decimals_a_widget_cannot_show")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            const auto value = std::make_shared<sight::data::real>(0.5);
            _service->set_inout(value, "item.data", true, false, 0);
            const auto configure = [&_service](const std::string& _widget, int _decimals)
                                   {
                                       sight::service::config_t config;
                                       sight::module::ui::qt::ut::add_item(config, "length", "real");
                                       config.put("item.<xmlattr>.widget", _widget);
                                       config.put("item.<xmlattr>.decimals", _decimals);
                                       sight::core::thread::get_default_worker()->post_task<void>(
                                           [_service, config]{_service->configure(config);}).get();
                                   };

            CHECK_THROWS_WITH(configure("slider", -1), doctest::Contains("must be between 0 and 9, not -1"));
            CHECK_THROWS_WITH(configure("slider", 10), doctest::Contains("must be between 0 and 9, not 10"));
            CHECK_THROWS_WITH(configure("spin", -1), doctest::Contains("not -1"));
            CHECK_NOTHROW(configure("slider", 9));
            CHECK_NOTHROW(configure("spin", 10));
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "a_real_slider_too_fine_for_its_range_moves_by_larger_steps")
    {
        test_service(
            "sight::module::ui::qt::settings",
            [](const sight::service::base::sptr& _service)
        {
            auto value = std::make_shared<sight::data::real>(500.);
            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "length", "real");
            config.put("item.<xmlattr>.widget", "slider");
            config.put("item.<xmlattr>.min", 0.);
            config.put("item.<xmlattr>.max", 1000.);
            config.put("item.<xmlattr>.decimals", 9);
            _service->set_inout(value, "item.data", true, false, 0);
            sight::core::thread::get_default_worker()->post_task<void>(
                [&]{_service->configure(config); _service->start().get();}).get();

            const auto slider = find_widget<QSlider>(sight::module::ui::qt::ut::name(_service, "length"));
            REQUIRE_FALSE(slider.isNull());
            sight::core::thread::get_default_worker()->post_task<void>(
                [slider]
            {
                // 10^9 steps per unit over 1000 units are more than an int counts: the slider keeps its whole range.
                CHECK(slider->maximum() == std::numeric_limits<int>::max());
                CHECK(slider->value() > 0);
            }).get();
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "widgets_left_by_a_stopped_or_destroyed_service_are_ignored")
    {
        // The settings live in a view of the test, so that the test holds their only reference.
        test_service(
            "sight::module::ui::view",
            [](const sight::service::base::sptr& _view)
        {
            const auto worker = sight::core::thread::get_default_worker();
            const auto value  = std::make_shared<sight::data::string>("alpha");
            std::string uid   = sight::core::tools::uuid::generate();
            std::erase(uid, '-');

            sight::service::config_t view_config;
            view_config.put("gui.layout.<xmlattr>.type", "sight::ui::layout::line");
            view_config.put("gui.layout.orientation.<xmlattr>.value", "vertical");
            view_config.put("gui.layout.view.<xmlattr>.proportion", "0");
            view_config.put("registry.view.<xmlattr>.sid", uid);

            auto settings = sight::service::add("sight::module::ui::qt::settings", uid);
            REQUIRE(settings);
            sight::service::config_t config;
            sight::module::ui::qt::ut::add_item(config, "mode", "combobox");
            settings->set_inout(value, "item.data", true, false, 0);
            worker->post_task<void>(
                [&]
            {
                _view->configure(view_config);
                _view->start().get();
                settings->configure(config);
                settings->start().get();
            }).get();

            const auto combo = find_widget<QComboBox>(sight::module::ui::qt::ut::name(settings, "mode"));
            REQUIRE_FALSE(combo.isNull());

            // Stopping deletes the widgets later, from the event loop: until then, they must no longer reach the data,
            // even once the service is destroyed. Everything runs in one task, so that the event loop never runs.
            worker->post_task<void>(
                [&]
            {
                CHECK(combo->currentData().toString() == "alpha");
                settings->stop().get();
                REQUIRE_FALSE(combo.isNull());
                combo->setCurrentIndex(2);
                CHECK(value->value() == "alpha");

                const std::weak_ptr<sight::service::base> weak_settings = settings;
                sight::service::remove(settings);
                settings.reset();
                REQUIRE(weak_settings.expired());
                REQUIRE_FALSE(combo.isNull());
                combo->setCurrentIndex(1);
                CHECK(value->value() == "alpha");
            }).get();
        });
    }
}

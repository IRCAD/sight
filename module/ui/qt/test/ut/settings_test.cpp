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
#include <data/string.hpp>

#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>
#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <QCheckBox>
#include <QComboBox>

#include <atomic>
#include <limits>
#include <memory>

namespace sight::module::ui::qt::ut
{

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
        item.put("<xmlattr>.depends", _depends);
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
            const auto combo = sight::ui::test::gui_fixture::find_widget<QComboBox>("preview");
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
            SIGHT_TEST_FAIL_WAIT(query_widget<QComboBox>("preview", [](auto* _w){return _w->count();}) == 1);
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
                        "preview",
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
            SIGHT_TEST_FAIL_WAIT(query_widget<QComboBox>("preview", [](auto* _w){return _w->count();}) == 3);
            worker->post_task<void>(
                [&]
            {
                selected->set_value(1);
                selected->async_emit(
                    sight::data::signals::MODIFIED
                );
            }).get();
            SIGHT_TEST_FAIL_WAIT(query_widget<QComboBox>("preview", [](auto* _w){return _w->currentIndex();}) == 1);
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
            SIGHT_TEST_FAIL_WAIT(query_widget<QComboBox>("preview", [](auto* _w){return _w->count();}) == 0);
            SIGHT_TEST_FAIL_WAIT(query_widget<QComboBox>("preview", [&](auto*){return selected->value();}) == -1);
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
            CHECK(query_widget<QComboBox>("numeric", [](auto* _w){return _w->currentText();}) == "20");
            CHECK(query_widget<QComboBox>("text", [](auto* _w){return _w->currentText();}) == "Beta");
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

            const auto enabled = [](const std::string& _name)
                                 {
                                     return sight::ui::test::gui_fixture::query_widget<QWidget>(
                                         _name + "_box",
                                         [](QWidget* _widget){return _widget->isEnabled();});
                                 };

            CHECK_EQ(
                sight::ui::test::gui_fixture::query_widget<QComboBox>(
                    "plain",
                    [](QComboBox* _widget){return _widget->currentText().toStdString();}),
                "Beta"
            );
            CHECK_EQ(enabled("selected"), false);
            CHECK_EQ(enabled("nested"), false);
            CHECK_EQ(enabled("leaf"), false);

            const auto combo = sight::ui::test::gui_fixture::find_widget<QComboBox>("mode");
            REQUIRE_FALSE(combo.isNull());
            CHECK_EQ(
                sight::ui::test::gui_fixture::query_widget<QComboBox>(
                    "mode",
                    [](QComboBox* _widget){return _widget->currentData().toString().toStdString();}),
                "gamma"
            );
            worker->post_task<void>([combo]{combo->setCurrentIndex(0);}).get();
            CHECK_EQ(enabled("selected"), true);
            CHECK_EQ(enabled("nested"), true);
            CHECK_EQ(enabled("leaf"), true);

            const auto check = sight::ui::test::gui_fixture::find_widget<QCheckBox>("selected");
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
}

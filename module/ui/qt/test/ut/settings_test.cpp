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

#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <QCheckBox>
#include <QComboBox>

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

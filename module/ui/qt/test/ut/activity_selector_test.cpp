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

#include "loader.hpp"

#include <core/runtime/runtime.hpp>
#include <core/thread/worker.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/qt/naming.hpp>

#include <QApplication>
#include <QPushButton>

#include <doctest/doctest.h>

#include <set>
#include <string>
#include <vector>

TEST_SUITE("sight::module::ui::qt::activity::selector")
{
    TEST_CASE("names_each_activity_button_after_its_xml_id")
    {
        // The activities of ut_sequencer have an id (id_0) and another title (title_0).
        sight::core::runtime::load_module("sight::module::ui::qt");
        CHECK_NOTHROW(sight::core::runtime::load_module("ut_sequencer"));
        CHECK_NOTHROW(sight::core::runtime::load_module("sight::module::activity"));
        auto [container, uid] = sight::module::ui::qt::ut::make_container();

        {
            auto selector = sight::service::add("sight::module::ui::qt::activity::selector", uid);
            const sight::module::ui::qt::ut::service_cleaner cleaner(selector);
            CHECK_NOTHROW(selector->configure(sight::service::config_t()));
            REQUIRE_NOTHROW(selector->start().get());

            // The names of the buttons the service shows, read where the widgets live.
            struct button
            {
                std::string name;
                std::string text;
                bool sanctioned {false};
            };

            const auto buttons = sight::core::thread::get_default_worker()->post_task<std::vector<button> >(
                []
            {
                std::vector<button> result;
                for(const auto* const widget : qApp->allWidgets())
                {
                    if(const auto* const push = qobject_cast<const QPushButton*>(widget); push != nullptr)
                    {
                        result.push_back(
                        {
                            .name       = push->objectName().toStdString(),
                            .text       = push->text().toStdString(),
                            .sanctioned = sight::ui::qt::detail::has_sanctioned_name(*push)
                        });
                    }
                }

                return result;
            }).get();

            // Each activity is reached by its id, never by the title it shows.
            std::set<std::string> names;
            for(const button& item : buttons)
            {
                CAPTURE(item.name);
                CAPTURE(item.text);
                CHECK(item.name.find("title_") == std::string::npos);
                if(item.text.find("title_") != std::string::npos)
                {
                    CHECK(item.sanctioned);
                    names.insert(item.name);
                }
            }

            CHECK(names == std::set<std::string> {uid + "/id_0", uid + "/id_1", uid + "/id_2"});
        }

        sight::module::ui::qt::ut::destroy_container(container);
    }
}

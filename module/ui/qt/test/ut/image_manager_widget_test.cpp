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

#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>
#include <data/image_series.hpp>
#include <data/series_set.hpp>
#include <utest/wait.hpp>

#include <QCheckBox>

#include <atomic>

TEST_SUITE("sight::module::ui::qt::series::image_manager_widget")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "cards_and_selection")
    {
        auto series_set = std::make_shared<sight::data::series_set>();
        auto first      = std::make_shared<sight::data::image_series>();
        auto second     = std::make_shared<sight::data::image_series>();

        for(const auto& image : {first, second})
        {
            image->resize({4, 4, 1}, sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
            image->set_series_description("Test image");
            series_set->push_back(image);
        }

        std::atomic_bool first_selected {false};
        std::atomic_bool second_selected {false};
        std::atomic_bool first_reselected {false};
        const auto selected_slot = sight::core::com::new_slot(
            [&first, &second, &first_selected, &second_selected, &first_reselected](std::string _id)
        {
            if(_id == first->get_id())
            {
                first_selected   = true;
                first_reselected = true;
            }
            else if(_id == second->get_id())
            {
                second_selected = true;
            }
        });
        selected_slot->set_worker(sight::core::thread::get_default_worker());

        test_service(
            "sight::module::ui::qt::series::image_manager_widget",
            [&series_set, &first, &second, selected_slot, &first_selected, &second_selected, &first_reselected]
            (const sight::service::base::sptr& _service)
        {
            _service->signal("image_selected")->connect(selected_slot);

            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, series_set]
            {
                _service->set_input(series_set, "series_set");

                sight::service::config_t config;
                config.put("config.<xmlattr>.thumbnail_width", 32);
                config.put("config.<xmlattr>.thumbnail_height", 32);
                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());
            SIGHT_TEST_WAIT(first_selected.load());
            CHECK(first_selected.load());
            CHECK(
                !sight::ui::test::gui_fixture::find_widget(
                    "image_card_" + first->get_id()
                ).isNull()
            );
            CHECK(
                !sight::ui::test::gui_fixture::find_widget(
                    "image_card_" + second->get_id()
                ).isNull()
            );

            const auto first_checkbox = sight::ui::test::gui_fixture::find_widget<QCheckBox>(
                "image_visibility_" + first->get_id()
            );
            const auto second_checkbox = sight::ui::test::gui_fixture::find_widget<QCheckBox>(
                "image_visibility_" + second->get_id()
            );
            REQUIRE(!first_checkbox.isNull());
            REQUIRE(!second_checkbox.isNull());
            CHECK(first_checkbox->isChecked());
            CHECK_FALSE(second_checkbox->isChecked());

            sight::core::thread::get_default_worker()->post_task<void>(
                [second_checkbox]
            {
                second_checkbox->setChecked(true);
                qApp->processEvents();
            }).get();
            SIGHT_TEST_WAIT(second_selected.load());
            CHECK(second_selected.load());

            first_reselected = false;
            sight::core::thread::get_default_worker()->post_task<void>(
                [second_checkbox]
            {
                second_checkbox->setChecked(false);
                qApp->processEvents();
            }).get();
            SIGHT_TEST_WAIT(first_reselected.load());
            CHECK(first_reselected.load());
            CHECK(first_checkbox->isChecked());
            CHECK_FALSE(second_checkbox->isChecked());

            first_reselected = false;
            sight::core::thread::get_default_worker()->post_task<void>(
                [first_checkbox]
            {
                first_checkbox->setChecked(false);
                qApp->processEvents();
            }).get();
            CHECK_FALSE(first_checkbox->isChecked());
            CHECK_FALSE(second_checkbox->isChecked());
            CHECK_FALSE(first_reselected.load());

            sight::core::thread::get_default_worker()->post_task<void>(
                [first_checkbox]
            {
                first_checkbox->setChecked(true);
                qApp->processEvents();
            }).get();
            SIGHT_TEST_WAIT(first_reselected.load());
            CHECK(first_checkbox->isChecked());
            CHECK_FALSE(second_checkbox->isChecked());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "series_set_removal")
    {
        auto series_set = std::make_shared<sight::data::series_set>();
        auto image      = std::make_shared<sight::data::image_series>();
        image->resize({4, 4, 1}, sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
        series_set->push_back(image);

        test_service(
            "sight::module::ui::qt::series::image_manager_widget",
            [&series_set, &image](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, series_set]
            {
                _service->set_input(series_set, "series_set");
                _service->configure();
                _service->start().wait();
            }).get();

            const std::string card_name = "image_card_" + image->get_id();
            const auto card             = sight::ui::test::gui_fixture::find_widget(card_name);
            REQUIRE(!card.isNull());

            sight::core::thread::get_default_worker()->post_task<void>(
                [series_set, image]
            {
                auto scoped_emitter = series_set->scoped_emit();
                series_set->remove(image);
            }).get();

            SIGHT_TEST_FAIL_WAIT((qApp->processEvents(), card.isNull()));
        });
    }
}

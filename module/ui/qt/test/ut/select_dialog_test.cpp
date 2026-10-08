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

#include <core/thread/worker.hpp>
#include <data/image_series.hpp>
#include <data/map.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/series_set.hpp>
#include <data/transfer_function.hpp>
#include <service/base.hpp>

#include <algorithm>
#include <memory>

TEST_SUITE("sight::module::ui::qt::series::select_dialog")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "transfer_functions_are_owned_by_image_id")
    {
        auto series_set = std::make_shared<sight::data::series_set>();
        auto tfs        = std::make_shared<sight::data::map>();
        auto first      = std::make_shared<sight::data::image_series>();
        auto second     = std::make_shared<sight::data::image_series>();
        first->resize({2, 2, 2}, sight::core::type::INT16, sight::data::image::pixel_format_t::gray_scale);
        second->resize({2, 2, 2}, sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
        series_set->push_back(first);
        series_set->push_back(second);

        test_service(
            "sight::module::ui::qt::series::select_dialog",
            [series_set, tfs, first, second](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, series_set, tfs]
            {
                _service->set_inout(series_set, "series_set");
                _service->set_inout(tfs, "transfer_functions");
                _service->configure();
                _service->start().get();
                _service->update().get();
            }).get();

            const auto selected_tf = [&_service]
                                     {
                                         return _service->output<sight::data::transfer_function>("transfer_function")
                                                .lock().get_shared();
                                     };
            auto first_tf = selected_tf();
            REQUIRE(first_tf);
            CHECK(first_tf == tfs->get<sight::data::transfer_function>(first->get_id()));
            first_tf->set_window(346.);
            first_tf->set_level(1073.);

            _service->slot("select_image")->async_run(second->get_id()).get();
            auto second_tf = selected_tf();
            REQUIRE(second_tf);
            CHECK(second_tf != first_tf);
            CHECK(second_tf == tfs->get<sight::data::transfer_function>(second->get_id()));
            CHECK(*second_tf == *sight::data::transfer_function::create_default_tf(second->type()));

            _service->slot("select_image")->async_run(first->get_id()).get();
            CHECK(selected_tf() == first_tf);
            CHECK(first_tf->window() == 346.);
            CHECK(first_tf->level() == 1073.);

            _service->slot("remove_image")->async_run(first->get_id()).get();
            CHECK_FALSE(tfs->contains(first->get_id()));
            CHECK(selected_tf() == second_tf);

            _service->slot("remove_image")->async_run(second->get_id()).get();
            CHECK(tfs->empty());
            CHECK_FALSE(selected_tf());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "selected_tf_survives_editor_restart")
    {
        test_services(
        {
            "sight::module::ui::qt::series::select_dialog",
            "sight::module::ui::qt::image::transfer_function"
        },
            [](const std::vector<sight::service::base::sptr>& _services)
        {
            auto series_set = std::make_shared<sight::data::series_set>();
            auto tfs        = std::make_shared<sight::data::map>();
            auto image      = std::make_shared<sight::data::image_series>();
            image->resize({2, 2, 2}, sight::core::type::INT16, sight::data::image::pixel_format_t::gray_scale);
            series_set->push_back(image);
            const auto selector_it = std::ranges::find_if(
                _services,
                [](const auto& _service)
            {
                return _service->is_a("sight::module::ui::qt::series::select_dialog");
            });
            const auto editor_it = std::ranges::find_if(
                _services,
                [](const auto& _service)
            {
                return _service->is_a("sight::module::ui::qt::image::transfer_function");
            });
            REQUIRE(selector_it != _services.end());
            REQUIRE(editor_it != _services.end());
            const auto& selector = *selector_it;
            const auto& editor   = *editor_it;

            sight::core::thread::get_default_worker()->post_task<void>(
                [selector, series_set, tfs]
            {
                selector->set_inout(series_set, "series_set");
                selector->set_inout(tfs, "transfer_functions");
                selector->configure();
                selector->start().get();
                selector->update().get();
            }).get();

            auto tf = selector->output<sight::data::transfer_function>("transfer_function").lock().get_shared();
            REQUIRE(tf);
            tf->set_window(346.);
            const auto saved = sight::data::object::copy(tf);

            sight::core::thread::get_default_worker()->post_task<void>(
                [editor, image, tf]
            {
                editor->set_input(image, "data.image");
                editor->set_inout(tf, "data.tf");
                sight::service::config_t config;
                config.put("config.<xmlattr>.useDefaultPath", false);
                config.put("config.<xmlattr>.preserve_current_tf", true);
                editor->configure(config);
                editor->start().get();
            }).get();

            CHECK(*tf == *saved);
            editor->slot("updateDefaultPreset")->async_run().get();
            CHECK(*tf == *saved);
            editor->stop().get();
            editor->start().get();
            CHECK(*tf == *saved);
            CHECK(tf == tfs->get<sight::data::transfer_function>(image->get_id()));
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "display_all_models_preserves_individual_selection")
    {
        auto series_set   = std::make_shared<sight::data::series_set>();
        auto image        = std::make_shared<sight::data::image_series>();
        auto first_model  = std::make_shared<sight::data::model_series>();
        auto second_model = std::make_shared<sight::data::model_series>();
        auto first_mesh   = std::make_shared<sight::data::reconstruction>();
        auto second_mesh  = std::make_shared<sight::data::reconstruction>();

        first_model->set_reconstruction_db({first_mesh});
        second_model->set_reconstruction_db({second_mesh});
        first_model->set_file("first.vtk");
        second_model->set_file("second.vtk");
        series_set->push_back(image);
        series_set->push_back(first_model);
        series_set->push_back(second_model);

        test_service(
            "sight::module::ui::qt::series::select_dialog",
            [&series_set, &image, &first_model, &second_model, &first_mesh, &second_mesh]
            (const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, series_set]
            {
                _service->set_inout(series_set, "series_set");

                sight::service::config_t config;
                config.put("config.<xmlattr>.display_all_models", true);
                _service->configure(config);
                _service->start().get();
                _service->update().get();
            }).get();

            const auto display = _service->output<sight::data::model_series>("display_models").lock().get_shared();
            REQUIRE(display);
            CHECK(display->get_reconstruction_db().size() == 2);
            CHECK(display->get_reconstruction_db()[0] == first_mesh);
            CHECK(display->get_reconstruction_db()[1] == second_mesh);
            CHECK(_service->output<sight::data::model_series>("model_series").lock().get_shared() == first_model);

            _service->slot("select_model")->async_run(second_mesh->get_id(), false).get();
            CHECK(_service->output<sight::data::model_series>("model_series").lock().get_shared() == second_model);
            CHECK(_service->output<sight::data::model_series>("display_models").lock().get_shared() == display);
            CHECK_FALSE(second_mesh->get_is_visible());

            _service->slot("remove_model")->async_run(second_mesh->get_id()).get();
            CHECK(series_set->size() == 2);
            CHECK(std::ranges::find(*series_set, image) != series_set->end());
            CHECK(std::ranges::find(*series_set, first_model) != series_set->end());
            CHECK(_service->output<sight::data::model_series>("model_series").lock().get_shared() == first_model);
            CHECK(display->get_reconstruction_db().size() == 1);
            CHECK(display->get_reconstruction_db().front() == first_mesh);

            _service->slot("remove_model")->async_run(first_mesh->get_id()).get();
            REQUIRE(series_set->size() == 1);
            CHECK(series_set->front() == image);
            CHECK_FALSE(_service->output<sight::data::model_series>("display_models").lock());
            CHECK_FALSE(_service->output<sight::data::model_series>("model_series").lock());
            CHECK(_service->output<sight::data::image>("image").lock().get_shared() == image);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "removing_last_model_keeps_image_and_clears_model_outputs")
    {
        auto series_set = std::make_shared<sight::data::series_set>();
        auto image      = std::make_shared<sight::data::image_series>();
        auto model      = std::make_shared<sight::data::model_series>();
        auto mesh       = std::make_shared<sight::data::reconstruction>();

        model->set_reconstruction_db({mesh});
        series_set->push_back(image);
        series_set->push_back(model);

        test_service(
            "sight::module::ui::qt::series::select_dialog",
            [&series_set, &image, &mesh](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, series_set]
            {
                _service->set_inout(series_set, "series_set");

                sight::service::config_t config;
                config.put("config.<xmlattr>.display_all_models", true);
                _service->configure(config);
                _service->start().get();
                _service->update().get();
            }).get();

            REQUIRE(_service->output<sight::data::model_series>("model_series").lock().get_shared());
            REQUIRE(_service->output<sight::data::model_series>("display_models").lock().get_shared());

            _service->slot("remove_model")->async_run(mesh->get_id()).get();

            REQUIRE(series_set->size() == 1);
            CHECK(series_set->front() == image);
            CHECK_FALSE(_service->output<sight::data::model_series>("model_series").lock());
            CHECK_FALSE(_service->output<sight::data::model_series>("display_models").lock());
            CHECK(_service->output<sight::data::image>("image").lock().get_shared() == image);
        });
    }
}

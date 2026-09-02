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
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <utest_data/generator/mesh.hpp>

#include <QApplication>
#include <QLabel>

#include <cstddef>
#include <memory>

TEST_SUITE("sight::module::ui::qt::image::thumbnail_widget")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "image_series_respects_max_thumbnails")
    {
        auto image = std::make_shared<sight::data::image_series>();
        image->resize({8, 8, 3}, sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);

        test_service(
            "sight::module::ui::qt::image::thumbnail_widget",
            [image](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, image]
            {
                _service->set_input(image, "image_series");

                sight::service::config_t config;
                config.put("config.<xmlattr>.width", 32);
                config.put("config.<xmlattr>.height", 32);
                config.put("config.<xmlattr>.max", 2);
                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto thumbnail_count = sight::core::thread::get_default_worker()->post_task<std::size_t>(
                []
            {
                std::size_t count = 0;
                for(auto* widget : qApp->allWidgets())
                {
                    if(const auto* label = qobject_cast<QLabel*>(widget);
                       label != nullptr && !label->pixmap(Qt::ReturnByValue).isNull())
                    {
                        ++count;
                    }
                }

                return count;
            }).get();

            CHECK_EQ(thumbnail_count, 2);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "model_series_renders_mesh_thumbnail")
    {
        auto model_series   = std::make_shared<sight::data::model_series>();
        auto reconstruction = std::make_shared<sight::data::reconstruction>();
        auto mesh           = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);
        reconstruction->set_mesh(mesh);
        model_series->set_reconstruction_db({reconstruction});

        test_service(
            "sight::module::ui::qt::image::thumbnail_widget",
            [model_series](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service, model_series]
            {
                _service->set_input(model_series, "model_series");

                sight::service::config_t config;
                config.put("config.<xmlattr>.width", 32);
                config.put("config.<xmlattr>.height", 32);
                config.put("config.<xmlattr>.max", 1);
                _service->set_config(config);
                _service->configure();
                _service->start().wait();
            }).get();

            CHECK(_service->started());

            const auto thumbnail_count = sight::core::thread::get_default_worker()->post_task<std::size_t>(
                []
            {
                std::size_t count = 0;
                for(auto* widget : qApp->allWidgets())
                {
                    if(const auto* label = qobject_cast<QLabel*>(widget);
                       label != nullptr && !label->pixmap(Qt::ReturnByValue).isNull())
                    {
                        ++count;
                    }
                }

                return count;
            }).get();

            CHECK_EQ(thumbnail_count, 1);
        });
    }
}

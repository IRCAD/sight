/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <core/os/temp_path.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <io/vtk/image_reader.hpp>
#include <io/vtk/image_writer.hpp>

#include <service/op.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

TEST_SUITE("sight::module::filter::mesh::vtk_mesher")
{
    namespace
    {

    class mesher_fixture
    {
    public:

        mesher_fixture()
        {
            mesher_service = sight::service::add("sight::module::filter::mesh::vtk_mesher");
            REQUIRE(mesher_service);
            REQUIRE(mesher_service->is_a("sight::module::filter::mesh::vtk_mesher"));

            image_series = std::make_shared<sight::data::image_series>();
            const auto dump_lock = image_series->dump_lock();

            const sight::data::image::size_t size               = {10, 20, 90};
            const sight::data::image::spacing_t spacing         = {1., 1., 1.};
            const sight::data::image::origin_t origin           = {0., 0., 0.};
            const sight::data::image::orientation_t orientation = {1., 0., 0., 0., 1., 0., 0., 0., 1.};

            sight::utest_data::generator::image::generate_image(
                image_series,
                size,
                spacing,
                origin,
                orientation,
                sight::core::type::get<std::int16_t>(),
                sight::data::image::pixel_format_t::gray_scale
            );

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                for(std::size_t y = 0 ; y < size[1] ; ++y)
                {
                    for(std::size_t z = 0 ; z < size[2] ; ++z)
                    {
                        const auto index = x + y * size[0] + z * size[0] * size[1];

                        if(x < 7 && y < 7 && z < 7)
                        {
                            std::int16_t val = 0;
                            image_series->set_pixel(index, reinterpret_cast<sight::data::image::buffer_t*>(&val));
                            CHECK(val == *reinterpret_cast<const std::int16_t*>(image_series->get_pixel(index)));
                        }
                        else
                        {
                            std::int16_t value = 255;
                            image_series->set_pixel(index, reinterpret_cast<sight::data::image::buffer_t*>(&value));
                            CHECK(value == *reinterpret_cast<const std::int16_t*>(image_series->get_pixel(index)));
                        }
                    }
                }
            }

            sight::core::os::temp_file temp_file;
            auto my_writer = std::make_shared<sight::io::vtk::image_writer>();
            my_writer->set_object(image_series);
            my_writer->set_file(temp_file);
            auto write_observer = std::make_shared<sight::core::notification::observer>("Test write");
            CHECK_NOTHROW(my_writer->write(write_observer));

            model_series = std::make_shared<sight::data::model_series>();

            mesher_service->set_input(image_series, "image_series");
            mesher_service->set_inout(model_series, "model_series");
        }

        ~mesher_fixture()
        {
            mesher_service->stop().get();
            sight::service::remove(mesher_service);
        }

        sight::service::base::sptr mesher_service;
        sight::data::model_series::sptr model_series;
        sight::data::image_series::sptr image_series;
    };

    } // namespace
    TEST_CASE_FIXTURE(mesher_fixture, "generate_mesh")
    {
        const std::string config =
            "<in key='image_series' uid='image_series'/>"
            "<out key='model_series' uid='modelSeries'/>"
            "<properties percent_reduction='50' value='255' pass_band='0.1' boundary_smoothing='true'"
            "feature_smoothing='true' feature_angle='120'/>";

        mesher_service->set_config(config);
        mesher_service->configure();
        mesher_service->start().get();
        mesher_service->update().get();
        {
            unsigned int number_points = 77;
            unsigned int number_cells  = 126;
            REQUIRE(model_series->get_reconstruction_db().size() == std::size_t(1));
            CHECK(model_series->get_reconstruction_db()[0]->get_mesh()->num_points() == number_points);
            CHECK(model_series->get_reconstruction_db()[0]->get_mesh()->num_cells() == number_cells);
        }
    }

    TEST_CASE_FIXTURE(mesher_fixture, "generate_mesh_with_min_reduction")
    {
        const std::string config =
            "<in key='image_series' uid='image_series'/>"
            "<out key='model_series' uid='modelSeries'/>"
            "<properties percent_reduction='0' value='255'/>";

        mesher_service->set_config(config);
        mesher_service->configure();
        mesher_service->start().get();
        mesher_service->update().get();
        {
            unsigned int number_points = 147;
            unsigned int number_cells  = 253;
            REQUIRE(model_series->get_reconstruction_db().size() == std::size_t(1));
            CHECK(model_series->get_reconstruction_db()[0]->get_mesh()->num_points() == number_points);
            CHECK(model_series->get_reconstruction_db()[0]->get_mesh()->num_cells() == number_cells);
        }
    }

    TEST_CASE_FIXTURE(mesher_fixture, "no_mesh_generated")
    {
        const std::string config =
            "<in key='image_series' uid='image_series'/>"
            "<out key='model_series' uid='modelSeries'/>"
            "<properties percent_reduction='90' value='30'/>";

        mesher_service->set_config(config);
        mesher_service->configure();
        mesher_service->start().get();
        mesher_service->update().get();

        CHECK(model_series->get_reconstruction_db().size() == std::size_t(0));
    }
}

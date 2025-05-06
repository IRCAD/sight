/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "vtk_mesher_test.hpp"

#include "data/exception.hpp"

#include <core/os/temp_path.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/random/generator.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <io/vtk/image_reader.hpp>
#include <io/vtk/image_writer.hpp>

#include <service/op.hpp>

#include <utest_data/generator/image.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <vtkVersion.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::filter::mesh::ut::vtk_mesher_test);

namespace sight::module::filter::mesh::ut
{

//------------------------------------------------------------------------------

void vtk_mesher_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::filter::mesh"));
}

//------------------------------------------------------------------------------

void vtk_mesher_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

inline static std::pair<sight::service::base::sptr, sight::data::image_series::sptr> generate_mesh_service()
{
    // Create service
    sight::service::base::sptr generate_mesh_service = sight::service::add("sight::module::filter::mesh::vtk_mesher");
    CPPUNIT_ASSERT(generate_mesh_service);
    CPPUNIT_ASSERT(generate_mesh_service->is_a("sight::module::filter::mesh::vtk_mesher"));

    auto image_series    = std::make_shared<sight::data::image_series>();
    const auto dump_lock = image_series->dump_lock();

    const data::image::size_t size               = {10, 20, 90};
    const data::image::spacing_t spacing         = {1., 1., 1.};
    const data::image::origin_t origin           = {0., 0., 0.};
    const data::image::orientation_t orientation = {1., 0., 0., 0., 1., 0., 0., 0., 1.};

    utest_data::generator::image::generate_image(
        image_series,
        size,
        spacing,
        origin,
        orientation,
        core::type::get<std::int16_t>(),
        data::image::pixel_format_t::gray_scale
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

                    image_series->set_pixel(index, reinterpret_cast<data::image::buffer_t*>(&val));
                    CPPUNIT_ASSERT_EQUAL(
                        val,
                        *reinterpret_cast<const std::int16_t*>(image_series->get_pixel(index))
                    );
                }
                else
                {
                    std::int16_t value = 255;
                    image_series->set_pixel(index, reinterpret_cast<data::image::buffer_t*>(&value));
                    CPPUNIT_ASSERT_EQUAL(
                        value,
                        *reinterpret_cast<const std::int16_t*>(image_series->get_pixel(index))
                    );
                }
            }
        }
    }

    core::os::temp_file temp_file;
    auto my_writer = std::make_shared<io::vtk::image_writer>();
    my_writer->set_object(image_series);
    my_writer->set_file(temp_file);
    CPPUNIT_ASSERT_NO_THROW(my_writer->write());

    return {generate_mesh_service, image_series};
}

//------------------------------------------------------------------------------

void vtk_mesher_test::generate_mesh()
{
    // Create service
    auto [mesher_service, image_series] = generate_mesh_service();

    service::config_t config;
    std::stringstream config_string;
    config_string
    << R"(<in key="image_series" uid="image_series"/>)"
       R"(<out key="model_series" uid="modelSeries"/>)"
       R"(<properties percent_reduction="50" value="255" pass_band="0.1" boundary_smoothing="true" )"
       R"(feature_smoothing="true" feature_angle="120"/>)";

    auto model_series = std::make_shared<sight::data::model_series>();

    boost::property_tree::read_xml(config_string, config);
    mesher_service->set_config(config);
    mesher_service->set_input(image_series, "image_series");
    mesher_service->set_inout(model_series, "model_series");
    mesher_service->configure();
    mesher_service->start().get();
    mesher_service->update().get();
    {
        unsigned int number_points = 77;
        unsigned int number_cells  = 126;
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), model_series->get_reconstruction_db().size());
        CPPUNIT_ASSERT_EQUAL(number_points, model_series->get_reconstruction_db()[0]->get_mesh()->num_points());
        CPPUNIT_ASSERT_EQUAL(number_cells, model_series->get_reconstruction_db()[0]->get_mesh()->num_cells());
    }
    mesher_service->stop().get();
    sight::service::remove(mesher_service);
}

//------------------------------------------------------------------------------

void vtk_mesher_test::generate_mesh_with_min_reduction()
{
    // Create service
    auto [mesher_service, image_series] = generate_mesh_service();

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<in key=\"image_series\" uid=\"image_series\"/>"
       "<out key=\"model_series\" uid=\"modelSeries\"/>"
       "<properties percent_reduction=\"0\" value=\"255\"/>";

    auto model_series = std::make_shared<sight::data::model_series>();

    boost::property_tree::read_xml(config_string, config);
    mesher_service->set_config(config);
    mesher_service->set_input(image_series, "image_series");
    mesher_service->set_inout(model_series, "model_series");
    mesher_service->configure();
    mesher_service->start().get();
    mesher_service->update().get();
    {
        unsigned int number_points = 147;
        unsigned int number_cells  = 253;
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), model_series->get_reconstruction_db().size());
        CPPUNIT_ASSERT_EQUAL(number_points, model_series->get_reconstruction_db()[0]->get_mesh()->num_points());
        CPPUNIT_ASSERT_EQUAL(number_cells, model_series->get_reconstruction_db()[0]->get_mesh()->num_cells());
    }
    mesher_service->stop().get();
    sight::service::remove(mesher_service);
}

//------------------------------------------------------------------------------

void vtk_mesher_test::no_mesh_generated()
{
    // Create service
    auto [mesher_service, image_series] = generate_mesh_service();

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<in key=\"image_series\" uid=\"image_series\"/>"
       "<out key=\"model_series\" uid=\"modelSeries\"/>"
       "<properties percent_reduction=\"90\" value=\"30\"/>";

    auto model_series = std::make_shared<sight::data::model_series>();

    boost::property_tree::read_xml(config_string, config);
    mesher_service->set_config(config);
    mesher_service->set_input(image_series, "image_series");
    mesher_service->set_inout(model_series, "model_series");
    mesher_service->configure();
    mesher_service->start().get();
    mesher_service->update().get();

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), model_series->get_reconstruction_db().size());

    mesher_service->stop().get();
    sight::service::remove(mesher_service);
}

} // namespace sight::module::filter::mesh::ut

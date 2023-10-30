/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "WriterReaderTest.hpp"

#include <core/os/temp_path.hpp>

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>
#include <data/reconstruction.hpp>
#include <data/series.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <io/dicom/reader/series_set.hpp>
#include <io/dicom/writer/series.hpp>
#include <io/dicom/writer/series_set.hpp>

#include <utest/filter.hpp>

#include <utest_data/generator/image.hpp>
#include <utest_data/generator/object.hpp>
#include <utest_data/generator/series_set.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::writer_reader_test);

namespace sight::io::dicom::ut
{

//-----------------------------------------------------------------------------

double tolerance(double _num)
{
    return std::floor(_num * 1000. + .5) / 1000;
}

//------------------------------------------------------------------------------

void round_spacing(data::image::sptr _image)
{
    data::image::spacing_t spacing = _image->spacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    _image->set_spacing(spacing);
}

//------------------------------------------------------------------------------

void writer_reader_test::setUp()
{
    // Set up context before running a test.
    if(utest::filter::ignore_slow_tests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
}

//------------------------------------------------------------------------------

void writer_reader_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void writer_reader_test::write_read_image_series_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    auto img_series = utest_data::generator::series_set::create_image_series();
    core::os::temp_dir tmp_dir;

    auto writer = std::make_shared<io::dicom::writer::series>();
    writer->set_object(img_series);
    writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    auto series_set = std::make_shared<data::series_set>();
    auto reader     = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(tmp_dir);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // check series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    auto series    = series_set->front();
    auto imgseries = std::dynamic_pointer_cast<data::image_series>(series);
    round_spacing(imgseries);
}

//------------------------------------------------------------------------------

data::series_set::sptr writer_reader_test::create_series_set()
{
    //create series_set
    auto series_set   = std::make_shared<data::series_set>();
    auto img_series   = utest_data::generator::series_set::create_image_series();
    auto model_series = utest_data::generator::series_set::create_model_series(1);

    series_set->push_back(img_series);
    series_set->push_back(model_series);

    // Add landmarks
    data::point_list::sptr landmarks     = data::helper::medical_image::get_landmarks(*img_series);
    const data::image::spacing_t spacing = img_series->spacing();
    const data::image::origin_t origin   = img_series->origin();
    const data::point::sptr point        = std::make_shared<data::point>(
        2.6 + origin[0],
        1.2 + origin[1],
        4.5 + origin[2]
    );
    point->set_label("Label1");
    landmarks->get_points().push_back(point);
    data::point::sptr point2 = std::make_shared<data::point>(
        1.2 + origin[0],
        2.4 + origin[1],
        0.3 + origin[2]
    );
    point2->set_label("Label2");
    landmarks->get_points().push_back(point2);
    const data::image::size_t size = img_series->size();
    const data::point::sptr point3 = std::make_shared<data::point>(
        1.2 + origin[0],
        2.4 + origin[1],
        static_cast<double>(size[2] - 1) * spacing[2] + origin[2]
    );
    point3->set_label("Label3");
    landmarks->get_points().push_back(point3);

    // Add distance
    data::point_list::sptr pl   = std::make_shared<data::point_list>();
    const data::point::sptr pt1 = std::make_shared<data::point>(0., 0., 0.);
    const data::point::sptr pt2 = std::make_shared<data::point>(
        static_cast<double>(size[0] - 1) * spacing[0],
        static_cast<double>(size[1] - 1) * spacing[1],
        static_cast<double>(size[2] - 1) * spacing[2]
    );
    pl->get_points().push_back(pt1);
    pl->get_points().push_back(pt2);

    data::vector::sptr vect_dist = data::helper::medical_image::get_distances(*img_series);

    if(!vect_dist)
    {
        vect_dist = std::make_shared<data::vector>();
        data::helper::medical_image::set_distances(*img_series, vect_dist);
    }

    vect_dist->push_back(pl);

    data::helper::medical_image::set_distance_visibility(*img_series, true);
    data::helper::medical_image::set_landmarks_visibility(*img_series, true);
    // Update Reconstruction
    data::reconstruction::sptr rec = model_series->get_reconstruction_db().front();
    data::mesh::sptr mesh          = rec->get_mesh();
    mesh->clear<data::mesh::attribute::cell_colors>();
    mesh->clear<data::mesh::attribute::point_colors>();
    mesh->clear<data::mesh::attribute::cell_normals>();

    // gdcm only manage ambient color in reconstruction
    data::material::sptr material = std::make_shared<data::material>();
    data::color::sptr color       = utest_data::generator::object::randomize_color();
    material->set_diffuse(color);
    rec->set_material(material);
    rec->set_image(data::image::sptr()); // not managed

    model_series->set_field("ShowReconstructions", std::make_shared<data::boolean>(true));

    return series_set;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut

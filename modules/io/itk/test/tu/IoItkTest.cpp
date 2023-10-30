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

#include "IoItkTest.hpp"

#include <core/os/temp_path.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/date_and_time.hpp>

#include <data/image_series.hpp>
#include <data/series_set.hpp>

#include <io/__/service/ioTypes.hpp>

#include <service/op.hpp>
#include <service/registry.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/image.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::itk::ut::io_itk_test);

namespace sight::module::io::itk::ut
{

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

void io_itk_test::setUp()
{
}

//------------------------------------------------------------------------------

void io_itk_test::tearDown()
{
}

//------------------------------------------------------------------------------

void execute_service(
    const SPTR(data::object)& _obj,
    const std::string& _srv_impl,
    const boost::property_tree::ptree& _cfg,
    const data::access _access = data::access::inout
)
{
    service::base::sptr srv = service::add(_srv_impl);
    CPPUNIT_ASSERT(srv);

    if(_access == data::access::inout)
    {
        srv->set_inout(_obj, sight::io::service::DATA_KEY);
    }
    else
    {
        srv->set_input(_obj, sight::io::service::DATA_KEY);
    }

    srv->set_config(_cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::unregister_service(srv);
}

//------------------------------------------------------------------------------

void io_itk_test::test_image_series_writer_jpg()
{
    // Create image series
    auto image_series = std::make_shared<data::image_series>();
    utest_data::generator::image::generate_random_image(image_series, core::type::INT16);

    // Create path
    core::os::temp_dir tmp_dir;

    // Create config
    service::config_t srv_cfg;
    srv_cfg.add("folder", tmp_dir.string());

    // Create and execute service
    execute_service(
        image_series,
        "sight::module::io::itk::sliced_image_series_writer",
        srv_cfg,
        data::access::in
    );
}

//------------------------------------------------------------------------------

double tolerance(double _num)
{
    return std::floor(_num * 100. + .5) / 100.;
}

//------------------------------------------------------------------------------

void io_itk_test::test_save_load_inr()
{
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::origin_t origin = {0., 0., 0.};
    image->set_origin(origin);

    // save image in inr
    core::os::temp_dir tmp_dir;
    const auto& path = tmp_dir / "image.inr.gz";

    // Create config
    service::config_t srv_cfg;
    srv_cfg.add("file", path.string());

    // Create and execute service
    execute_service(
        image,
        "sight::module::io::itk::image_writer",
        srv_cfg,
        data::access::in
    );

    // load image
    data::image::sptr image2 = std::make_shared<data::image>();
    execute_service(
        image2,
        "sight::module::io::itk::image_reader",
        srv_cfg,
        data::access::inout
    );

    data::image::spacing_t spacing = image2->spacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->set_spacing(spacing);

    // check image
    image2->set_window_center(image->window_center());
    image2->set_window_width(image->window_width());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void io_itk_test::test_save_load_nifti()
{
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, core::type::INT16);

    const data::image::origin_t origin = {0.5F, 0.2F, 1.2F};
    image->set_origin(origin);

    // save image in inr
    core::os::temp_dir tmp_dir;
    const auto& path = tmp_dir / "image.nii";

    // Create config
    service::config_t srv_cfg;
    srv_cfg.add("file", path.string());

    // Create and execute service
    execute_service(
        image,
        "sight::module::io::itk::image_writer",
        srv_cfg,
        data::access::in
    );

    // load image
    data::image::sptr image2 = std::make_shared<data::image>();
    execute_service(
        image2,
        "sight::module::io::itk::image_reader",
        srv_cfg,
        data::access::inout
    );

    data::image::spacing_t spacing = image2->spacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->set_spacing(spacing);

    // check image
    image2->set_window_center(image->window_center());
    image2->set_window_width(image->window_width());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void io_itk_test::image_series_inr_test()
{
    auto image_series = std::make_shared<data::image_series>();
    utest_data::generator::image::generate_random_image(image_series, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::origin_t origin = {0., 0., 0.};
    image_series->set_origin(origin);

    // save image in inr
    core::os::temp_dir tmp_dir;
    const auto& path = tmp_dir / "imageseries.inr.gz";

    // Create config
    service::config_t srv_cfg;
    srv_cfg.add("file", path.string());

    // Create and execute service
    execute_service(
        image_series,
        "sight::module::io::itk::image_series_writer",
        srv_cfg,
        data::access::in
    );

    // load image
    auto image_series2 = std::make_shared<data::image_series>();
    execute_service(
        image_series2,
        "sight::module::io::itk::image_series_reader",
        srv_cfg,
        data::access::inout
    );

    data::image::spacing_t spacing = image_series2->spacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image_series2->set_spacing(spacing);

    // check image
    image_series2->set_window_center(image_series->window_center());
    image_series2->set_window_width(image_series->window_width());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "image" part...
    const auto image  = std::dynamic_pointer_cast<data::image>(image_series);
    const auto image2 = std::dynamic_pointer_cast<data::image>(image_series2);
    image2->set_description(image->get_description());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void io_itk_test::image_series_nifti_test()
{
    auto image_series = std::make_shared<data::image_series>();
    utest_data::generator::image::generate_random_image(image_series, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::origin_t origin = {0., 0., 0.};
    image_series->set_origin(origin);

    // save image in inr
    core::os::temp_dir tmp_dir;
    const auto& path = tmp_dir / "imageseries.nii";

    // Create config
    service::config_t srv_cfg;
    srv_cfg.add("file", path.string());

    // Create and execute service
    execute_service(
        image_series,
        "sight::module::io::itk::image_series_writer",
        srv_cfg,
        data::access::in
    );

    // load image
    data::image_series::sptr image_series2 = std::make_shared<data::image_series>();
    execute_service(
        image_series2,
        "sight::module::io::itk::image_series_reader",
        srv_cfg,
        data::access::inout
    );

    data::image::spacing_t spacing = image_series2->spacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image_series2->set_spacing(spacing);

    // check image
    image_series2->set_window_center(image_series->window_center());
    image_series2->set_window_width(image_series->window_width());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "image" part...
    const auto image  = std::dynamic_pointer_cast<data::image>(image_series);
    const auto image2 = std::dynamic_pointer_cast<data::image>(image_series2);
    image2->set_description(image->get_description());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void io_itk_test::series_set_inr_test()
{
    /*
     * - image.inr.gz : CT, type int16, size: 512x512x134, spacing 0.781:0.781:1.6
     * - skin.inr.gz : mask skin, type uint8, size: 512x512x134, spacing 0.781:0.781:1.6
     */
    const std::filesystem::path image_file = utest_data::dir() / "sight/image/inr/image.inr.gz";
    const std::filesystem::path skin_file  = utest_data::dir() / "sight/image/inr/skin.inr.gz";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_file.string() + "' does not exist",
        std::filesystem::exists(image_file)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + skin_file.string() + "' does not exist",
        std::filesystem::exists(skin_file)
    );

    // Create config
    service::config_t srv_cfg;
    srv_cfg.add("file", image_file.string());
    srv_cfg.add("file", skin_file.string());

    // load series_set
    auto series_set = std::make_shared<data::series_set>();
    execute_service(
        series_set,
        "sight::module::io::itk::series_set_reader",
        srv_cfg,
        data::access::inout
    );

    const data::image::spacing_t spacing = {0.781, 0.781, 1.6};
    const data::image::size_t size       = {512, 512, 134};

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());
    data::image_series::sptr img_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
    CPPUNIT_ASSERT(img_series);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), img_series->get_modality());

    CPPUNIT_ASSERT_EQUAL(std::string("int16"), img_series->type().name());
    CPPUNIT_ASSERT(size == img_series->size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], img_series->spacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], img_series->spacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], img_series->spacing()[2], EPSILON);

    img_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(1));
    CPPUNIT_ASSERT(img_series);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), img_series->get_modality());

    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), img_series->type().name());
    CPPUNIT_ASSERT(size == img_series->size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], img_series->spacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], img_series->spacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], img_series->spacing()[2], EPSILON);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk::ut

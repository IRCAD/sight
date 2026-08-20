/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/os/temp_path.hpp>

#include <data/image_series.hpp>
#include <data/series_set.hpp>

#include <io/__/service/io_types.hpp>
#include <io/__/service/reader.hpp>

#include <service/op.hpp>
#include <service/registry.hpp>
#include <ui/test/dialog/location.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <filesystem>

TEST_SUITE("sight::module::io::itk")
{
    static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

    static void execute_service(
        const sight::data::object::sptr& _obj,
        const std::string& _srv_impl,
        const boost::property_tree::ptree& _cfg,
        const sight::data::access _access = sight::data::access::inout
)
    {
        sight::service::base::sptr srv = sight::service::add(_srv_impl);
        CHECK(srv);

        if(_access == sight::data::access::inout)
        {
            // Readers expose their generic data under the hierarchical key, writers keep the legacy one
            srv->set_inout(_obj, sight::io::service::READER_DATA_KEY);
        }
        else
        {
            srv->set_input(_obj, sight::io::service::WRITER_DATA_KEY);
        }

        srv->set_config(_cfg);
        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());
        CHECK_NOTHROW(srv->update().get());
        CHECK_NOTHROW(srv->stop().get());
        sight::service::unregister_service(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("image_series_writer_jpg")
    {
        // Create image series
        auto image_series = std::make_shared<sight::data::image_series>();
        sight::utest_data::generator::image::generate_random_image(image_series, sight::core::type::INT16);

        // Create path
        sight::core::os::temp_dir tmp_dir;

        // Create config
        sight::service::config_t srv_cfg;
        srv_cfg.add("path.<xmlattr>.folder", tmp_dir.string());

        // Create and execute service
        execute_service(
            image_series,
            "sight::module::io::itk::sliced_image_series_writer",
            srv_cfg,
            sight::data::access::in
        );
    }

//------------------------------------------------------------------------------

    static double tolerance(double _num)
    {
        return std::floor(_num * 100. + .5) / 100.;
    }

//------------------------------------------------------------------------------

    TEST_CASE("save_load_inr")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::INT16);

        // inr only support image origin (0,0,0)
        const sight::data::image::origin_t origin = {0., 0., 0.};
        image->set_origin(origin);
        image->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        // save image in inr
        sight::core::os::temp_dir tmp_dir;
        const auto& path = tmp_dir / "image.inr.gz";

        // Create config
        sight::service::config_t srv_cfg;
        srv_cfg.add("path.<xmlattr>.file", path.string());

        // Create and execute service
        execute_service(
            image,
            "sight::module::io::itk::image_writer",
            srv_cfg,
            sight::data::access::in
        );

        // load image
        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        execute_service(
            image2,
            "sight::module::io::itk::image_reader",
            srv_cfg,
            sight::data::access::inout
        );

        sight::data::image::spacing_t spacing = image2->spacing();
        std::ranges::transform(spacing, spacing.begin(), tolerance);
        image2->set_spacing(spacing);

        // check image
        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("save_load_nifti")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::INT16);

        sight::data::image::orientation_t orientation = {0.36F, 0.48F, -0.8F, -0.8F, 0.6F, 0.0F, 0.48F, 0.64F, 0.6F};
        std::ranges::transform(orientation, orientation.begin(), tolerance);
        image->set_orientation(orientation);

        const sight::data::image::origin_t origin = {0.5F, 0.2F, 1.2F};
        image->set_origin(origin);

        // save image in inr
        sight::core::os::temp_dir tmp_dir;
        const auto& path = tmp_dir / "image.nii";

        // Create config
        sight::service::config_t srv_cfg;
        srv_cfg.add("path.<xmlattr>.file", path.string());

        // Create and execute service
        execute_service(
            image,
            "sight::module::io::itk::image_writer",
            srv_cfg,
            sight::data::access::in
        );

        // load image
        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        execute_service(
            image2,
            "sight::module::io::itk::image_reader",
            srv_cfg,
            sight::data::access::inout
        );

        sight::data::image::spacing_t spacing = image2->spacing();
        std::ranges::transform(spacing, spacing.begin(), tolerance);
        image2->set_spacing(spacing);

        auto orientation2 = image2->orientation();
        std::ranges::transform(orientation2, orientation2.begin(), tolerance);
        image2->set_orientation(orientation2);

        // check image
        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("image_series_inr")
    {
        auto image_series = std::make_shared<sight::data::image_series>();
        sight::utest_data::generator::image::generate_random_image(image_series, sight::core::type::INT16);

        // inr only support image origin (0,0,0)
        image_series->set_origin({0., 0., 0.});
        image_series->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        // save image in inr
        sight::core::os::temp_dir tmp_dir;
        const auto& path = tmp_dir / "imageseries.inr.gz";

        // Create config
        sight::service::config_t srv_cfg;
        srv_cfg.add("path.<xmlattr>.file", path.string());

        // Create and execute service
        execute_service(
            image_series,
            "sight::module::io::itk::image_series_writer",
            srv_cfg,
            sight::data::access::in
        );

        // load image
        auto image_series2 = std::make_shared<sight::data::image_series>();
        execute_service(
            image_series2,
            "sight::module::io::itk::image_series_reader",
            srv_cfg,
            sight::data::access::inout
        );

        sight::data::image::spacing_t spacing = image_series2->spacing();
        std::ranges::transform(spacing, spacing.begin(), tolerance);
        image_series2->set_spacing(spacing);

        // check image
        image_series2->set_window_center(image_series->window_center());
        image_series2->set_window_width(image_series->window_width());

        // ITK reader change the description of the image, the modality is set to "OT", etc ...
        // We only compare "image" part...
        const auto image  = std::dynamic_pointer_cast<sight::data::image>(image_series);
        const auto image2 = std::dynamic_pointer_cast<sight::data::image>(image_series2);
        image2->set_description(image->get_description());

        CHECK(*image == *image2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("image_series_nifti")
    {
        auto image_series = std::make_shared<sight::data::image_series>();
        sight::utest_data::generator::image::generate_random_image(image_series, sight::core::type::INT16);

        // Use float value since ITK will convert double to float which may change the value
        image_series->set_origin({1.F, 1.F, 1.F});
        image_series->set_orientation({1.F, 0.F, 0.F, 0.F, 0.F, -1.F, 0.F, 1.F, 0.F});

        // save image in inr
        sight::core::os::temp_dir tmp_dir;
        const auto& path = tmp_dir / "imageseries.nii";

        // Create config
        sight::service::config_t srv_cfg;
        srv_cfg.add("path.<xmlattr>.file", path.string());

        // Create and execute service
        execute_service(
            image_series,
            "sight::module::io::itk::image_series_writer",
            srv_cfg,
            sight::data::access::in
        );

        // load image
        sight::data::image_series::sptr image_series2 = std::make_shared<sight::data::image_series>();
        execute_service(
            image_series2,
            "sight::module::io::itk::image_series_reader",
            srv_cfg,
            sight::data::access::inout
        );

        sight::data::image::spacing_t spacing = image_series2->spacing();
        std::ranges::transform(spacing, spacing.begin(), tolerance);
        image_series2->set_spacing(spacing);

        // check image
        image_series2->set_window_center(image_series->window_center());
        image_series2->set_window_width(image_series->window_width());

        // ITK reader change the description of the image, the modality is set to "OT", etc ...
        // We only compare "image" part...
        const auto image  = std::dynamic_pointer_cast<sight::data::image>(image_series);
        const auto image2 = std::dynamic_pointer_cast<sight::data::image>(image_series2);
        image2->set_description(image->get_description());

        CHECK(*image == *image2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("series_set_inr")
    {
        /*
         * - image.inr.gz : CT, type int16, size: 512x512x134, spacing 0.781:0.781:1.6
         * - skin.inr.gz : mask skin, type uint8, size: 512x512x134, spacing 0.781:0.781:1.6
         */
        const std::filesystem::path image_file = sight::utest_data::dir() / "sight/image/inr/image.inr.gz";
        const std::filesystem::path skin_file  = sight::utest_data::dir() / "sight/image/inr/skin.inr.gz";

        CHECK_MESSAGE(
            std::filesystem::exists(image_file),
            "The file '",
            image_file.string(),
            "' does not exist"
        );

        CHECK_MESSAGE(
            std::filesystem::exists(skin_file),
            "The file '",
            skin_file.string(),
            "' does not exist"
        );

        // Create config
        sight::service::config_t srv_cfg;
        srv_cfg.add("path.<xmlattr>.file", image_file.string() + ";" + skin_file.string());

        // load series_set
        auto series_set = std::make_shared<sight::data::series_set>();
        execute_service(
            series_set,
            "sight::module::io::itk::series_set_reader",
            srv_cfg,
            sight::data::access::inout
        );

        const sight::data::image::spacing_t spacing = {0.781, 0.781, 1.6};
        const sight::data::image::size_t size       = {512, 512, 134};

        CHECK_EQ(std::size_t(2), series_set->size());
        sight::data::image_series::sptr img_series =
            std::dynamic_pointer_cast<sight::data::image_series>(series_set->at(0));
        CHECK(img_series);
        CHECK_EQ(sight::data::dicom::modality_t::ot, img_series->get_modality());

        CHECK_EQ(std::string("int16"), img_series->type().name());
        CHECK(size == img_series->size());
        CHECK((std::abs(spacing[0] - img_series->spacing()[0]) < EPSILON));
        CHECK((std::abs(spacing[1] - img_series->spacing()[1]) < EPSILON));
        CHECK((std::abs(spacing[2] - img_series->spacing()[2]) < EPSILON));

        img_series = std::dynamic_pointer_cast<sight::data::image_series>(series_set->at(1));
        CHECK(img_series);
        CHECK_EQ(sight::data::dicom::modality_t::ot, img_series->get_modality());

        CHECK_EQ(std::string("uint8"), img_series->type().name());
        CHECK(size == img_series->size());
        CHECK((std::abs(spacing[0] - img_series->spacing()[0]) < EPSILON));
        CHECK((std::abs(spacing[1] - img_series->spacing()[1]) < EPSILON));
        CHECK((std::abs(spacing[2] - img_series->spacing()[2]) < EPSILON));
    }

//------------------------------------------------------------------------------

    TEST_CASE("series_set_reader_open_location_dialog")
    {
        const auto file1 = std::filesystem::temp_directory_path() / "image.nii";
        const auto file2 = std::filesystem::temp_directory_path() / "image.inr.gz";

        CHECK(sight::ui::test::dialog::location::clear());

        sight::ui::test::dialog::location::set_paths({file1, file2});

        auto reader = sight::service::add<sight::io::service::reader>(
            "sight::module::io::itk::series_set_reader"
        );

        REQUIRE(reader);

        reader->configure();

        CHECK_NOTHROW(reader->open_location_dialog());

        const auto& files = reader->get_files();

        REQUIRE_EQ(files.size(), std::size_t(2));
        CHECK_EQ(files[0], file1);
        CHECK_EQ(files[1], file2);

        sight::service::remove(reader);

        CHECK(sight::ui::test::dialog::location::clear());
    }
} // TEST_SUITE

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
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::itk::ut::IoItkTest);

namespace sight::module::io::itk::ut
{

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

void IoItkTest::setUp()
{
}

//------------------------------------------------------------------------------

void IoItkTest::tearDown()
{
}

//------------------------------------------------------------------------------

void execute_service(
    const SPTR(data::object)& _obj,
    const std::string& _srv_impl,
    const boost::property_tree::ptree& _cfg,
    const data::Access _access = data::Access::inout
)
{
    service::base::sptr srv = service::add(_srv_impl);
    CPPUNIT_ASSERT(srv);

    if(_access == data::Access::inout)
    {
        srv->set_inout(_obj, sight::io::service::s_DATA_KEY);
    }
    else
    {
        srv->set_input(_obj, sight::io::service::s_DATA_KEY);
    }

    srv->set_config(_cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::unregister_service(srv);
}

//------------------------------------------------------------------------------

void IoItkTest::testImageSeriesWriterJPG()
{
    // Create image series
    auto image_series = std::make_shared<data::image_series>();
    utest_data::generator::image::generateRandomImage(image_series, core::type::INT16);

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
        data::Access::in
    );
}

//------------------------------------------------------------------------------

double tolerance(double _num)
{
    return std::floor(_num * 100. + .5) / 100.;
}

//------------------------------------------------------------------------------

void IoItkTest::testSaveLoadInr()
{
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(image, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::Origin origin = {0., 0., 0.};
    image->setOrigin(origin);

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
        data::Access::in
    );

    // load image
    data::image::sptr image2 = std::make_shared<data::image>();
    execute_service(
        image2,
        "sight::module::io::itk::image_reader",
        srv_cfg,
        data::Access::inout
    );

    data::image::Spacing spacing = image2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check image
    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::testSaveLoadNifti()
{
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(image, core::type::INT16);

    const data::image::Origin origin = {0.5F, 0.2F, 1.2F};
    image->setOrigin(origin);

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
        data::Access::in
    );

    // load image
    data::image::sptr image2 = std::make_shared<data::image>();
    execute_service(
        image2,
        "sight::module::io::itk::image_reader",
        srv_cfg,
        data::Access::inout
    );

    data::image::Spacing spacing = image2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check image
    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesInrTest()
{
    auto image_series = std::make_shared<data::image_series>();
    utest_data::generator::image::generateRandomImage(image_series, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::Origin origin = {0., 0., 0.};
    image_series->setOrigin(origin);

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
        data::Access::in
    );

    // load image
    auto image_series2 = std::make_shared<data::image_series>();
    execute_service(
        image_series2,
        "sight::module::io::itk::image_series_reader",
        srv_cfg,
        data::Access::inout
    );

    data::image::Spacing spacing = image_series2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image_series2->setSpacing(spacing);

    // check image
    image_series2->setWindowCenter(image_series->getWindowCenter());
    image_series2->setWindowWidth(image_series->getWindowWidth());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "image" part...
    const auto image  = std::dynamic_pointer_cast<data::image>(image_series);
    const auto image2 = std::dynamic_pointer_cast<data::image>(image_series2);
    image2->setDescription(image->getDescription());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesNiftiTest()
{
    auto image_series = std::make_shared<data::image_series>();
    utest_data::generator::image::generateRandomImage(image_series, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::Origin origin = {0., 0., 0.};
    image_series->setOrigin(origin);

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
        data::Access::in
    );

    // load image
    data::image_series::sptr image_series2 = std::make_shared<data::image_series>();
    execute_service(
        image_series2,
        "sight::module::io::itk::image_series_reader",
        srv_cfg,
        data::Access::inout
    );

    data::image::Spacing spacing = image_series2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image_series2->setSpacing(spacing);

    // check image
    image_series2->setWindowCenter(image_series->getWindowCenter());
    image_series2->setWindowWidth(image_series->getWindowWidth());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "image" part...
    const auto image  = std::dynamic_pointer_cast<data::image>(image_series);
    const auto image2 = std::dynamic_pointer_cast<data::image>(image_series2);
    image2->setDescription(image->getDescription());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::SeriesSetInrTest()
{
    /*
     * - image.inr.gz : CT, type int16, size: 512x512x134, spacing 0.781:0.781:1.6
     * - skin.inr.gz : mask skin, type uint8, size: 512x512x134, spacing 0.781:0.781:1.6
     */
    const std::filesystem::path image_file = utest_data::Data::dir() / "sight/image/inr/image.inr.gz";
    const std::filesystem::path skin_file  = utest_data::Data::dir() / "sight/image/inr/skin.inr.gz";

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
        data::Access::inout
    );

    const data::image::Spacing spacing = {0.781, 0.781, 1.6};
    const data::image::Size size       = {512, 512, 134};

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());
    data::image_series::sptr img_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
    CPPUNIT_ASSERT(img_series);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), img_series->getModality());

    CPPUNIT_ASSERT_EQUAL(std::string("int16"), img_series->getType().name());
    CPPUNIT_ASSERT(size == img_series->size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], img_series->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], img_series->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], img_series->getSpacing()[2], EPSILON);

    img_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(1));
    CPPUNIT_ASSERT(img_series);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), img_series->getModality());

    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), img_series->getType().name());
    CPPUNIT_ASSERT(size == img_series->size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], img_series->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], img_series->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], img_series->getSpacing()[2], EPSILON);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk::ut

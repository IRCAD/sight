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

void executeService(
    const SPTR(data::object)& obj,
    const std::string& srvImpl,
    const boost::property_tree::ptree& cfg,
    const data::Access access = data::Access::inout
)
{
    service::base::sptr srv = service::add(srvImpl);
    CPPUNIT_ASSERT(srv);

    if(access == data::Access::inout)
    {
        srv->set_inout(obj, sight::io::service::s_DATA_KEY);
    }
    else
    {
        srv->set_input(obj, sight::io::service::s_DATA_KEY);
    }

    srv->set_config(cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::unregisterService(srv);
}

//------------------------------------------------------------------------------

void IoItkTest::testImageSeriesWriterJPG()
{
    // Create image series
    auto imageSeries = std::make_shared<data::image_series>();
    utest_data::generator::image::generateRandomImage(imageSeries, core::type::INT16);

    // Create path
    core::os::temp_dir tmpDir;

    // Create config
    service::config_t srvCfg;
    srvCfg.add("folder", tmpDir.string());

    // Create and execute service
    executeService(
        imageSeries,
        "sight::module::io::itk::sliced_image_series_writer",
        srvCfg,
        data::Access::in
    );
}

//------------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 100. + .5) / 100.;
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
    core::os::temp_dir tmpDir;
    const auto& path = tmpDir / "image.inr.gz";

    // Create config
    service::config_t srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        image,
        "sight::module::io::itk::image_writer",
        srvCfg,
        data::Access::in
    );

    // load image
    data::image::sptr image2 = std::make_shared<data::image>();
    executeService(
        image2,
        "sight::module::io::itk::image_reader",
        srvCfg,
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
    core::os::temp_dir tmpDir;
    const auto& path = tmpDir / "image.nii";

    // Create config
    service::config_t srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        image,
        "sight::module::io::itk::image_writer",
        srvCfg,
        data::Access::in
    );

    // load image
    data::image::sptr image2 = std::make_shared<data::image>();
    executeService(
        image2,
        "sight::module::io::itk::image_reader",
        srvCfg,
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
    auto imageSeries = std::make_shared<data::image_series>();
    utest_data::generator::image::generateRandomImage(imageSeries, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::Origin origin = {0., 0., 0.};
    imageSeries->setOrigin(origin);

    // save image in inr
    core::os::temp_dir tmpDir;
    const auto& path = tmpDir / "imageseries.inr.gz";

    // Create config
    service::config_t srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        imageSeries,
        "sight::module::io::itk::image_series_writer",
        srvCfg,
        data::Access::in
    );

    // load image
    auto imageSeries2 = std::make_shared<data::image_series>();
    executeService(
        imageSeries2,
        "sight::module::io::itk::image_series_reader",
        srvCfg,
        data::Access::inout
    );

    data::image::Spacing spacing = imageSeries2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    imageSeries2->setSpacing(spacing);

    // check image
    imageSeries2->setWindowCenter(imageSeries->getWindowCenter());
    imageSeries2->setWindowWidth(imageSeries->getWindowWidth());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "image" part...
    const auto image  = std::dynamic_pointer_cast<data::image>(imageSeries);
    const auto image2 = std::dynamic_pointer_cast<data::image>(imageSeries2);
    image2->setDescription(image->getDescription());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesNiftiTest()
{
    auto imageSeries = std::make_shared<data::image_series>();
    utest_data::generator::image::generateRandomImage(imageSeries, core::type::INT16);

    // inr only support image origin (0,0,0)
    const data::image::Origin origin = {0., 0., 0.};
    imageSeries->setOrigin(origin);

    // save image in inr
    core::os::temp_dir tmpDir;
    const auto& path = tmpDir / "imageseries.nii";

    // Create config
    service::config_t srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        imageSeries,
        "sight::module::io::itk::image_series_writer",
        srvCfg,
        data::Access::in
    );

    // load image
    data::image_series::sptr imageSeries2 = std::make_shared<data::image_series>();
    executeService(
        imageSeries2,
        "sight::module::io::itk::image_series_reader",
        srvCfg,
        data::Access::inout
    );

    data::image::Spacing spacing = imageSeries2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    imageSeries2->setSpacing(spacing);

    // check image
    imageSeries2->setWindowCenter(imageSeries->getWindowCenter());
    imageSeries2->setWindowWidth(imageSeries->getWindowWidth());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "image" part...
    const auto image  = std::dynamic_pointer_cast<data::image>(imageSeries);
    const auto image2 = std::dynamic_pointer_cast<data::image>(imageSeries2);
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
    const std::filesystem::path imageFile = utest_data::Data::dir() / "sight/image/inr/image.inr.gz";
    const std::filesystem::path skinFile  = utest_data::Data::dir() / "sight/image/inr/skin.inr.gz";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imageFile.string() + "' does not exist",
        std::filesystem::exists(imageFile)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + skinFile.string() + "' does not exist",
        std::filesystem::exists(skinFile)
    );

    // Create config
    service::config_t srvCfg;
    srvCfg.add("file", imageFile.string());
    srvCfg.add("file", skinFile.string());

    // load series_set
    auto series_set = std::make_shared<data::series_set>();
    executeService(
        series_set,
        "sight::module::io::itk::series_set_reader",
        srvCfg,
        data::Access::inout
    );

    const data::image::Spacing spacing = {0.781, 0.781, 1.6};
    const data::image::Size size       = {512, 512, 134};

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());
    data::image_series::sptr imgSeries = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());

    CPPUNIT_ASSERT_EQUAL(std::string("int16"), imgSeries->getType().name());
    CPPUNIT_ASSERT(size == imgSeries->size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], imgSeries->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], imgSeries->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], imgSeries->getSpacing()[2], EPSILON);

    imgSeries = std::dynamic_pointer_cast<data::image_series>(series_set->at(1));
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());

    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), imgSeries->getType().name());
    CPPUNIT_ASSERT(size == imgSeries->size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], imgSeries->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], imgSeries->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], imgSeries->getSpacing()[2], EPSILON);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk::ut

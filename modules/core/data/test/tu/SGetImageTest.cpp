/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SGetImageTest.hpp"

#include "data/Exception.hpp"

#include <core/service/base.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>

#include <service/op/Add.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SGetImageTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SGetImageTest::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::loadModule(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void SGetImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SGetImageTest::extractsImageFromImageSeries()
{
    // Create service
    sight::service::IService::sptr getImageService = sight::service::add("sight::module::data::SGetImage");
    CPPUNIT_ASSERT(getImageService);

    // create an empty image
    sight::data::Image::sptr inputImage = sight::data::Image::New();

    // create an empty image series
    sight::data::ImageSeries::sptr imageSeries = sight::data::ImageSeries::New();
    const std::string ID                       = "inputImage";
    inputImage->setID(ID);
    imageSeries->setImage(inputImage);
    getImageService->setInOut(imageSeries, "imageSeries");
    getImageService->setID("getImageService");
    getImageService->start().wait();
    CPPUNIT_ASSERT_NO_THROW(getImageService->update().get());
    auto outputImage = getImageService->getOutput<sight::data::Image>("image").lock();

    getImageService->stop().wait();

    CPPUNIT_ASSERT_EQUAL(inputImage, outputImage.get_shared());
    CPPUNIT_ASSERT_EQUAL(ID, outputImage->getID());
    sight::service::remove(getImageService);
}

//------------------------------------------------------------------------------

void SGetImageTest::invalidImageSeries()
{
    // Create service
    sight::service::IService::sptr getImageService = sight::service::add("sight::module::data::SGetImage");
    CPPUNIT_ASSERT(getImageService);
    getImageService->setInput(nullptr, "imageSeries");
    getImageService->start().wait();
    CPPUNIT_ASSERT_THROW(getImageService->update().get(), sight::data::Exception);
    getImageService->stop().wait();
    sight::service::remove(getImageService);
}

//------------------------------------------------------------------------------

void SGetImageTest::invalidOutputImage()
{
    // Create service
    sight::service::IService::sptr getImageService = sight::service::add("sight::module::data::SGetImage");
    CPPUNIT_ASSERT(getImageService);
    // create an empty image series
    sight::data::ImageSeries::sptr imageSeries = sight::data::ImageSeries::New();
    imageSeries->setImage(nullptr);
    getImageService->setInput(imageSeries, "imageSeries");
    getImageService->start().wait();
    CPPUNIT_ASSERT_THROW(getImageService->update().get(), sight::data::Exception);
    getImageService->stop().wait();
    sight::service::remove(getImageService);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut

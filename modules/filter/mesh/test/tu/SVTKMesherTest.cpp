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

#include "SVTKMesherTest.hpp"

#include "data/Exception.hpp"

#include <core/service/base.hpp>
#include <core/tools/random/Generator.hpp>
#include <core/tools/System.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>

#include <io/vtk/ImageReader.hpp>
#include <io/vtk/ImageWriter.hpp>

#include <service/op/Add.hpp>

#include <utestData/generator/Image.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <vtkVersion.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::filter::mesh::ut::SVTKMesherTest);

namespace sight::module::filter::mesh
{

namespace ut
{

using core::tools::random::safeRand;

//------------------------------------------------------------------------------

void SVTKMesherTest::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::loadModule(std::string("sight::module::filter::mesh"));
}

//------------------------------------------------------------------------------

void SVTKMesherTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SVTKMesherTest::generateMesh()
{
    // Create service
    sight::service::IService::sptr generateMeshService = sight::service::add("sight::module::filter::mesh::SVTKMesher");
    CPPUNIT_ASSERT(generateMeshService);
    data::Image::sptr image = data::Image::New();
    typedef std::int16_t Type;
    const data::Image::Size SIZE       = {10, 20, 90};
    const data::Image::Spacing spacing = {1., 1., 1.};
    const data::Image::Origin origin   = {0., 0., 0.};
    utestData::generator::Image::generateImage(
        image,
        SIZE,
        spacing,
        origin,
        core::Type::get<Type>(),
        data::Image::PixelFormat::GRAY_SCALE
    );

    [[maybe_unused]] const auto dumpLock = image->dump_lock();
    for(std::size_t x = 0 ; x < SIZE[0] ; ++x)
    {
        for(std::size_t y = 0 ; y < SIZE[1] ; ++y)
        {
            for(std::size_t z = 0 ; z < SIZE[2] ; ++z)
            {
                const auto index = x + y * SIZE[0] + z * SIZE[0] * SIZE[1];

                if(x < 7 && y < 7 && z < 7)
                {
                    std::int16_t val = 0;

                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&val));
                    CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
                else
                {
                    std::int16_t threshold = 255;
                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&threshold));
                    CPPUNIT_ASSERT_EQUAL(threshold, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
            }
        }
    }

    const std::filesystem::path path    = core::tools::System::getTemporaryFolder() / "meshTest.vtk";
    io::vtk::ImageWriter::sptr myWriter = io::vtk::ImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFile(path);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
    // create an empty image series
    sight::data::ImageSeries::sptr imageSeries = sight::data::ImageSeries::New();
    imageSeries->setImage(image);
    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"50\" threshold=\"255\"/>"
    ;
    boost::property_tree::read_xml(config_string, config);
    generateMeshService->setConfiguration(config);
    generateMeshService->setInput(imageSeries, "imageSeries");
    generateMeshService->setObjectId("modelSeries", "modelSeries");
    generateMeshService->configure();
    generateMeshService->start().wait();
    generateMeshService->update().wait();
    auto modelSeries = generateMeshService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
#if VTK_MAJOR_VERSION >= 9 && VTK_MINOR_VERSION >= 0 && VTK_BUILD_VERSION > 1
    unsigned int numberPoints = 77;
    unsigned int numberCells  = 125;
#else
    unsigned int numberPoints = 83;
    unsigned int numberCells  = 125;
#endif
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    generateMeshService->stop().wait();
    sight::service::remove(generateMeshService);
}

//------------------------------------------------------------------------------

void SVTKMesherTest::generateMeshWithMinReduction()
{
    // Create service
    sight::service::IService::sptr generateMeshService = sight::service::add("sight::module::filter::mesh::SVTKMesher");
    CPPUNIT_ASSERT(generateMeshService);
    typedef std::int16_t Type;
    data::Image::sptr image            = data::Image::New();
    const data::Image::Size SIZE       = {10, 20, 90};
    const data::Image::Spacing spacing = {1., 1., 1.};
    const data::Image::Origin origin   = {0., 0., 0.};
    utestData::generator::Image::generateImage(
        image,
        SIZE,
        spacing,
        origin,
        core::Type::get<Type>(),
        data::Image::PixelFormat::GRAY_SCALE
    );

    [[maybe_unused]] const auto dumpLock = image->dump_lock();
    for(std::size_t x = 0 ; x < SIZE[0] ; ++x)
    {
        for(std::size_t y = 0 ; y < SIZE[1] ; ++y)
        {
            for(std::size_t z = 0 ; z < SIZE[2] ; ++z)
            {
                const auto index = x + y * SIZE[0] + z * SIZE[0] * SIZE[1];

                if(x < 7 && y < 7 && z < 7)
                {
                    std::int16_t val = 0;

                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&val));
                    CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
                else
                {
                    std::int16_t threshold = 255;
                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&threshold));
                    CPPUNIT_ASSERT_EQUAL(threshold, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
            }
        }
    }

    const std::filesystem::path path    = core::tools::System::getTemporaryFolder() / "meshTest.vtk";
    io::vtk::ImageWriter::sptr myWriter = io::vtk::ImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFile(path);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
    // create an empty image series
    sight::data::ImageSeries::sptr imageSeries = sight::data::ImageSeries::New();
    imageSeries->setImage(image);
    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"0\" threshold=\"255\"/>"
    ;
    boost::property_tree::read_xml(config_string, config);
    generateMeshService->setConfiguration(config);
    generateMeshService->setInput(imageSeries, "imageSeries");
    generateMeshService->setObjectId("modelSeries", "modelSeries");
    generateMeshService->configure();
    generateMeshService->start().wait();
    generateMeshService->update().wait();
    auto modelSeries          = generateMeshService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
    unsigned int numberPoints = 147;
    unsigned int numberCells  = 253;
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    generateMeshService->stop().wait();
    sight::service::remove(generateMeshService);
}

//------------------------------------------------------------------------------

void SVTKMesherTest::noMeshGenerated()
{
    // Create service
    sight::service::IService::sptr generateMeshService = sight::service::add("sight::module::filter::mesh::SVTKMesher");
    CPPUNIT_ASSERT(generateMeshService);
    typedef std::int16_t Type;
    data::Image::sptr image = data::Image::New();

    const data::Image::Size SIZE       = {10, 20, 90};
    const data::Image::Spacing spacing = {1., 1., 1.};
    const data::Image::Origin origin   = {0., 0., 0.};
    utestData::generator::Image::generateImage(
        image,
        SIZE,
        spacing,
        origin,
        core::Type::get<Type>(),
        data::Image::PixelFormat::GRAY_SCALE
    );

    [[maybe_unused]] const auto dumpLock = image->dump_lock();
    for(std::size_t x = 0 ; x < SIZE[0] ; ++x)
    {
        for(std::size_t y = 0 ; y < SIZE[1] ; ++y)
        {
            for(std::size_t z = 0 ; z < SIZE[2] ; ++z)
            {
                const auto index = x + y * SIZE[0] + z * SIZE[0] * SIZE[1];

                if(x < 7 && y < 7 && z < 7)
                {
                    std::int16_t val = 0;

                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&val));
                    CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
                else
                {
                    std::int16_t threshold = 255;
                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&threshold));
                    CPPUNIT_ASSERT_EQUAL(threshold, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
            }
        }
    }

    const std::filesystem::path path    = core::tools::System::getTemporaryFolder() / "meshTest.vtk";
    io::vtk::ImageWriter::sptr myWriter = io::vtk::ImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFile(path);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
    // create an empty image series
    sight::data::ImageSeries::sptr imageSeries = sight::data::ImageSeries::New();
    imageSeries->setImage(image);
    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"90\" threshold=\"30\"/>"
    ;
    boost::property_tree::read_xml(config_string, config);
    generateMeshService->setConfiguration(config);
    generateMeshService->setInput(imageSeries, "imageSeries");
    generateMeshService->setObjectId("modelSeries", "modelSeries");
    generateMeshService->configure();
    generateMeshService->start().wait();
    generateMeshService->update().wait();
    auto modelSeries          = generateMeshService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
    unsigned int numberPoints = 0;
    unsigned int numberCells  = 0;
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    generateMeshService->stop().wait();
    sight::service::remove(generateMeshService);
}

//------------------------------------------------------------------------------

void SVTKMesherTest::updateThresholdTest()
{
    // Create service
    sight::service::IService::sptr generateMeshService = sight::service::add("sight::module::filter::mesh::SVTKMesher");
    CPPUNIT_ASSERT(generateMeshService);

    //generate Image
    typedef std::int16_t Type;
    data::Image::sptr image            = data::Image::New();
    const data::Image::Size SIZE       = {10, 20, 90};
    const data::Image::Spacing spacing = {1., 1., 1.};
    const data::Image::Origin origin   = {0., 0., 0.};
    utestData::generator::Image::generateImage(
        image,
        SIZE,
        spacing,
        origin,
        core::Type::get<Type>(),
        data::Image::PixelFormat::GRAY_SCALE
    );

    [[maybe_unused]] const auto dumpLock = image->dump_lock();
    for(std::size_t x = 0 ; x < SIZE[0] ; ++x)
    {
        for(std::size_t y = 0 ; y < SIZE[1] ; ++y)
        {
            for(std::size_t z = 0 ; z < SIZE[2] ; ++z)
            {
                const auto index = x + y * SIZE[0] + z * SIZE[0] * SIZE[1];

                if(x < 7 && y < 7 && z < 7)
                {
                    std::int16_t val = 0;

                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&val));
                    CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
                else
                {
                    std::int16_t threshold = 255;
                    image->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&threshold));
                    CPPUNIT_ASSERT_EQUAL(threshold, *reinterpret_cast<const std::int16_t*>(image->getPixel(index)));
                }
            }
        }
    }

    const std::filesystem::path path    = core::tools::System::getTemporaryFolder() / "meshTest.vtk";
    io::vtk::ImageWriter::sptr myWriter = io::vtk::ImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFile(path);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
    // create an empty image series
    sight::data::ImageSeries::sptr imageSeries = sight::data::ImageSeries::New();
    imageSeries->setImage(image);
    service::IService::ConfigType config;
    std::stringstream config_string;

    //threshold is set to 255 by the configuration
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"0\" threshold=\"255\"/>"
    ;
    boost::property_tree::read_xml(config_string, config);
    generateMeshService->setConfiguration(config);
    generateMeshService->setInput(imageSeries, "imageSeries");
    generateMeshService->setObjectId("modelSeries", "modelSeries");
    generateMeshService->configure();

    //threshold is modified by the slot updateThreshold
    const int newThreshold = 50;
    generateMeshService->slot("updateThreshold")->run(newThreshold);

    generateMeshService->start().wait();
    generateMeshService->update().wait();
    auto modelSeries          = generateMeshService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
    unsigned int numberPoints = 0;
    unsigned int numberCells  = 0;
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    generateMeshService->stop().wait();
    sight::service::remove(generateMeshService);
}

} //namespace ut

} //namespace sight::module::filter::mesh

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

#include <core/runtime/runtime.hpp>
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

namespace sight::module::filter::mesh::ut
{

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

inline static std::pair<sight::service::IService::sptr, sight::data::ImageSeries::sptr> generateMeshService()
{
    // Create service
    sight::service::IService::sptr generateMeshService = sight::service::add("sight::module::filter::mesh::SVTKMesher");
    CPPUNIT_ASSERT(generateMeshService);

    auto imageSeries    = sight::data::ImageSeries::New();
    const auto dumpLock = imageSeries->dump_lock();

    const data::Image::Size size       = {10, 20, 90};
    const data::Image::Spacing spacing = {1., 1., 1.};
    const data::Image::Origin origin   = {0., 0., 0.};

    utestData::generator::Image::generateImage(
        imageSeries,
        size,
        spacing,
        origin,
        core::Type::get<std::int16_t>(),
        data::Image::PixelFormat::GRAY_SCALE
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

                    imageSeries->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&val));
                    CPPUNIT_ASSERT_EQUAL(
                        val,
                        *reinterpret_cast<const std::int16_t*>(imageSeries->getPixel(index))
                    );
                }
                else
                {
                    std::int16_t threshold = 255;
                    imageSeries->setPixel(index, reinterpret_cast<data::Image::BufferType*>(&threshold));
                    CPPUNIT_ASSERT_EQUAL(
                        threshold,
                        *reinterpret_cast<const std::int16_t*>(imageSeries->getPixel(index))
                    );
                }
            }
        }
    }

    const std::filesystem::path path    = core::tools::System::getTemporaryFolder() / "meshTest.vtk";
    io::vtk::ImageWriter::sptr myWriter = io::vtk::ImageWriter::New();
    myWriter->setObject(imageSeries);
    myWriter->setFile(path);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    return {generateMeshService, imageSeries};
}

//------------------------------------------------------------------------------

void SVTKMesherTest::generateMesh()
{
    // Create service
    auto [mesherService, imageSeries] = generateMeshService();

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"50\" threshold=\"255\"/>";

    boost::property_tree::read_xml(config_string, config);
    mesherService->setConfiguration(config);
    mesherService->setInput(imageSeries, "imageSeries");
    mesherService->setObjectId("modelSeries", "modelSeries");
    mesherService->configure();
    mesherService->start().wait();
    mesherService->update().wait();
    auto modelSeries          = mesherService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
    unsigned int numberPoints = 77;
    unsigned int numberCells  = 125;
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    mesherService->stop().wait();
    sight::service::remove(mesherService);
}

//------------------------------------------------------------------------------

void SVTKMesherTest::generateMeshWithMinReduction()
{
    // Create service
    auto [mesherService, imageSeries] = generateMeshService();

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"0\" threshold=\"255\"/>";

    boost::property_tree::read_xml(config_string, config);
    mesherService->setConfiguration(config);
    mesherService->setInput(imageSeries, "imageSeries");
    mesherService->setObjectId("modelSeries", "modelSeries");
    mesherService->configure();
    mesherService->start().wait();
    mesherService->update().wait();
    auto modelSeries          = mesherService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
    unsigned int numberPoints = 147;
    unsigned int numberCells  = 253;
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    mesherService->stop().wait();
    sight::service::remove(mesherService);
}

//------------------------------------------------------------------------------

void SVTKMesherTest::noMeshGenerated()
{
    // Create service
    auto [mesherService, imageSeries] = generateMeshService();

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"90\" threshold=\"30\"/>";

    boost::property_tree::read_xml(config_string, config);
    mesherService->setConfiguration(config);
    mesherService->setInput(imageSeries, "imageSeries");
    mesherService->setObjectId("modelSeries", "modelSeries");
    mesherService->configure();
    mesherService->start().wait();
    mesherService->update().wait();
    auto modelSeries          = mesherService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
    unsigned int numberPoints = 0;
    unsigned int numberCells  = 0;
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    mesherService->stop().wait();
    sight::service::remove(mesherService);
}

//------------------------------------------------------------------------------

void SVTKMesherTest::updateThresholdTest()
{
    // Create service
    auto [mesherService, imageSeries] = generateMeshService();
    service::IService::ConfigType config;
    std::stringstream config_string;

    //threshold is set to 255 by the configuration
    config_string
    << "<in key=\"imageSeries\" uid=\"imageSeries\"/>"
       "<out key=\"modelSeries\" uid=\"modelSeries\"/>"
       "<config percentReduction=\"0\" threshold=\"255\"/>";

    boost::property_tree::read_xml(config_string, config);
    mesherService->setConfiguration(config);
    mesherService->setInput(imageSeries, "imageSeries");
    mesherService->setObjectId("modelSeries", "modelSeries");
    mesherService->configure();

    //threshold is modified by the slot updateThreshold
    const int newThreshold = 50;
    mesherService->slot("updateThreshold")->run(newThreshold);

    mesherService->start().wait();
    mesherService->update().wait();
    auto modelSeries          = mesherService->getOutput<sight::data::ModelSeries>("modelSeries").lock();
    unsigned int numberPoints = 0;
    unsigned int numberCells  = 0;
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numPoints(), numberPoints);
    CPPUNIT_ASSERT_EQUAL(modelSeries->getReconstructionDB()[0]->getMesh()->numCells(), numberCells);
    mesherService->stop().wait();
    sight::service::remove(mesherService);
}

} // namespace sight::module::filter::mesh::ut

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

#include "SGetCameraTest.hpp"

#include "data/Exception.hpp"

#include <core/service/base.hpp>

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

#include <service/IService.hpp>

#include <boost/config.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SGetCameraTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SGetCameraTest::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::loadModule(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void SGetCameraTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsCameraFromCameraSeries()
{
    // Create a camera series.
    sight::data::CameraSeries::sptr cameraSeries = sight::data::CameraSeries::New();
    // create service
    sight::service::IService::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // ------------------ create a matrix ----------------------
    sight::data::Matrix4::sptr matrix = sight::data::Matrix4::New();
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = sight::data::Camera::New();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = sight::data::Camera::New();

    cameraSeries->addCamera(camera1);
    cameraSeries->addCamera(camera2);
    CPPUNIT_ASSERT_EQUAL(cameraSeries->numCameras(), std::size_t(2));
    // -------------Set extrinsic Matrix-----------------
    cameraSeries->setExtrinsicMatrix(0, matrix);

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>"
       "<out group=\"extrinsic\">"
       "<key index=\"0\" uid=\"extrinsic\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInput(cameraSeries, "cameraSeries");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_NO_THROW(getCameraSrv->update().get());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 0).lock()->getID(), camera1->getID());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 1).lock()->getID(), camera2->getID());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("extrinsic", 0).lock()->getID(), matrix->getID());

    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsInvalidCameraSeries()
{
    // create service
    sight::service::IService::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>"
       "<out group=\"extrinsic\">"
       "<key index=\"0\" uid=\"extrinsic\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInput(nullptr, "cameraSeries");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_THROW(getCameraSrv->update().get(), sight::data::Exception);
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsValidExtrinsic()
{
    // // Create a camera series.
    sight::data::CameraSeries::sptr cameraSeries = sight::data::CameraSeries::New();
    // create service
    sight::service::IService::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = sight::data::Camera::New();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = sight::data::Camera::New();
    cameraSeries->addCamera(camera1);
    cameraSeries->addCamera(camera2);
    CPPUNIT_ASSERT_EQUAL(cameraSeries->numCameras(), std::size_t(2));

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInput(cameraSeries, "cameraSeries");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_NO_THROW(getCameraSrv->update().get());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 0).lock()->getID(), camera1->getID());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 1).lock()->getID(), camera2->getID());
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsValidExtrinsic1()
{
    // // Create a camera series.
    sight::data::CameraSeries::sptr cameraSeries = sight::data::CameraSeries::New();
    // create service
    sight::service::IService::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = sight::data::Camera::New();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = sight::data::Camera::New();
    // ------------------ create a matrix ----------------------
    sight::data::Matrix4::sptr matrix = sight::data::Matrix4::New();

    cameraSeries->addCamera(camera1);
    cameraSeries->addCamera(camera2);
    CPPUNIT_ASSERT_EQUAL(cameraSeries->numCameras(), std::size_t(2));
    cameraSeries->setExtrinsicMatrix(0, matrix);
    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInput(cameraSeries, "cameraSeries");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_NO_THROW(getCameraSrv->update().get());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 0).lock()->getID(), camera1->getID());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 1).lock()->getID(), camera2->getID());
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsCameraOutOfBoundIndex()
{
    // Create a camera series.
    sight::data::CameraSeries::sptr cameraSeries = sight::data::CameraSeries::New();
    // create service
    sight::service::IService::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = sight::data::Camera::New();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = sight::data::Camera::New();

    cameraSeries->addCamera(camera1);
    cameraSeries->addCamera(camera2);
    CPPUNIT_ASSERT_EQUAL(cameraSeries->numCameras(), std::size_t(2));

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"5\" uid=\"RGBCamera\"/>"
       "</out>"
       "<out group=\"extrinsic\">"
       "<key index=\"1\" uid=\"extrinsic\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInput(cameraSeries, "cameraSeries");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_THROW(getCameraSrv->update().get(), core::Exception);
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsExtrinsicOutOfBoundIndex()
{
    // Create a camera series.
    sight::data::CameraSeries::sptr cameraSeries = sight::data::CameraSeries::New();
    // create service
    sight::service::IService::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");

    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = sight::data::Camera::New();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = sight::data::Camera::New();
    cameraSeries->addCamera(camera1);
    cameraSeries->addCamera(camera2);
    CPPUNIT_ASSERT_EQUAL(cameraSeries->numCameras(), std::size_t(2));

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>"
       "<out group=\"extrinsic\">"
       "<key index=\"5\" uid=\"extrinsic\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInput(cameraSeries, "cameraSeries");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_THROW(getCameraSrv->update().get(), core::Exception);
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsInvalidExtrinsic()
{
    // Create a camera series.
    sight::data::CameraSeries::sptr cameraSeries = sight::data::CameraSeries::New();
    // create service
    sight::service::IService::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = sight::data::Camera::New();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = sight::data::Camera::New();

    cameraSeries->addCamera(camera1);
    cameraSeries->addCamera(camera2);
    CPPUNIT_ASSERT_EQUAL(cameraSeries->numCameras(), std::size_t(2));

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>"
       "<out group=\"extrinsic\">"
       "<key index=\"1\" uid=\"extrinsic\"/>"
       "</out>"
    ;
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInput(cameraSeries, "cameraSeries");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_THROW(getCameraSrv->update().get(), core::Exception);
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 0).lock()->getID(), camera1->getID());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->getOutput<sight::data::Object>("camera", 1).lock()->getID(), camera2->getID());

    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut

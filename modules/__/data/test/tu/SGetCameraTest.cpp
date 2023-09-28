/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <core/runtime/runtime.hpp>

#include <data/Camera.hpp>
#include <data/CameraSet.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

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
    auto module = core::runtime::load_module(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void SGetCameraTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsCameraFromCameraSet()
{
    // Create a camera series.
    sight::data::CameraSet::sptr camera_set = std::make_shared<sight::data::CameraSet>();
    // create service
    sight::service::base::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // ------------------ create a matrix ----------------------
    sight::data::Matrix4::sptr matrix = std::make_shared<sight::data::Matrix4>();
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = std::make_shared<sight::data::Camera>();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = std::make_shared<sight::data::Camera>();

    camera_set->add_camera(camera1);
    camera_set->add_camera(camera2);
    CPPUNIT_ASSERT_EQUAL(camera_set->size(), std::size_t(2));
    // -------------Set extrinsic Matrix-----------------
    camera_set->set_extrinsic_matrix(0, matrix);

    service::config_t config;
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
    getCameraSrv->setInOut(camera_set, "cameraSet");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_NO_THROW(getCameraSrv->update().get());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 0).lock()->get_id(), camera1->get_id());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 1).lock()->get_id(), camera2->get_id());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("extrinsic", 0).lock()->get_id(), matrix->get_id());

    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsInvalidCameraSet()
{
    // create service
    sight::service::base::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    service::config_t config;
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
    getCameraSrv->setInOut(nullptr, "cameraSet");
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
    sight::data::CameraSet::sptr camera_set = std::make_shared<sight::data::CameraSet>();
    // create service
    sight::service::base::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = std::make_shared<sight::data::Camera>();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = std::make_shared<sight::data::Camera>();
    camera_set->add_camera(camera1);
    camera_set->add_camera(camera2);
    CPPUNIT_ASSERT_EQUAL(camera_set->size(), std::size_t(2));

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInOut(camera_set, "cameraSet");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_NO_THROW(getCameraSrv->update().get());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 0).lock()->get_id(), camera1->get_id());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 1).lock()->get_id(), camera2->get_id());
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsValidExtrinsic1()
{
    // // Create a camera series.
    sight::data::CameraSet::sptr camera_set = std::make_shared<sight::data::CameraSet>();
    // create service
    sight::service::base::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = std::make_shared<sight::data::Camera>();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = std::make_shared<sight::data::Camera>();
    // ------------------ create a matrix ----------------------
    sight::data::Matrix4::sptr matrix = std::make_shared<sight::data::Matrix4>();

    camera_set->add_camera(camera1);
    camera_set->add_camera(camera2);
    CPPUNIT_ASSERT_EQUAL(camera_set->size(), std::size_t(2));
    camera_set->set_extrinsic_matrix(0, matrix);
    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"camera\">"
       "<key index=\"0\" uid=\"DCamera\"/>"
       "<key index=\"1\" uid=\"RGBCamera\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);

    getCameraSrv->setConfiguration(config);
    getCameraSrv->setInOut(camera_set, "cameraSet");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_NO_THROW(getCameraSrv->update().get());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 0).lock()->get_id(), camera1->get_id());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 1).lock()->get_id(), camera2->get_id());
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsCameraOutOfBoundIndex()
{
    // Create a camera series.
    auto camera_set = std::make_shared<sight::data::CameraSet>();
    // create service
    auto getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    auto camera1 = std::make_shared<sight::data::Camera>();
    // -- Camera 2 --
    auto camera2 = std::make_shared<sight::data::Camera>();

    camera_set->add_camera(camera1);
    camera_set->add_camera(camera2);
    CPPUNIT_ASSERT_EQUAL(camera_set->size(), std::size_t(2));

    service::config_t config;
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
    getCameraSrv->setInOut(camera_set, "cameraSet");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_THROW(getCameraSrv->update().get(), std::out_of_range);
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsExtrinsicOutOfBoundIndex()
{
    // Create a camera series.
    sight::data::CameraSet::sptr camera_set = std::make_shared<sight::data::CameraSet>();
    // create service
    sight::service::base::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");

    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = std::make_shared<sight::data::Camera>();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = std::make_shared<sight::data::Camera>();
    camera_set->add_camera(camera1);
    camera_set->add_camera(camera2);
    CPPUNIT_ASSERT_EQUAL(camera_set->size(), std::size_t(2));

    service::config_t config;
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
    getCameraSrv->setInOut(camera_set, "cameraSet");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_THROW(getCameraSrv->update().get(), std::out_of_range);
    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

void SGetCameraTest::extractsInvalidExtrinsic()
{
    // Create a camera series.
    sight::data::CameraSet::sptr camera_set = std::make_shared<sight::data::CameraSet>();
    // create service
    sight::service::base::sptr getCameraSrv = sight::service::add("sight::module::data::SGetCamera");
    // -- Camera 1 --
    sight::data::Camera::sptr camera1 = std::make_shared<sight::data::Camera>();
    // -- Camera 2 --
    sight::data::Camera::sptr camera2 = std::make_shared<sight::data::Camera>();

    camera_set->add_camera(camera1);
    camera_set->add_camera(camera2);
    CPPUNIT_ASSERT_EQUAL(camera_set->size(), std::size_t(2));

    service::config_t config;
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
    getCameraSrv->setInOut(camera_set, "cameraSet");
    getCameraSrv->configure();
    getCameraSrv->start().wait();

    CPPUNIT_ASSERT_THROW(getCameraSrv->update().get(), core::exception);
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 0).lock()->get_id(), camera1->get_id());
    CPPUNIT_ASSERT_EQUAL(getCameraSrv->output("camera", 1).lock()->get_id(), camera2->get_id());

    getCameraSrv->stop().wait();
    sight::service::remove(getCameraSrv);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut

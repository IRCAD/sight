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

#include "image_picker_test.hpp"

#include <core/com/slot_base.hxx>

#include <data/camera.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/tools/picking_info.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::video::ut::image_picker_test);

namespace sight::module::ui::video::ut
{

//------------------------------------------------------------------------------

void image_picker_test::setUp()
{
    m_imagePicker = service::add("sight::module::ui::video::image_picker");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::video::image_picker'", m_imagePicker);
}

//------------------------------------------------------------------------------

void image_picker_test::tearDown()
{
    if(!m_imagePicker->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_imagePicker->stop().get());
    }

    service::remove(m_imagePicker);
}

//------------------------------------------------------------------------------

void image_picker_test::basicTest()
{
    auto camera = std::make_shared<data::camera>();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = std::make_shared<data::point_list>();
    auto pixelPointList = std::make_shared<data::point_list>();

    m_imagePicker->set_input(camera, "camera");
    m_imagePicker->set_inout(pointList, "pointList");
    m_imagePicker->set_inout(pixelPointList, "pixelPointList");
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    data::tools::picking_info info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::picking_info::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::picking_info::CTRL
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixelPointList->getPoints()[0]);

    info.m_worldPos = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *pointList->getPoints()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(2., 1.), *pixelPointList->getPoints()[1]);

    // Clicking with the right mouse button should remove the last added point
    info.m_eventId = data::tools::picking_info::Event::MOUSE_RIGHT_DOWN;
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixelPointList->getPoints()[0]);
}

//------------------------------------------------------------------------------

void image_picker_test::clickWithoutControlTest()
{
    auto camera         = std::make_shared<data::camera>();
    auto pointList      = std::make_shared<data::point_list>();
    auto pixelPointList = std::make_shared<data::point_list>();

    m_imagePicker->set_input(camera, "camera");
    m_imagePicker->set_inout(pointList, "pointList");
    m_imagePicker->set_inout(pixelPointList, "pixelPointList");
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    // By default, to do something, the Control modifier key must be held when clicking. If not, nothing happens and the
    // lists are unchanged.
    data::tools::picking_info info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::picking_info::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::picking_info::NONE
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT(pointList->getPoints().empty());
    CPPUNIT_ASSERT(pixelPointList->getPoints().empty());
}

//------------------------------------------------------------------------------

void image_picker_test::topLeftRefTest()
{
    auto camera = std::make_shared<data::camera>();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = std::make_shared<data::point_list>();
    auto pixelPointList = std::make_shared<data::point_list>();

    m_imagePicker->set_input(camera, "camera");
    m_imagePicker->set_inout(pointList, "pointList");
    m_imagePicker->set_inout(pixelPointList, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.videoReference", "top_left");
    m_imagePicker->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    data::tools::picking_info info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::picking_info::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::picking_info::CTRL
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *pixelPointList->getPoints()[0]);

    info.m_worldPos = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *pointList->getPoints()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *pixelPointList->getPoints()[1]);
}

//------------------------------------------------------------------------------

void image_picker_test::singlePointModeTest()
{
    auto camera = std::make_shared<data::camera>();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = std::make_shared<data::point_list>();
    auto pixelPointList = std::make_shared<data::point_list>();

    m_imagePicker->set_input(camera, "camera");
    m_imagePicker->set_inout(pointList, "pointList");
    m_imagePicker->set_inout(pixelPointList, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.singlePointMode", "true");
    m_imagePicker->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    data::tools::picking_info info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::picking_info::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::picking_info::CTRL
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixelPointList->getPoints()[0]);

    // Since the single point mode is enabled, the next click removes the last added point
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT(pointList->getPoints().empty());
    CPPUNIT_ASSERT(pixelPointList->getPoints().empty());

    // Clicking yet another time should add another point
    info.m_worldPos = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(2., 1.), *pixelPointList->getPoints()[0]);
}

//------------------------------------------------------------------------------

void image_picker_test::noCtrlModifierTest()
{
    auto camera = std::make_shared<data::camera>();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = std::make_shared<data::point_list>();
    auto pixelPointList = std::make_shared<data::point_list>();

    m_imagePicker->set_input(camera, "camera");
    m_imagePicker->set_inout(pointList, "pointList");
    m_imagePicker->set_inout(pixelPointList, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.useCtrlModifier", "false");
    m_imagePicker->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    // The point should still be added, event if the control modifier isn't held when clicking, because useCtrlModifier
    // is set to false.
    data::tools::picking_info info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::picking_info::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::picking_info::NONE
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixelPointList->getPoints()[0]);

    // Clicking with the control modifier held should still work.
    info.m_modifierMask = data::tools::picking_info::CTRL;
    info.m_worldPos     = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *pointList->getPoints()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(2., 1.), *pixelPointList->getPoints()[1]);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::video::ut

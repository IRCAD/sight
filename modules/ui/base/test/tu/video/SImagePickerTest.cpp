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

#include "SImagePickerTest.hpp"

#include <core/com/SlotBase.hxx>

#include <data/Camera.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/tools/PickingInfo.hpp>

#include <service/base.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::video::ut::SImagePickerTest);

namespace sight::module::ui::base::video::ut
{

//------------------------------------------------------------------------------

void SImagePickerTest::setUp()
{
    m_imagePicker = service::add("sight::module::ui::base::video::SImagePicker");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::base::video::SImagePicker'", m_imagePicker);
}

//------------------------------------------------------------------------------

void SImagePickerTest::tearDown()
{
    if(!m_imagePicker->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_imagePicker->stop().get());
    }

    service::remove(m_imagePicker);
}

//------------------------------------------------------------------------------

void SImagePickerTest::basicTest()
{
    auto camera = data::Camera::New();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = data::PointList::New();
    auto pixelPointList = data::PointList::New();

    m_imagePicker->setInput(camera, "camera");
    m_imagePicker->setInOut(pointList, "pointList");
    m_imagePicker->setInOut(pixelPointList, "pixelPointList");
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    data::tools::PickingInfo info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::PickingInfo::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::PickingInfo::CTRL
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 2.), *pixelPointList->getPoints()[0]);

    info.m_worldPos = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 1.), *pointList->getPoints()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(2., 1.), *pixelPointList->getPoints()[1]);

    // Clicking with the right mouse button should remove the last added point
    info.m_eventId = data::tools::PickingInfo::Event::MOUSE_RIGHT_DOWN;
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 2.), *pixelPointList->getPoints()[0]);
}

//------------------------------------------------------------------------------

void SImagePickerTest::clickWithoutControlTest()
{
    auto camera         = data::Camera::New();
    auto pointList      = data::PointList::New();
    auto pixelPointList = data::PointList::New();

    m_imagePicker->setInput(camera, "camera");
    m_imagePicker->setInOut(pointList, "pointList");
    m_imagePicker->setInOut(pixelPointList, "pixelPointList");
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    // By default, to do something, the Control modifier key must be held when clicking. If not, nothing happens and the
    // lists are unchanged.
    data::tools::PickingInfo info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::PickingInfo::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::PickingInfo::NONE
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT(pointList->getPoints().empty());
    CPPUNIT_ASSERT(pixelPointList->getPoints().empty());
}

//------------------------------------------------------------------------------

void SImagePickerTest::topLeftRefTest()
{
    auto camera = data::Camera::New();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = data::PointList::New();
    auto pixelPointList = data::PointList::New();

    m_imagePicker->setInput(camera, "camera");
    m_imagePicker->setInOut(pointList, "pointList");
    m_imagePicker->setInOut(pixelPointList, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.videoReference", "top_left");
    m_imagePicker->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    data::tools::PickingInfo info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::PickingInfo::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::PickingInfo::CTRL
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(0., 0.), *pixelPointList->getPoints()[0]);

    info.m_worldPos = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 1.), *pointList->getPoints()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 1.), *pixelPointList->getPoints()[1]);
}

//------------------------------------------------------------------------------

void SImagePickerTest::singlePointModeTest()
{
    auto camera = data::Camera::New();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = data::PointList::New();
    auto pixelPointList = data::PointList::New();

    m_imagePicker->setInput(camera, "camera");
    m_imagePicker->setInOut(pointList, "pointList");
    m_imagePicker->setInOut(pixelPointList, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.singlePointMode", "true");
    m_imagePicker->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    data::tools::PickingInfo info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::PickingInfo::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::PickingInfo::CTRL
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 2.), *pixelPointList->getPoints()[0]);

    // Since the single point mode is enabled, the next click removes the last added point
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT(pointList->getPoints().empty());
    CPPUNIT_ASSERT(pixelPointList->getPoints().empty());

    // Clicking yet another time should add another point
    info.m_worldPos = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 1.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(2., 1.), *pixelPointList->getPoints()[0]);
}

//------------------------------------------------------------------------------

void SImagePickerTest::noCtrlModifierTest()
{
    auto camera = data::Camera::New();
    camera->setWidth(2);
    camera->setHeight(4);
    auto pointList      = data::PointList::New();
    auto pixelPointList = data::PointList::New();

    m_imagePicker->setInput(camera, "camera");
    m_imagePicker->setInOut(pointList, "pointList");
    m_imagePicker->setInOut(pixelPointList, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.useCtrlModifier", "false");
    m_imagePicker->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_imagePicker->start().get());

    // The point should still be added, event if the control modifier isn't held when clicking, because useCtrlModifier
    // is set to false.
    data::tools::PickingInfo info {
        .m_worldPos     = {0, 0, 0},
        .m_eventId      = data::tools::PickingInfo::Event::MOUSE_LEFT_DOWN,
        .m_modifierMask = data::tools::PickingInfo::NONE
    };
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(0., 0.), *pointList->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 2.), *pixelPointList->getPoints()[0]);

    // Clicking with the control modifier held should still work.
    info.m_modifierMask = data::tools::PickingInfo::CTRL;
    info.m_worldPos     = {1, 1, 0};
    m_imagePicker->slot("getInteraction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(1., 1.), *pointList->getPoints()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixelPointList->getPoints().size());
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(2., 1.), *pixelPointList->getPoints()[1]);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::video::ut

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
    m_image_picker = service::add("sight::module::ui::video::image_picker");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::video::image_picker'", m_image_picker);
}

//------------------------------------------------------------------------------

void image_picker_test::tearDown()
{
    if(!m_image_picker->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_image_picker->stop().get());
    }

    service::remove(m_image_picker);
}

//------------------------------------------------------------------------------

void image_picker_test::basic_test()
{
    auto camera = std::make_shared<data::camera>();
    camera->set_width(2);
    camera->set_height(4);
    auto point_list       = std::make_shared<data::point_list>();
    auto pixel_point_list = std::make_shared<data::point_list>();

    m_image_picker->set_input(camera, "camera");
    m_image_picker->set_inout(point_list, "pointList");
    m_image_picker->set_inout(pixel_point_list, "pixelPointList");
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->start().get());

    data::tools::picking_info info {
        .m_world_pos     = {0, 0, 0},
        .m_event_id      = data::tools::picking_info::event::mouse_left_down,
        .m_modifier_mask = data::tools::picking_info::ctrl
    };
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *point_list->get_points()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixel_point_list->get_points()[0]);

    info.m_world_pos = {1, 1, 0};
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *point_list->get_points()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(2., 1.), *pixel_point_list->get_points()[1]);

    // Clicking with the right mouse button should remove the last added point
    info.m_event_id = data::tools::picking_info::event::mouse_right_down;
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *point_list->get_points()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixel_point_list->get_points()[0]);
}

//------------------------------------------------------------------------------

void image_picker_test::click_without_control_test()
{
    auto camera           = std::make_shared<data::camera>();
    auto point_list       = std::make_shared<data::point_list>();
    auto pixel_point_list = std::make_shared<data::point_list>();

    m_image_picker->set_input(camera, "camera");
    m_image_picker->set_inout(point_list, "pointList");
    m_image_picker->set_inout(pixel_point_list, "pixelPointList");
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->start().get());

    // By default, to do something, the Control modifier key must be held when clicking. If not, nothing happens and the
    // lists are unchanged.
    data::tools::picking_info info {
        .m_world_pos     = {0, 0, 0},
        .m_event_id      = data::tools::picking_info::event::mouse_left_down,
        .m_modifier_mask = data::tools::picking_info::none
    };
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT(point_list->get_points().empty());
    CPPUNIT_ASSERT(pixel_point_list->get_points().empty());
}

//------------------------------------------------------------------------------

void image_picker_test::top_left_ref_test()
{
    auto camera = std::make_shared<data::camera>();
    camera->set_width(2);
    camera->set_height(4);
    auto point_list       = std::make_shared<data::point_list>();
    auto pixel_point_list = std::make_shared<data::point_list>();

    m_image_picker->set_input(camera, "camera");
    m_image_picker->set_inout(point_list, "pointList");
    m_image_picker->set_inout(pixel_point_list, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.videoReference", "top_left");
    m_image_picker->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->start().get());

    data::tools::picking_info info {
        .m_world_pos     = {0, 0, 0},
        .m_event_id      = data::tools::picking_info::event::mouse_left_down,
        .m_modifier_mask = data::tools::picking_info::ctrl
    };
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *point_list->get_points()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *pixel_point_list->get_points()[0]);

    info.m_world_pos = {1, 1, 0};
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *point_list->get_points()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *pixel_point_list->get_points()[1]);
}

//------------------------------------------------------------------------------

void image_picker_test::single_point_mode_test()
{
    auto camera = std::make_shared<data::camera>();
    camera->set_width(2);
    camera->set_height(4);
    auto point_list       = std::make_shared<data::point_list>();
    auto pixel_point_list = std::make_shared<data::point_list>();

    m_image_picker->set_input(camera, "camera");
    m_image_picker->set_inout(point_list, "pointList");
    m_image_picker->set_inout(pixel_point_list, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.singlePointMode", "true");
    m_image_picker->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->start().get());

    data::tools::picking_info info {
        .m_world_pos     = {0, 0, 0},
        .m_event_id      = data::tools::picking_info::event::mouse_left_down,
        .m_modifier_mask = data::tools::picking_info::ctrl
    };
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *point_list->get_points()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixel_point_list->get_points()[0]);

    // Since the single point mode is enabled, the next click removes the last added point
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT(point_list->get_points().empty());
    CPPUNIT_ASSERT(pixel_point_list->get_points().empty());

    // Clicking yet another time should add another point
    info.m_world_pos = {1, 1, 0};
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *point_list->get_points()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(2., 1.), *pixel_point_list->get_points()[0]);
}

//------------------------------------------------------------------------------

void image_picker_test::no_ctrl_modifier_test()
{
    auto camera = std::make_shared<data::camera>();
    camera->set_width(2);
    camera->set_height(4);
    auto point_list       = std::make_shared<data::point_list>();
    auto pixel_point_list = std::make_shared<data::point_list>();

    m_image_picker->set_input(camera, "camera");
    m_image_picker->set_inout(point_list, "pointList");
    m_image_picker->set_inout(pixel_point_list, "pixelPointList");
    boost::property_tree::ptree ptree;
    ptree.put("config.<xmlattr>.useCtrlModifier", "false");
    m_image_picker->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->configure());
    CPPUNIT_ASSERT_NO_THROW(m_image_picker->start().get());

    // The point should still be added, event if the control modifier isn't held when clicking, because useCtrlModifier
    // is set to false.
    data::tools::picking_info info {
        .m_world_pos     = {0, 0, 0},
        .m_event_id      = data::tools::picking_info::event::mouse_left_down,
        .m_modifier_mask = data::tools::picking_info::none
    };
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(0., 0.), *point_list->get_points()[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 2.), *pixel_point_list->get_points()[0]);

    // Clicking with the control modifier held should still work.
    info.m_modifier_mask = data::tools::picking_info::ctrl;
    info.m_world_pos     = {1, 1, 0};
    m_image_picker->slot("get_interaction")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(1., 1.), *point_list->get_points()[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pixel_point_list->get_points().size());
    CPPUNIT_ASSERT_EQUAL(*std::make_shared<data::point>(2., 1.), *pixel_point_list->get_points()[1]);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::video::ut

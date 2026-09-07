/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <data/camera.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/tools/picking_info.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <doctest/doctest.h>

#include <memory>

namespace
{

struct image_picker_fixture
{
    image_picker_fixture()
    {
        m_image_picker = sight::service::add("sight::module::ui::video::image_picker");
        REQUIRE_MESSAGE(m_image_picker, "Failed to create service 'sight::module::ui::video::image_picker'");

        m_point_list       = std::make_shared<sight::data::point_list>();
        m_pixel_point_list = std::make_shared<sight::data::point_list>();
    }

    ~image_picker_fixture()
    {
        if(!m_image_picker->stopped())
        {
            CHECK_NOTHROW(m_image_picker->stop().get());
        }

        sight::service::remove(m_image_picker);
    }

    image_picker_fixture(const image_picker_fixture&)            = delete;
    image_picker_fixture& operator=(const image_picker_fixture&) = delete;
    image_picker_fixture(image_picker_fixture&&)                 = delete;
    image_picker_fixture& operator=(image_picker_fixture&&)      = delete;

    //------------------------------------------------------------------------------

    /// Set the service up with a camera of the given size, and the optional configuration attribute.
    void setup(
        std::size_t _width,
        std::size_t _height,
        const std::string& _attribute = {},
        const std::string& _value     = {})
    {
        // Kept as a member: the service only holds a weak reference to its inputs.
        m_camera = std::make_shared<sight::data::camera>();
        m_camera->set_width(_width);
        m_camera->set_height(_height);

        m_image_picker->set_input(m_camera, "input.camera");
        m_image_picker->set_inout(m_point_list, "output.world_points");
        m_image_picker->set_inout(m_pixel_point_list, "output.screen_points");

        if(!_attribute.empty())
        {
            boost::property_tree::ptree ptree;
            ptree.put("config.<xmlattr>." + _attribute, _value);
            m_image_picker->set_config(ptree);
        }

        CHECK_NOTHROW(m_image_picker->configure());
        CHECK_NOTHROW(m_image_picker->start().get());
    }

    //------------------------------------------------------------------------------

    void pick(const sight::data::tools::picking_info& _info) const
    {
        m_image_picker->slot("get_interaction")->run(_info);
    }

    //------------------------------------------------------------------------------

    /// Check that both lists hold the expected number of points, and that the one at _index matches.
    void check_point(
        std::size_t _size,
        std::size_t _index,
        double _x,
        double _y,
        double _pixel_x,
        double _pixel_y
    ) const
    {
        REQUIRE_EQ(_size, m_point_list->size());
        CHECK_EQ(*std::make_shared<sight::data::point>(_x, _y), *(*m_point_list)[_index]);
        REQUIRE_EQ(_size, m_pixel_point_list->size());
        CHECK_EQ(*std::make_shared<sight::data::point>(_pixel_x, _pixel_y), *(*m_pixel_point_list)[_index]);
    }

    sight::service::base::sptr m_image_picker;
    sight::data::camera::sptr m_camera;
    sight::data::point_list::sptr m_point_list;
    sight::data::point_list::sptr m_pixel_point_list;
};

using picking_info = sight::data::tools::picking_info;

} // namespace

TEST_SUITE("sight::module::ui::video::image_picker")
{
    TEST_CASE_FIXTURE(image_picker_fixture, "basic")
    {
        setup(2, 4);

        picking_info info {
            .m_world_pos     = {0, 0, 0},
            .m_event_id      = picking_info::event::mouse_left_down,
            .m_modifier_mask = picking_info::ctrl
        };
        pick(info);
        check_point(1, 0, 0., 0., 1., 2.);

        info.m_world_pos = {1, 1, 0};
        pick(info);
        check_point(2, 1, 1., 1., 2., 1.);

        // Clicking with the right mouse button should remove the last added point
        info.m_event_id = picking_info::event::mouse_right_down;
        pick(info);
        check_point(1, 0, 0., 0., 1., 2.);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(image_picker_fixture, "click_without_control")
    {
        setup(0, 0);

        // By default, to do something, the Control modifier key must be held when clicking. If not, nothing
        // happens and the lists are unchanged.
        pick(
        {
            .m_world_pos     = {0, 0, 0},
            .m_event_id      = picking_info::event::mouse_left_down,
            .m_modifier_mask = picking_info::none
        });

        CHECK(m_point_list->empty());
        CHECK(m_pixel_point_list->empty());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(image_picker_fixture, "top_left_ref")
    {
        setup(2, 4, "videoReference", "top_left");

        picking_info info {
            .m_world_pos     = {0, 0, 0},
            .m_event_id      = picking_info::event::mouse_left_down,
            .m_modifier_mask = picking_info::ctrl
        };
        pick(info);
        check_point(1, 0, 0., 0., 0., 0.);

        info.m_world_pos = {1, 1, 0};
        pick(info);
        check_point(2, 1, 1., 1., 1., 1.);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(image_picker_fixture, "single_point_mode")
    {
        setup(2, 4, "singlePointMode", "true");

        picking_info info {
            .m_world_pos     = {0, 0, 0},
            .m_event_id      = picking_info::event::mouse_left_down,
            .m_modifier_mask = picking_info::ctrl
        };
        pick(info);
        check_point(1, 0, 0., 0., 1., 2.);

        // Since the single point mode is enabled, the next click removes the last added point
        pick(info);
        CHECK(m_point_list->empty());
        CHECK(m_pixel_point_list->empty());

        // Clicking yet another time should add another point
        info.m_world_pos = {1, 1, 0};
        pick(info);
        check_point(1, 0, 1., 1., 2., 1.);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(image_picker_fixture, "no_ctrl_modifier")
    {
        setup(2, 4, "useCtrlModifier", "false");

        // The point should still be added, even if the control modifier isn't held when clicking, because
        // useCtrlModifier is set to false.
        picking_info info {
            .m_world_pos     = {0, 0, 0},
            .m_event_id      = picking_info::event::mouse_left_down,
            .m_modifier_mask = picking_info::none
        };
        pick(info);
        check_point(1, 0, 0., 0., 1., 2.);

        // Clicking with the control modifier held should still work.
        info.m_modifier_mask = picking_info::ctrl;
        info.m_world_pos     = {1, 1, 0};
        pick(info);
        check_point(2, 1, 1., 1., 2., 1.);
    }
}

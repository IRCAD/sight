/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#pragma once

#include <core/runtime/runtime.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <utest/service_fixture.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/render.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>

#include <optional>

namespace sight::module::viz::scene3d::ut
{

struct camera_rotation_fixture : sight::utest::service_fixture
{
    //------------------------------------------------------------------------------

    static std::string load_camera(const std::string& _name)
    {
        sight::core::runtime::init();
        sight::core::runtime::load_module("sight::module::ui::qt");
        sight::core::runtime::load_module("sight::module::viz::scene3d");
        sight::core::runtime::load_module("sight::module::viz::scene3d_qt");
        sight::core::runtime::load_module("sight::module::viz::scene3d::test");
        return "sight::module::viz::scene3d::adaptor::" + _name;
    }

    explicit camera_rotation_fixture(const std::string& _name) :
        sight::utest::service_fixture(load_camera(_name))
    {
        m_service->set_id("rotation_camera");
        m_render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(
            sight::service::add("sight::viz::scene3d::render")
        );
        REQUIRE(m_render);
        m_render->set_inout(m_offscreen, "offscreen");
        m_render->set_config(
            "<inout key='offscreen' uid='output'/>"
            "<scene width='64' height='64' render_mode='manual'>"
            "<layer id='default'><adaptor uid='rotation_camera'/></layer></scene>"
        );
        m_render->configure();
        m_render->start().get();
        m_interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(m_service);
        REQUIRE(m_interactor);
    }

    ~camera_rotation_fixture() override
    {
        if(m_service->started())
        {
            CHECK_NOTHROW(m_service->stop().get());
        }

        if(m_render->started())
        {
            CHECK_NOTHROW(m_render->stop().get());
        }

        sight::service::remove(m_render);
    }

    //------------------------------------------------------------------------------

    void start(std::optional<bool> _follow_view_axes = std::nullopt)
    {
        std::string config = "<config animate='false' mouseRotation='true'/><positions/>";
        if(_follow_view_axes.has_value())
        {
            config += *_follow_view_axes ? "<data follow_view_axes='true'/>" : "<data follow_view_axes='false'/>";
        }

        m_service->set_config(config);
        m_service->configure();
        m_service->start().get();
    }

    //------------------------------------------------------------------------------

    Ogre::SceneNode* camera_node()
    {
        return m_render->layer("default")->get_default_camera()->getParentSceneNode();
    }

    //------------------------------------------------------------------------------

    void set_view_up(const Ogre::Vector3& _axis)
    {
        (*m_view_up)(0, 1) = _axis.x;
        (*m_view_up)(1, 1) = _axis.y;
        (*m_view_up)(2, 1) = _axis.z;
        m_service->set_input(m_view_up, "data.view_up", false);
    }

    //------------------------------------------------------------------------------

    void clear_view_up()
    {
        m_service->set_input(nullptr, "data.view_up", false);
    }

    //------------------------------------------------------------------------------

    static Ogre::Quaternion tilted_orientation()
    {
        return Ogre::Quaternion(Ogre::Degree(60), Ogre::Vector3::UNIT_Z)
               * Ogre::Quaternion(Ogre::Degree(35), Ogre::Vector3::UNIT_X);
    }

    //------------------------------------------------------------------------------

    void check_horizontal_rotation(const Ogre::Vector3& _axis)
    {
        const auto before = camera_node()->getOrientation();
        // A drag across one quarter of the 64-pixel viewport must rotate by 90 degrees.
        const auto expected = Ogre::Quaternion(Ogre::Degree(90), _axis) * before;
        m_interactor->button_press_event(sight::viz::scene3d::interactor::base::left, {}, 32, 32);
        m_interactor->mouse_move_event(sight::viz::scene3d::interactor::base::left, {}, 48, 32, 16, 0);
        m_interactor->button_release_event(sight::viz::scene3d::interactor::base::left, {}, 48, 32);
        const auto actual = camera_node()->getOrientation();
        CHECK((actual.xAxis() - expected.xAxis()).length() == doctest::Approx(0.F).epsilon(0.0001));
        CHECK((actual.yAxis() - expected.yAxis()).length() == doctest::Approx(0.F).epsilon(0.0001));
        CHECK((actual.zAxis() - expected.zAxis()).length() == doctest::Approx(0.F).epsilon(0.0001));
    }

    //------------------------------------------------------------------------------

    void check_rotation_mode(std::optional<bool> _follow_view_axes, bool _with_matrix)
    {
        INFO("follow_view_axes: ", _follow_view_axes.has_value() ? (*_follow_view_axes ? "true" : "false") : "omitted");
        INFO("view_up matrix supplied: ", _with_matrix);
        if(m_service->started())
        {
            m_service->stop().get();
        }

        if(_with_matrix)
        {
            set_view_up(Ogre::Vector3::UNIT_Z);
        }
        else
        {
            clear_view_up();
        }

        start(_follow_view_axes);
        camera_node()->setOrientation(tilted_orientation());
        if(_follow_view_axes.value_or(false))
        {
            check_horizontal_rotation(camera_node()->getOrientation().yAxis());
            // The next event must use the new orientation, rather than a cached up axis.
            camera_node()->setOrientation(Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Z));
            check_horizontal_rotation(camera_node()->getOrientation().yAxis());
        }
        else
        {
            check_horizontal_rotation(_with_matrix ? Ogre::Vector3::UNIT_Z : Ogre::Vector3::NEGATIVE_UNIT_Y);
        }
    }

    //------------------------------------------------------------------------------

    void check_rotation_modes()
    {
        // Reuse the offscreen scene while checking every combination of the two optional inputs.
        check_rotation_mode(std::nullopt, false);
        check_rotation_mode(std::nullopt, true);
        check_rotation_mode(false, false);
        check_rotation_mode(false, true);
        check_rotation_mode(true, false);
        check_rotation_mode(true, true);
    }

    //------------------------------------------------------------------------------

    void check_restart()
    {
        set_view_up(Ogre::Vector3::UNIT_Z);
        start(false);
        camera_node()->setOrientation(tilted_orientation());
        check_horizontal_rotation(Ogre::Vector3::UNIT_Z);
        m_service->stop().get();
        clear_view_up();
        m_service->start().get();
        camera_node()->setOrientation(tilted_orientation());
        check_horizontal_rotation(Ogre::Vector3::NEGATIVE_UNIT_Y);
        m_service->stop().get();
        set_view_up(Ogre::Vector3::UNIT_X);
        m_service->start().get();
        camera_node()->setOrientation(tilted_orientation());
        check_horizontal_rotation(Ogre::Vector3::UNIT_X);
    }

    sight::viz::scene3d::render::sptr m_render;
    sight::data::image::sptr m_offscreen {std::make_shared<sight::data::image>()};
    sight::data::matrix4::sptr m_view_up {std::make_shared<sight::data::matrix4>()};
    std::shared_ptr<sight::viz::scene3d::interactor::base> m_interactor;
};

} // namespace sight::module::viz::scene3d::ut

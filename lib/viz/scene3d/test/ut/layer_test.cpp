/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include <core/runtime/runtime.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/render.hpp>

#include <doctest/doctest.h>

#include <OGRE/OgreRenderTarget.h>

class dummy_render_target : public Ogre::RenderTarget
{
public:

    dummy_render_target()
    {
        this->mWidth  = 640;
        this->mHeight = 400;
    }

    //------------------------------------------------------------------------------

    void copyContentsToMemory(
        const Ogre::Box& /*src*/,
        const Ogre::PixelBox& /*dst*/,
        FrameBuffer /*buffer*/ = FB_AUTO
    ) override
    {
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool requiresTextureFlipping() const override
    {
        return false;
    }
};

TEST_SUITE("sight::viz::scene3d::helper::layer")
{
    TEST_CASE("set_orthographic")
    {
        auto layer = std::make_shared<sight::viz::scene3d::layer>();
        CHECK(!layer->is_orthographic_camera_force());

        layer->set_orthographic_camera(true);
        CHECK(layer->is_orthographic_camera_force());
    }

    TEST_CASE("reset_camera_clipping_range")
    {
        auto offscreen  = std::make_shared<sight::data::image>();
        auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

        sight::service::config_t config;
        const std::string config_string =
            "<inout key='offscreen' uid='dummy' />"
            "<scene>"
            "  <background color='#36393E' />"
            "  <layer id='default' />"
            "</scene>";

        render_srv->set_config(config_string);
        render_srv->set_inout(offscreen, "offscreen");
        render_srv->configure();

        // Yes, surprisingly, we do not start the render service, this is not necessary for this test
        // and would be difficult to achieve because in the end, we need a QApplication (to get the OpenGL context,
        // etc...)

        auto layer          = std::make_shared<sight::viz::scene3d::layer>();
        auto* render_target = new dummy_render_target();

        layer->set_render_service(render_srv);
        layer->set_render_target(render_target);
        layer->set_has_default_light(false);
        layer->create_scene();

        auto* object = layer->get_scene_manager()->createManualObject("object");
        object->begin("Default", Ogre::RenderOperation::OT_LINE_LIST, sight::viz::scene3d::RESOURCE_GROUP);
        object->position(Ogre::Vector3(1.0, 1.0, 100.0));
        object->position(Ogre::Vector3(0.0, 0.0, 0.0));
        object->end();

        layer->get_scene_manager()->getRootSceneNode()->attachObject(object);
        layer->reset_camera_coordinates();
        layer->reset_camera_clipping_range();

        auto* const camera            = layer->get_default_camera();
        static const double s_EPSILON = 1e-2F;

        CHECK(doctest::Approx(camera->getNearClipDistance()).epsilon(s_EPSILON) == 35.01);
        CHECK(doctest::Approx(camera->getFarClipDistance()).epsilon(s_EPSILON) == 165.01);

        // It seems Ogre doesn't reset the bounding box when calling beginUpdate !
        object->setBoundingBox(Ogre::AxisAlignedBox::BOX_NULL);
        object->beginUpdate(0);
        object->position(Ogre::Vector3(100.0, 1.0, 1.0));
        object->position(Ogre::Vector3(0.0, 0.0, 0.0));
        object->end();

        camera->getParentSceneNode()->setPosition(600.0, 200.0, 200.0);
        camera->getParentSceneNode()->setDirection(-1.0, 0.0, 0.0);
        layer->reset_camera_clipping_range();

        CHECK(doctest::Approx(camera->getNearClipDistance()).epsilon(s_EPSILON) == 485);
        CHECK(doctest::Approx(camera->getFarClipDistance()).epsilon(s_EPSILON) == 615);

        layer->destroy_scene();
        sight::service::remove(render_srv);
    }
}

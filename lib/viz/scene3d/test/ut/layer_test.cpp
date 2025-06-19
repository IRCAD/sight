/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "layer_test.hpp"

#include <core/runtime/runtime.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/render.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <OGRE/OgreRenderTarget.h>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::layer_test);

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
        FrameBuffer  /*buffer*/ = FB_AUTO
    ) override
    {
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool requiresTextureFlipping() const override
    {
        return false;
    }
};

namespace sight::viz::scene3d::helper::ut
{

//------------------------------------------------------------------------------

void layer_test::setUp()
{
    // sight::core::runtime::load_module("sight::module::viz::scene3d_qt");
}

//------------------------------------------------------------------------------

void layer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void layer_test::set_orthographic()
{
    auto layer = std::make_shared<sight::viz::scene3d::layer>();

    CPPUNIT_ASSERT(!layer->is_orthographic_camera_force());

    layer->set_orthographic_camera(true);

    CPPUNIT_ASSERT(layer->is_orthographic_camera_force());
}

//------------------------------------------------------------------------------

void layer_test::reset_camera_clipping_range()
{
    auto offscreen  = std::make_shared<sight::data::image>();
    auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

    service::config_t config;
    std::stringstream config_string;
    config_string
    << R"(<inout key="offscreen" uid="dummy" />)"
    << R"(<scene>)"
       R"(    <background color="#36393E" />)"
       R"(    <layer id="default" >)"
       R"(    </layer>)"
       R"(</scene>)";
    boost::property_tree::read_xml(config_string, config);
    render_srv->set_config(config);
    render_srv->set_inout(offscreen, "offscreen");
    render_srv->configure();

    // Yes, surprisingly, we do not start the render service, this is not necessary for this test
    // and would be difficult to achieve because in the end, we need a QApplication (to get the OpenGL context, etc...)

    auto layer = std::make_shared<sight::viz::scene3d::layer>();

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

    auto* const camera = layer->get_default_camera();

    static const double s_EPSILON = 1e-2F;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.01, camera->getNearClipDistance(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(165.01, camera->getFarClipDistance(), s_EPSILON);

    // It seems Ogre doesn't reset the bounding box when calling beginUpdate !
    object->setBoundingBox(Ogre::AxisAlignedBox::BOX_NULL);
    object->beginUpdate(0);
    object->position(Ogre::Vector3(100.0, 1.0, 1.0));
    object->position(Ogre::Vector3(0.0, 0.0, 0.0));
    object->end();

    camera->getParentSceneNode()->setPosition(600.0, 200.0, 200.0);
    camera->getParentSceneNode()->setDirection(-1.0, 0.0, 0.0);
    layer->reset_camera_clipping_range();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(485, camera->getNearClipDistance(), s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(615, camera->getFarClipDistance(), s_EPSILON);

    layer->destroy_scene();

    sight::service::remove(render_srv);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut

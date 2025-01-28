/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/axis.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>

#include <data/color.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/render.hpp>

#include <OgreNode.h>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

axis::axis() noexcept
{
    new_slot("update_origin_color", &axis::update_origin_color, this);
}

//-----------------------------------------------------------------------------

void axis::update_origin_color(sight::data::color::sptr _new_color)
{
    if(m_origin == nullptr)
    {
        return;
    }

    Ogre::ColourValue color((*_new_color)[0], (*_new_color)[1], (*_new_color)[2], (*_new_color)[3]);
    m_origin_material->material()->setAmbient(color);
}

//-----------------------------------------------------------------------------

void axis::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    const std::string transform_id = config.get<std::string>(
        module::viz::scene3d::adaptor::transform::TRANSFORM_CONFIG,
        gen_id("transform")
    );

    this->set_transform_id(transform_id);

    static const std::string s_LENGTH_CONFIG    = CONFIG + "length";
    static const std::string s_LABEL_CONFIG     = CONFIG + "label";
    static const std::string s_FONT_SIZE_CONFIG = CONFIG + "fontSize";
    static const std::string s_ORIGIN_CONFIG    = CONFIG + "origin";
    static const std::string s_AXIS_CONFIG      = CONFIG + "axis";
    static const std::string s_AXIS_NAME        = CONFIG + "name";

    m_length            = config.get<float>(s_LENGTH_CONFIG, m_length);
    m_enable_label      = config.get<bool>(s_LABEL_CONFIG, m_enable_label);
    m_font_size         = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_origin_visibility = config.get<bool>(s_ORIGIN_CONFIG, m_origin_visibility);
    m_axis_visibility   = config.get<bool>(s_AXIS_CONFIG, m_axis_visibility);
    m_axis_name         = config.get<std::string>(s_AXIS_NAME, m_axis_name);

    // Force disable label if axisVisibility is false.
    m_enable_label = m_axis_visibility ? m_enable_label : false;
}

//-----------------------------------------------------------------------------

void axis::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    Ogre::SceneNode* const root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* const transform_node  = this->get_or_create_transform_node(root_scene_node);
    m_scene_node = transform_node->createChildSceneNode(gen_id("main_node"));

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    if(m_origin_visibility)
    {
        m_origin = scene_mgr->createManualObject(gen_id("origin"));
    }

    if(m_axis_visibility)
    {
        m_x_line = scene_mgr->createManualObject(gen_id("x_line"));
        m_y_line = scene_mgr->createManualObject(gen_id("y_line"));
        m_z_line = scene_mgr->createManualObject(gen_id("z_line"));

        m_x_cone = scene_mgr->createManualObject(gen_id("x_cone"));
        m_y_cone = scene_mgr->createManualObject(gen_id("y_cone"));
        m_z_cone = scene_mgr->createManualObject(gen_id("z_cone"));
    }

    const auto material_name = gen_id("material");
    m_material = std::make_unique<sight::viz::scene3d::material::standard>(material_name);
    m_material->set_layout(data::mesh::attribute::point_normals | data::mesh::attribute::point_colors);
    m_material->set_shading(data::material::shading_t::phong, this->layer()->num_lights());

    const auto origin_material_name = gen_id("origin_material");
    m_origin_material = std::make_unique<sight::viz::scene3d::material::standard>(origin_material_name);
    m_origin_material->set_layout(data::mesh::attribute::point_normals | data::mesh::attribute::point_colors);
    m_origin_material->set_shading(data::material::shading_t::ambient, this->layer()->num_lights());

    // Sizes
    const float origin_radius   = m_length * 0.1F;
    const float cylinder_length = m_length * 0.85F;
    const float cylinder_radius = m_length * 0.025F;
    const float cone_length     = m_length - cylinder_length;
    const float cone_radius     = cylinder_radius * 2.F;
    const unsigned sample       = 64;

    // origin
    if(m_origin_visibility)
    {
        const auto origin_color = *m_origin_color;
        sight::viz::scene3d::helper::manual_object::create_sphere(
            m_origin,
            m_origin_material->name(),
            Ogre::ColourValue(origin_color[0], origin_color[1], origin_color[2], origin_color[3]),
            origin_radius,
            sample
        );

        m_scene_node->attachObject(m_origin);
    }

    Ogre::Camera* const cam = this->layer()->get_default_camera();

    if(m_axis_visibility)
    {
        // X axis
        sight::viz::scene3d::helper::manual_object::create_cylinder(
            m_x_line,
            m_material->name(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const x_line_node = m_scene_node->createChildSceneNode();
        x_line_node->attachObject(m_x_line);
        x_line_node->pitch(Ogre::Degree(90));

        // Y axis
        sight::viz::scene3d::helper::manual_object::create_cylinder(
            m_y_line,
            m_material->name(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const y_line_node = m_scene_node->createChildSceneNode();
        y_line_node->attachObject(m_y_line);
        y_line_node->roll(Ogre::Degree(90));

        // Z axis
        sight::viz::scene3d::helper::manual_object::create_cylinder(
            m_z_line,
            m_material->name(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const z_line_node = m_scene_node->createChildSceneNode();
        z_line_node->attachObject(m_z_line);
        z_line_node->yaw(Ogre::Degree(-90));

        // X cone
        sight::viz::scene3d::helper::manual_object::create_cone(
            m_x_cone,
            m_material->name(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const x_cone_node = m_scene_node->createChildSceneNode();

        if(m_enable_label)
        {
            m_axis_labels[0] = sight::viz::scene3d::text::make(this->layer());
            m_axis_labels[0]->set_text("X");
            m_axis_labels[0]->set_font_size(m_font_size);
            m_axis_labels[0]->attach_to_node(x_cone_node, this->layer()->get_default_camera());
        }

        x_cone_node->attachObject(m_x_cone);
        x_cone_node->translate(cylinder_length, 0.F, 0.F);

        // Y cone
        sight::viz::scene3d::helper::manual_object::create_cone(
            m_y_cone,
            m_material->name(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const y_cone_node = m_scene_node->createChildSceneNode();
        y_cone_node->attachObject(m_y_cone);

        if(m_enable_label)
        {
            m_axis_labels[1] = sight::viz::scene3d::text::make(this->layer());
            m_axis_labels[1]->set_text("Y");
            m_axis_labels[1]->set_font_size(m_font_size);
            m_axis_labels[1]->attach_to_node(y_cone_node, cam);
        }

        y_cone_node->translate(0.F, cylinder_length, 0.F);
        y_cone_node->roll(Ogre::Degree(90));

        // Z cone
        sight::viz::scene3d::helper::manual_object::create_cone(
            m_z_cone,
            m_material->name(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const z_cone_node = m_scene_node->createChildSceneNode();
        z_cone_node->attachObject(m_z_cone);

        if(m_enable_label)
        {
            m_axis_labels[2] = sight::viz::scene3d::text::make(this->layer());
            m_axis_labels[2]->set_text("Z");
            m_axis_labels[2]->set_font_size(m_font_size);
            m_axis_labels[2]->attach_to_node(z_cone_node, this->layer()->get_default_camera());
        }

        z_cone_node->translate(0.F, 0.F, cylinder_length);
        z_cone_node->yaw(Ogre::Degree(-90));
    }

    // Display Name if provided.
    if(!m_axis_name.empty())
    {
        m_axis_name_txt = sight::viz::scene3d::text::make(this->layer());
        m_axis_name_txt->set_text(m_axis_name);
        m_axis_name_txt->set_font_size(m_font_size);
        m_axis_name_txt->attach_to_node(m_scene_node, cam);

        const auto origin_color = *m_origin_color;
        m_axis_name_txt->set_text_color(Ogre::ColourValue(origin_color[0], origin_color[1], origin_color[2]));
    }

    this->apply_visibility();

    this->request_render();
}

//-----------------------------------------------------------------------------

void axis::updating()
{
    this->render_service()->make_current();

    Ogre::SceneNode* const transform_node = this->get_transform_node();
    if(transform_node != nullptr)
    {
        bool render_requested = false;

        bool has_image = false;
        data::image::origin_t position {0.0, 0.0, 0.0};
        data::image::orientation_t orientation {
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0
        };

        {
            const auto image = m_image.lock();
            if(image)
            {
                has_image   = true;
                position    = image->origin();
                orientation = image->orientation();
            }
        }
        if(has_image)
        {
            // Decompose the matrix

            transform_node->setOrientation(
                Ogre::Matrix3 {
                    static_cast<Ogre::Real>(orientation[0]),
                    static_cast<Ogre::Real>(orientation[1]),
                    static_cast<Ogre::Real>(orientation[2]),
                    static_cast<Ogre::Real>(orientation[3]),
                    static_cast<Ogre::Real>(orientation[4]),
                    static_cast<Ogre::Real>(orientation[5]),
                    static_cast<Ogre::Real>(orientation[6]),
                    static_cast<Ogre::Real>(orientation[7]),
                    static_cast<Ogre::Real>(orientation[8])
                });
            transform_node->setPosition(
                Ogre::Vector3 {static_cast<Ogre::Real>(position[0]), static_cast<Ogre::Real>(position[1]),
                               static_cast<Ogre::Real>(position[2])
                });
            transform_node->setScale(Ogre::Vector3 {1.0, 1.0, 1.0});

            render_requested = true;
        }

        if(render_requested)
        {
            this->request_render();
        }
    }
}

//-----------------------------------------------------------------------------

void axis::stopping()
{
    this->render_service()->make_current();

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    if(m_scene_node != nullptr)
    {
        if(m_axis_visibility)
        {
            m_scene_node->removeAndDestroyAllChildren();
        }
    }

    if(m_enable_label)
    {
        for(auto& label : m_axis_labels)
        {
            SIGHT_ASSERT("label should not be null", label);
            label->detach_from_node();
            label.reset();
        }
    }

    if(m_axis_name_txt != nullptr)
    {
        m_axis_name_txt->detach_from_node();
        m_axis_name_txt.reset();
    }

    if(m_origin_visibility)
    {
        scene_mgr->destroyManualObject(m_origin);
    }

    if(m_axis_visibility)
    {
        scene_mgr->destroyManualObject(m_x_line);
        scene_mgr->destroyManualObject(m_y_line);
        scene_mgr->destroyManualObject(m_z_line);

        scene_mgr->destroyManualObject(m_x_cone);
        scene_mgr->destroyManualObject(m_y_cone);
        scene_mgr->destroyManualObject(m_z_cone);
    }

    Ogre::SceneNode* const transform_node = this->get_transform_node();
    if(transform_node != nullptr)
    {
        transform_node->removeAndDestroyChild(gen_id("main_node"));
    }

    m_material.reset();
    m_origin_material.reset();

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void axis::set_visible(bool _visible)
{
    if(m_scene_node != nullptr)
    {
        m_scene_node->setVisible(_visible);
        if(m_enable_label)
        {
            for(auto& label : m_axis_labels)
            {
                SIGHT_ASSERT("label should not be null", label);
                label->set_visible(_visible);
            }
        }

        if(m_axis_name_txt != nullptr)
        {
            m_axis_name_txt->set_visible(_visible);
        }
    }

    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t axis::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(m_image, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

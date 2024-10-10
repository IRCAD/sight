/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <service/macros.hpp>

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

    m_origin_material->set_ambient(_new_color);

    const auto sig = m_origin_material->signal<core::com::signal<void()> >(data::material::MODIFIED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void axis::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    const std::string transform_id = config.get<std::string>(
        module::viz::scene3d::adaptor::transform::TRANSFORM_CONFIG,
        this->get_id() + "_transform"
    );

    this->set_transform_id(transform_id);

    static const std::string s_LENGTH_CONFIG       = CONFIG + "length";
    static const std::string s_LABEL_CONFIG        = CONFIG + "label";
    static const std::string s_FONT_SIZE_CONFIG    = CONFIG + "fontSize";
    static const std::string s_ORIGIN_CONFIG       = CONFIG + "origin";
    static const std::string s_AXIS_CONFIG         = CONFIG + "axis";
    static const std::string s_ORIGIN_COLOR_CONFIG = CONFIG + "originColor";
    static const std::string s_AXIS_NAME           = CONFIG + "name";

    m_length            = config.get<float>(s_LENGTH_CONFIG, m_length);
    m_enable_label      = config.get<bool>(s_LABEL_CONFIG, m_enable_label);
    m_font_size         = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_origin_visibility = config.get<bool>(s_ORIGIN_CONFIG, m_origin_visibility);
    m_axis_visibility   = config.get<bool>(s_AXIS_CONFIG, m_axis_visibility);
    m_axis_name         = config.get<std::string>(s_AXIS_NAME, m_axis_name);

    m_origin_color = config.get<std::string>(s_ORIGIN_COLOR_CONFIG, m_origin_color);
    SIGHT_ASSERT(
        "Color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << m_origin_color,
        m_origin_color[0] == '#'
        && (m_origin_color.length() == 7 || m_origin_color.length() == 9)
    );

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
    m_scene_node = transform_node->createChildSceneNode(this->get_id() + "_mainNode");

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    if(m_origin_visibility)
    {
        m_origin = scene_mgr->createManualObject(this->get_id() + "_origin");
    }

    if(m_axis_visibility)
    {
        m_x_line = scene_mgr->createManualObject(this->get_id() + "_xLine");
        m_y_line = scene_mgr->createManualObject(this->get_id() + "_yLine");
        m_z_line = scene_mgr->createManualObject(this->get_id() + "_zLine");

        m_x_cone = scene_mgr->createManualObject(this->get_id() + "_xCone");
        m_y_cone = scene_mgr->createManualObject(this->get_id() + "_yCone");
        m_z_cone = scene_mgr->createManualObject(this->get_id() + "_zCone");
    }

    // set the material
    m_material        = std::make_shared<data::material>();
    m_origin_material = std::make_shared<data::material>();

    const module::viz::scene3d::adaptor::material::sptr material_adaptor =
        this->register_service<module::viz::scene3d::adaptor::material>(
            "sight::module::viz::scene3d::adaptor::material"
        );
    material_adaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::MATERIAL_INOUT, true);
    material_adaptor->configure(
        this->get_id() + material_adaptor->get_id(),
        this->get_id() + material_adaptor->get_id(),
        this->render_service(),
        m_layer_id
    );
    material_adaptor->start();

    material_adaptor->get_material_fw()->set_has_vertex_color(true);
    material_adaptor->update();

    const module::viz::scene3d::adaptor::material::sptr origin_material_adaptor =
        this->register_service<module::viz::scene3d::adaptor::material>(
            "sight::module::viz::scene3d::adaptor::material",
            this->get_id() + "_originMaterialAdp"
        );
    origin_material_adaptor->set_inout(
        m_origin_material,
        module::viz::scene3d::adaptor::material::MATERIAL_INOUT,
        true
    );
    origin_material_adaptor->configure(
        this->get_id() + origin_material_adaptor->get_id(),
        this->get_id() + origin_material_adaptor->get_id(),
        this->render_service(),
        m_layer_id,
        "ambient"
    );
    origin_material_adaptor->start();
    origin_material_adaptor->update();

    // Sizes
    const float origin_radius   = m_length * 0.1F;
    const float cylinder_length = m_length * 0.85F;
    const float cylinder_radius = m_length * 0.025F;
    const float cone_length     = m_length - cylinder_length;
    const float cone_radius     = cylinder_radius * 2.F;
    const unsigned sample       = 64;

    // Draw

    // origin

    if(m_origin_visibility)
    {
        const data::color::sptr origin_color = std::make_shared<data::color>();
        origin_color->set_rgba(m_origin_color);
        m_origin_material->set_ambient(origin_color);
        m_origin_material->set_diffuse(std::make_shared<data::color>(0.F, 0.F, 0.F, 1.F));

        const auto sig = m_origin_material->signal<core::com::signal<void()> >(data::material::MODIFIED_SIG);
        sig->async_emit();

        sight::viz::scene3d::helper::manual_object::create_sphere(
            m_origin,
            origin_material_adaptor->get_material_name(),
            Ogre::ColourValue(
                origin_color->red(),
                origin_color->green(),
                origin_color->blue(),
                origin_color->alpha()
            ),
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
            material_adaptor->get_material_name(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const x_line_node = m_scene_node->createChildSceneNode(this->get_id() + "_xLine");
        x_line_node->attachObject(m_x_line);
        x_line_node->pitch(Ogre::Degree(90));

        // Y axis
        sight::viz::scene3d::helper::manual_object::create_cylinder(
            m_y_line,
            material_adaptor->get_material_name(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const y_line_node = m_scene_node->createChildSceneNode(this->get_id() + "_yLine");
        y_line_node->attachObject(m_y_line);
        y_line_node->roll(Ogre::Degree(90));

        // Z axis
        sight::viz::scene3d::helper::manual_object::create_cylinder(
            m_z_line,
            material_adaptor->get_material_name(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const z_line_node = m_scene_node->createChildSceneNode(this->get_id() + "_zLine");
        z_line_node->attachObject(m_z_line);
        z_line_node->yaw(Ogre::Degree(-90));

        // X cone
        sight::viz::scene3d::helper::manual_object::create_cone(
            m_x_cone,
            material_adaptor->get_material_name(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const x_cone_node = m_scene_node->createChildSceneNode(this->get_id() + "_xCone");

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
            material_adaptor->get_material_name(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const y_cone_node = m_scene_node->createChildSceneNode(this->get_id() + "_yCone");
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
            material_adaptor->get_material_name(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const z_cone_node = m_scene_node->createChildSceneNode(this->get_id() + "_zCone");
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
        const data::color::sptr txt_color = std::make_shared<data::color>();
        txt_color->set_rgba(m_origin_color);
        m_axis_name_txt->set_text_color(
            Ogre::ColourValue(
                txt_color->red(),
                txt_color->green(),
                txt_color->blue()
            )
        );
    }

    this->apply_visibility();

    this->request_render();
}

//-----------------------------------------------------------------------------

void axis::updating()
{
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
            m_scene_node->removeAndDestroyChild(this->get_id() + "_xLine");
            m_scene_node->removeAndDestroyChild(this->get_id() + "_yLine");
            m_scene_node->removeAndDestroyChild(this->get_id() + "_zLine");
            m_scene_node->removeAndDestroyChild(this->get_id() + "_xCone");
            m_scene_node->removeAndDestroyChild(this->get_id() + "_yCone");
            m_scene_node->removeAndDestroyChild(this->get_id() + "_zCone");
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
        transform_node->removeAndDestroyChild(this->get_id() + "_mainNode");
    }

    this->unregister_services();
    m_material.reset();

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

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

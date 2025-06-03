/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "module/viz/scene3d/adaptor/point_list.hpp"

#include "module/viz/scene3d/adaptor/material.hpp"

#include "viz/scene3d/ogre.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/string.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/r2vb_renderable.hpp>
#include <viz/scene3d/render.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

#include <cstdint>

namespace sight::module::viz::scene3d::adaptor
{

static const std::string BILLBOARD_SIZE_UNIFORM = "u_billboardSize";

//-----------------------------------------------------------------------------

point_list::point_list() noexcept
{
    m_material = std::make_shared<data::material>();
}

//-----------------------------------------------------------------------------

point_list::~point_list() noexcept
{
    if(m_entity != nullptr)
    {
        Ogre::SceneManager* scene_mgr = this->get_scene_manager();
        scene_mgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void point_list::set_visible(bool _visible)
{
    if(m_entity != nullptr)
    {
        this->render_service()->make_current();

        m_entity->setVisible(_visible);

        m_mesh_geometry->set_visible(_visible);

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void point_list::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_COLOR_CONFIG             = CONFIG + "color";
    static const std::string s_VISIBLE_CONFIG           = CONFIG + "visible";
    static const std::string s_AUTORESET_CAMERA_CONFIG  = CONFIG + "autoresetcamera";
    static const std::string s_MATERIAL_TEMPLATE_CONFIG = CONFIG + "material_template";
    static const std::string s_FIXED_SIZE_CONFIG        = CONFIG + "fixedSize";
    static const std::string s_TEXTURE_NAME_CONFIG      = CONFIG + "textureName";
    static const std::string s_QUERY_CONFIG             = CONFIG + "queryFlags";
    static const std::string s_RADIUS_CONFIG            = CONFIG + "radius";
    static const std::string s_DISPLAY_LABEL_CONFIG     = CONFIG + "displayLabel";
    static const std::string s_LABEL_COLOR_CONFIG       = CONFIG + "labelColor";
    static const std::string s_FONT_SOURCE_CONFIG       = CONFIG + "fontSource";
    static const std::string s_FONT_SIZE_CONFIG         = CONFIG + "fontSize";

    const std::string color = config.get<std::string>(s_COLOR_CONFIG, "");

    SIGHT_ASSERT("Material not found", m_material);
    m_material->diffuse()->from_string(color.empty() ? "#FFFFFFFF" : color);

    m_auto_reset_camera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    if(config.count(s_MATERIAL_TEMPLATE_CONFIG) != 0U)
    {
        // An existing Ogre material will be used for this mesh
        m_custom_material        = true;
        m_material_template_name = config.get<std::string>(s_MATERIAL_TEMPLATE_CONFIG);
    }
    else if(config.get(s_FIXED_SIZE_CONFIG, false))
    {
        m_material_template_name = "Billboard_FixedSize";
    }

    // The mesh adaptor will pass the texture name to the created material adaptor
    m_texture_name = config.get(s_TEXTURE_NAME_CONFIG, m_texture_name);

    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );

    const std::string hexa_mask = config.get<std::string>(s_QUERY_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_flags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    m_font_source = config.get(s_FONT_SOURCE_CONFIG, m_font_source);
    m_font_size   = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);

    m_radius        = config.get(s_RADIUS_CONFIG, m_radius);
    m_display_label = config.get(s_DISPLAY_LABEL_CONFIG, m_display_label);

    const std::string label_color = config.get(s_LABEL_COLOR_CONFIG, "#FFFFFF");
    m_label_color = std::make_shared<data::color>();
    m_label_color->from_string(label_color);
}

//-----------------------------------------------------------------------------

void point_list::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    m_mesh_geometry = std::make_shared<sight::viz::scene3d::mesh>(this->get_id());
    m_mesh_geometry->set_dynamic(true);
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    m_scene_node = this->get_or_create_transform_node(root_scene_node);

    this->updating();

    this->apply_visibility();
}

//-----------------------------------------------------------------------------

service::connections_t point_list::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(m_point_list, data::point_list::POINT_ADDED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(m_point_list, data::point_list::POINT_REMOVED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(m_point_list, data::point_list::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);

    connections.push(m_points, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);

    connections.push(m_mesh, data::mesh::VERTEX_MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(m_mesh, data::mesh::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);

    return connections;
}

//-----------------------------------------------------------------------------

void point_list::stopping()
{
    this->render_service()->make_current();

    this->unregister_services();

    this->destroy_label();

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);
    m_mesh_geometry->clear_mesh(*scene_mgr);

    if(m_entity != nullptr)
    {
        scene_mgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    m_mesh_geometry.reset();

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void point_list::updating()
{
    this->render_service()->make_current();

    if(!get_visibility())
    {
        return;
    }

    this->destroy_label();

    if(const auto point_list = m_point_list.lock(); point_list)
    {
        this->update_mesh(point_list.get_shared());
    }
    else
    {
        if(not m_points.empty())
        {
            const auto tmp_list = std::make_shared<sight::data::point_list>();
            std::ranges::for_each(
                m_points,
                [&tmp_list](const auto& _x)
                {
                    const auto point = _x.second->const_lock();
                    tmp_list->get_points().push_back(std::const_pointer_cast<sight::data::point>(point.get_shared()));
                });
            this->update_mesh(tmp_list);
        }
        else
        {
            if(const auto mesh = m_mesh.lock(); mesh)
            {
                if(!m_custom_material && mesh->has<data::mesh::attribute::point_colors>())
                {
                    m_material_template_name += "_PerPointColor";
                }

                this->update_mesh(mesh.get_shared());
            }
            else
            {
                SIGHT_ERROR("No 'pointList', 'points' or 'mesh' specified.")
            }
        }
    }

    this->update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void point_list::create_label(const data::point_list::csptr& _point_list)
{
    auto render_srv          = this->render_service();
    std::size_t i            = 0;
    std::string label_number = std::to_string(i);
    for(const auto& point : _point_list->get_points())
    {
        const auto label = point->get_label();
        if(!label.empty())
        {
            label_number = label;
        }
        else
        {
            label_number = std::to_string(i);
        }

        auto text_label = sight::viz::scene3d::text::make(this->layer());
        text_label->set_font_size(m_font_size);
        text_label->set_text(label_number);
        text_label->set_text_color(
            Ogre::ColourValue(
                m_label_color->red(),
                m_label_color->green(),
                m_label_color->blue()
            )
        );
        auto* node = m_scene_node->createChildSceneNode(gen_id(label_number));
        m_nodes.push_back(node);
        text_label->attach_to_node(node, this->layer()->get_default_camera());
        m_labels.push_back(text_label);

        const auto& coord = *point;
        node->translate(static_cast<float>(coord[0]), static_cast<float>(coord[1]), static_cast<float>(coord[2]));
        i++;
    }
}

//------------------------------------------------------------------------------

void point_list::destroy_label()
{
    std::ranges::for_each(m_labels, [](auto& _label){_label->detach_from_node();});
    m_labels.clear();

    std::ranges::for_each(m_nodes, [this](auto& _node){m_scene_node->removeAndDestroyChild(_node);});
    m_nodes.clear();
}

//-----------------------------------------------------------------------------

void point_list::update_mesh(const data::point_list::csptr& _point_list)
{
    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    detach_and_destroy_entity();

    const std::size_t ui_num_vertices = _point_list->get_points().size();
    if(ui_num_vertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");
        m_mesh_geometry->clear_mesh(*scene_mgr);

        return;
    }

    if(m_display_label)
    {
        this->create_label(_point_list);
    }

    this->render_service()->make_current();

    m_mesh_geometry->update_mesh(_point_list);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(m_entity == nullptr)
    {
        m_entity = m_mesh_geometry->create_entity(*scene_mgr);
        m_entity->setVisible(visible());
        m_entity->setQueryFlags(m_query_flags);
    }

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_mesh_geometry->update_vertices(_point_list);

    //------------------------------------------
    // Create sub-services
    //------------------------------------------
    this->update_material_adaptor(_point_list->get_id());

    this->attach_node(m_entity);

    m_mesh_geometry->set_visible(visible());

    if(m_auto_reset_camera)
    {
        this->render_service()->reset_camera_coordinates(m_layer_id);
    }
}

//------------------------------------------------------------------------------

void point_list::update_mesh(const data::mesh::csptr& _mesh)
{
    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    detach_and_destroy_entity();

    const std::size_t ui_num_vertices = _mesh->num_points();
    if(ui_num_vertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");

        m_mesh_geometry->clear_mesh(*scene_mgr);
        return;
    }

    this->render_service()->make_current();

    m_mesh_geometry->update_mesh(std::const_pointer_cast<data::mesh>(_mesh), true);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(m_entity == nullptr)
    {
        m_entity = m_mesh_geometry->create_entity(*scene_mgr);
        m_entity->setVisible(visible());
        m_entity->setQueryFlags(m_query_flags);
    }

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_mesh_geometry->update_vertices(_mesh);
    m_mesh_geometry->update_colors(_mesh);

    //------------------------------------------
    // Create sub-services
    //------------------------------------------
    this->update_material_adaptor(_mesh->get_id());

    this->attach_node(m_entity);

    m_mesh_geometry->set_visible(visible());

    if(m_auto_reset_camera)
    {
        this->render_service()->reset_camera_coordinates(m_layer_id);
    }
}

//------------------------------------------------------------------------------

void point_list::update_material_adaptor(const std::string& _mesh_id)
{
    if(!m_material_adaptor)
    {
        if(m_entity != nullptr)
        {
            m_material_adaptor = this->register_service<module::viz::scene3d::adaptor::material>(
                "sight::module::viz::scene3d::adaptor::material"
            );
            m_material_adaptor->set_inout(m_material, "material", true);

            SIGHT_ASSERT("Template name empty", !m_material_template_name.empty());

            config_t material_adp_config;
            material_adp_config.put("config.<xmlattr>.material_template", m_material_template_name);

            if(!m_uniforms.empty())
            {
                std::size_t i = 0;
                for(const auto& uniform_data : m_uniforms)
                {
                    m_material_adaptor->set_inout(uniform_data.second->lock().get_shared(), "uniforms", true, {}, i++);
                }

                const auto config = this->get_config();
                if(const auto inouts_cfg = config.get_child_optional("inout"); inouts_cfg.has_value())
                {
                    const auto group = inouts_cfg->get<std::string>("<xmlattr>.group");
                    if(group == "uniforms")
                    {
                        material_adp_config.add_child("inout", inouts_cfg.value());
                    }
                }
            }

            const std::string mtl_name = core::id::join(_mesh_id, m_material_adaptor->get_id());
            SIGHT_ASSERT("Template name empty", !m_material_template_name.empty());

            m_material_adaptor->service::base::configure(material_adp_config);
            m_material_adaptor->set_id(gen_id(m_material_adaptor->get_id()));
            m_material_adaptor->set_material_name(mtl_name);
            m_material_adaptor->set_render_service(this->render_service());
            m_material_adaptor->set_layer_id(m_layer_id);
            m_material_adaptor->set_material_template_name(m_material_template_name);

            m_material_adaptor->start();

            auto* material_impl = m_material_adaptor->get_material_impl();
            material_impl->set_layout(*m_mesh_geometry);
            material_impl->set_geometry_uniform(BILLBOARD_SIZE_UNIFORM, m_radius);

            m_entity->setMaterialName(m_material_adaptor->get_material_name());

            if(!m_texture_name.empty())
            {
                const auto texture = Ogre::TextureManager::getSingleton().load(
                    m_texture_name,
                    sight::viz::scene3d::RESOURCE_GROUP
                );
                material_impl->set_texture("sprite", texture);
            }

            m_material_adaptor->update();
        }
    }
    else if(m_material_adaptor->inout<data::material>(material::MATERIAL_INOUT).lock() != m_material)
    {
        auto* material_impl = m_material_adaptor->get_material_impl();
        material_impl->set_layout(*m_mesh_geometry);
        material_impl->set_geometry_uniform(BILLBOARD_SIZE_UNIFORM, m_radius);
    }
    else
    {
        auto* material_impl = m_material_adaptor->get_material_impl();
        material_impl->set_layout(*m_mesh_geometry);
        material_impl->set_geometry_uniform(BILLBOARD_SIZE_UNIFORM, m_radius);

        m_entity->setMaterialName(m_material_adaptor->get_material_name());

        m_material_adaptor->slot(adaptor::slots::LAZY_UPDATE)->run();
    }
}

//-----------------------------------------------------------------------------

void point_list::attach_node(Ogre::MovableObject* _node)
{
    SIGHT_ASSERT("transform Node shouldn't be null", m_scene_node);
    m_scene_node->attachObject(_node);

    // Needed to recompute world bounding boxes of the scene node using its attached mesh bounds
    m_scene_node->_update(true, false);
}

//-----------------------------------------------------------------------------

void point_list::detach_and_destroy_entity()
{
    if(m_entity != nullptr)
    {
        Ogre::SceneManager* const scene_mgr = this->get_scene_manager();
        if(m_scene_node != nullptr)
        {
            m_scene_node->detachObject(m_entity);
        }

        scene_mgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

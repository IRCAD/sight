/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "modules/viz/scene3d/adaptor/video.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreCommon.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreMovablePlane.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t UPDATE_TF_SLOT     = "update_tf";
static const core::com::slots::key_t UPDATE_PL_SLOT     = "updatePL";
static const core::com::slots::key_t SET_FILTERING_SLOT = "set_filtering";
static const core::com::slots::key_t SCALE_SLOT         = "scale";

static const std::string VIDEO_MATERIAL_NAME             = "Video";
static const std::string VIDEO_WITH_TF_MATERIAL_NAME     = "VideoWithTF";
static const std::string VIDEO_WITH_TF_INT_MATERIAL_NAME = "VideoWithTF_Int";

//------------------------------------------------------------------------------

video::video() noexcept
{
    new_slot(UPDATE_TF_SLOT, &video::update_tf, this);
    new_slot(UPDATE_PL_SLOT, &video::update_pl, this);
    new_slot(SET_FILTERING_SLOT, &video::set_filtering, this);
    new_slot(SCALE_SLOT, &video::scale, this);
}

//------------------------------------------------------------------------------

void video::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    const bool visible = config.get<bool>(config::VISIBLE, m_visible);
    this->update_visibility(visible);

    m_material_template_name = config.get<std::string>(config::MATERIAL_TEMPLATE, m_material_template_name);
    m_texture_name           = config.get<std::string>(config::TEXTURE_NAME, m_texture_name);
    m_filtering              = config.get<bool>(config::FILTERING, m_filtering);
    m_scaling                = config.get<bool>(config::SCALING, m_scaling);
    m_radius                 = config.get<std::string>(config::RADIUS, m_radius);
    m_display_label          = config.get<std::string>(config::DISPLAY_LABEL, m_display_label);
    m_label_color            = config.get<std::string>(config::LABEL_COLOR, m_label_color);
    m_color                  = config.get<std::string>(config::COLOR, m_color);
    m_fixed_size             = config.get<std::string>(config::FIXED_SIZE, m_fixed_size);
    m_query_flags            = config.get<std::string>(config::QUERY, m_query_flags);
    m_font_source            = config.get<std::string>(config::FONT_SOURCE, m_font_source);
    m_font_size              = config.get<std::string>(config::FONT_SIZE, m_font_size);
}

//------------------------------------------------------------------------------

void video::starting()
{
    this->initialize();

    const auto pl_w = m_pl.lock();

    if(pl_w)
    {
        m_point_list = std::make_shared<data::point_list>();

        update_pl();

        m_point_list_adaptor = this->register_service<sight::viz::scene3d::adaptor>(
            "sight::module::viz::scene3d::adaptor::point_list"
        );

        m_point_list_adaptor->set_input(m_point_list, PL_INPUT, true);

        service::config_t config;
        config.add(CONFIG + "autoresetcamera", "false");
        if(!m_material_template_name.empty())
        {
            config.add(config::MATERIAL_TEMPLATE, m_material_template_name);
        }

        if(!m_texture_name.empty())
        {
            config.add(config::TEXTURE_NAME, m_texture_name);
        }

        if(!m_radius.empty())
        {
            config.add(config::RADIUS, m_radius);
        }

        if(!m_display_label.empty())
        {
            config.add(config::DISPLAY_LABEL, m_display_label);
        }

        if(!m_label_color.empty())
        {
            config.add(config::LABEL_COLOR, m_label_color);
        }

        if(!m_color.empty())
        {
            config.add(config::COLOR, m_color);
        }

        if(!m_fixed_size.empty())
        {
            config.add(config::FIXED_SIZE, m_fixed_size);
        }

        if(!m_query_flags.empty())
        {
            config.add(config::QUERY, m_query_flags);
        }

        if(!m_font_source.empty())
        {
            config.add(config::FONT_SOURCE, m_font_source);
        }

        if(!m_font_size.empty())
        {
            config.add(config::FONT_SIZE, m_font_size);
        }

        m_point_list_adaptor->set_config(config);

        m_point_list_adaptor->set_render_service(this->render_service());
        m_point_list_adaptor->set_layer_id(this->layer_id());

        m_point_list_adaptor->configure();
        m_point_list_adaptor->start();

        SIGHT_ASSERT("point_list is not started", m_point_list_adaptor->started());
    }

    {
        const auto image = m_image.lock();
        m_texture = std::make_shared<sight::viz::scene3d::texture>(image.get_shared());
    }
}

//-----------------------------------------------------------------------------

service::connections_t video::auto_connections() const
{
    service::connections_t connections;
    connections.push(IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE);

    connections.push(TF_INPUT, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT);
    connections.push(TF_INPUT, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT);
    connections.push(TF_INPUT, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT);

    connections.push(PL_INPUT, data::point_list::MODIFIED_SIG, UPDATE_PL_SLOT);
    connections.push(PL_INPUT, data::point_list::POINT_ADDED_SIG, UPDATE_PL_SLOT);
    connections.push(PL_INPUT, data::point_list::POINT_REMOVED_SIG, UPDATE_PL_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void video::updating()
{
    this->render_service()->make_current();

    const auto&& type_and_size = [this]
                                 {
                                     const auto image = m_image.lock();
                                     return std::make_pair(image->type(), image->size());
                                 }();
    const auto type = type_and_size.first;
    const auto size = type_and_size.second;
    if(size[0] == 0 || size[1] == 0)
    {
        return;
    }

    if(!m_is_texture_init || type != m_previous_type)
    {
        auto& mtl_mgr = Ogre::MaterialManager::getSingleton();
        const auto tf = m_tf.lock();

        Ogre::MaterialPtr default_mat;
        if(tf)
        {
            if(type == core::type::FLOAT || type == core::type::DOUBLE)
            {
                default_mat = mtl_mgr.getByName(VIDEO_WITH_TF_MATERIAL_NAME, sight::viz::scene3d::RESOURCE_GROUP);
            }
            else
            {
                default_mat = mtl_mgr.getByName(VIDEO_WITH_TF_INT_MATERIAL_NAME, sight::viz::scene3d::RESOURCE_GROUP);
            }
        }
        else
        {
            default_mat = mtl_mgr.getByName(VIDEO_MATERIAL_NAME, sight::viz::scene3d::RESOURCE_GROUP);
        }

        // Duplicate the template material to create our own material
        auto material = Ogre::MaterialManager::getSingletonPtr()->createOrRetrieve(
            this->get_id() + "_VideoMaterial",
            sight::viz::scene3d::RESOURCE_GROUP,
            true
        ).first;
        m_material = Ogre::dynamic_pointer_cast<Ogre::Material>(material);

        default_mat->copyDetailsTo(m_material);

        // Set the texture to the main material pass
        this->update_texture_filtering();

        if(tf)
        {
            // TF texture initialization
            m_gpu_tf = std::make_unique<sight::viz::scene3d::transfer_function>(tf.get_shared());
            this->update_tf();
        }

        m_previous_type = type;
    }

    m_texture->update();

    const auto layer                     = this->layer();
    const Ogre::Viewport* const viewport = layer->get_viewport();
    SIGHT_ASSERT("The current viewport cannot be retrieved.", viewport);

    if(!m_is_texture_init || size[0] != m_previous_width || size[1] != m_previous_height
       // If scaling is disabled and one of the viewport coordinate is modified
       // Then we need to trigger an update of the viewport displaying the texture
       || (!m_scaling
           && (viewport->getActualWidth() != m_previous_viewport_width
               || viewport->getActualHeight() != m_previous_viewport_height))
       || m_force_plane_update)
    {
        Ogre::Pass* pass = m_material->getTechnique(0)->getPass(0);
        m_texture->bind(pass, "image");

        this->clear_entity();

        // /////////////////////////////////////////////////////////////////////
        // Create the plane entity
        // /////////////////////////////////////////////////////////////////////
        const std::string this_id         = this->get_id();
        const std::string video_mesh_name = this_id + "_VideoMesh";
        const std::string entity_name     = this_id + "_VideoEntity";
        const std::string node_name       = this_id + "_VideoSceneNode";

        Ogre::MovablePlane plane(Ogre::Vector3::UNIT_Z, 0);

        Ogre::MeshManager& mesh_manager = Ogre::MeshManager::getSingleton();

        m_mesh = mesh_manager.createPlane(
            video_mesh_name,
            sight::viz::scene3d::RESOURCE_GROUP,
            plane,
            static_cast<Ogre::Real>(size[0]),
            static_cast<Ogre::Real>(size[1])
        );

        Ogre::SceneManager* scene_manager = this->get_scene_manager();
        SIGHT_ASSERT("The current scene manager cannot be retrieved.", scene_manager);

        // Create Ogre Entity
        m_entity = scene_manager->createEntity(entity_name, video_mesh_name);
        m_entity->setMaterial(m_material);

        // Add the entity to the scene
        m_scene_node = scene_manager->getRootSceneNode()->createChildSceneNode(node_name);
        m_scene_node->attachObject(m_entity);

        // Slightly offset the plane in Z to allow some space for other entities, thus they can be rendered on top
        m_scene_node->setPosition(0, 0, -1);
        m_scene_node->setVisible(m_visible);

        Ogre::Camera* cam = layer->get_default_camera();
        SIGHT_ASSERT("Default camera not found", cam);
        cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);

        if(m_scaling)
        {
            cam->setOrthoWindowHeight(static_cast<Ogre::Real>(size[1]));
        }
        else
        {
            cam->setOrthoWindowHeight(static_cast<Ogre::Real>(viewport->getActualHeight()));
            cam->setOrthoWindowWidth(static_cast<Ogre::Real>(viewport->getActualWidth()));
        }

        // Make sure no further scaling request is required
        m_force_plane_update = false;

        m_is_texture_init = true;
    }

    m_previous_width  = size[0];
    m_previous_height = size[1];

    m_previous_viewport_width  = viewport->getActualWidth();
    m_previous_viewport_height = viewport->getActualHeight();

    this->request_render();
}

//------------------------------------------------------------------------------

void video::stopping()
{
    this->render_service()->make_current();

    if(m_point_list_adaptor)
    {
        this->unregister_service(m_point_list_adaptor);
        m_point_list.reset();
    }

    this->clear_entity();

    m_texture.reset();
    m_material.reset();
    m_gpu_tf.reset();

    m_is_texture_init = false;
}

//-----------------------------------------------------------------------------

void video::set_visible(bool _visible)
{
    if(m_entity != nullptr)
    {
        m_entity->setVisible(_visible);

        this->request_render();
    }
}

//------------------------------------------------------------------------------

void video::update_tf()
{
    if(m_gpu_tf)
    {
        m_gpu_tf->update();

        Ogre::Pass* ogre_pass = m_material->getTechnique(0)->getPass(0);
        m_gpu_tf->bind(ogre_pass, "tf", ogre_pass->getFragmentProgramParameters());

        this->request_render();
    }
}

//------------------------------------------------------------------------------

void video::update_pl()
{
    const auto image = m_image.lock();
    const auto pl    = m_pl.lock();

    const data::point_list::container_t& in_points = pl->get_points();

    data::point_list::container_t& out_points = m_point_list->get_points();
    out_points.clear();

    for(const auto& in_point : in_points)
    {
        const data::point::point_coord_array_t& point = in_point->get_coord();
        out_points.push_back(
            std::make_shared<data::point>(
                point[0] - static_cast<double>(image->size()[0]) * 0.5,
                -(point[1] - static_cast<double>(image->size()[1]) * 0.5),
                point[2]
            )
        );
    }

    // Send the signal:
    auto modified_sig = m_point_list->signal<data::point_list::modified_signal_t>(
        data::point_list::MODIFIED_SIG
    );
    modified_sig->async_emit();
}

//------------------------------------------------------------------------------

void video::clear_entity()
{
    if(m_entity != nullptr)
    {
        m_entity->detachFromParent();
        this->get_scene_manager()->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    if(m_scene_node != nullptr)
    {
        m_scene_node->removeAndDestroyAllChildren();
        this->get_scene_manager()->destroySceneNode(m_scene_node);
        m_scene_node = nullptr;
    }

    if(m_mesh)
    {
        Ogre::MeshManager& mesh_manager = Ogre::MeshManager::getSingleton();
        mesh_manager.remove(m_mesh->getName(), sight::viz::scene3d::RESOURCE_GROUP);
        m_mesh.reset();
    }
}

//------------------------------------------------------------------------------

void video::set_filtering(bool _filtering)
{
    m_filtering = _filtering;

    // Only allow updating when the texture has been initialized
    // Otherwise, we might end up in cases where the slot is called before the
    // initialization, thus crashing the app
    if(m_is_texture_init)
    {
        this->update_texture_filtering();
    }
}

//------------------------------------------------------------------------------

void video::update_texture_filtering()
{
    Ogre::Pass* pass = m_material->getTechnique(0)->getPass(0);
    SIGHT_ASSERT("The current pass cannot be retrieved.", pass);
    Ogre::TextureUnitState* tus = pass->getTextureUnitState("image");
    SIGHT_ASSERT("The texture unit cannot be retrieved.", tus);

    // Set up texture filtering
    tus->setTextureFiltering(m_filtering ? Ogre::TFO_BILINEAR : Ogre::TFO_NONE);
}

//------------------------------------------------------------------------------

void video::scale(bool _value)
{
    m_scaling            = _value;
    m_force_plane_update = true;

    // Only allow updating when the texture has been initialized
    // Otherwise, we might end up in cases where the slot is called before the
    // initialization, thus crashing the app
    if(m_is_texture_init)
    {
        this->updating();
    }
}

} // namespace sight::module::viz::scene3d::adaptor.

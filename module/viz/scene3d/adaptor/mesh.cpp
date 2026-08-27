/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include "module/viz/scene3d/adaptor/mesh.hpp"

#include <core/ptree.hpp>

#include <viz/scene3d/r2vb_renderable.hpp>
#include <viz/scene3d/render.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreMaterialManager.h>

#include <algorithm>
#include <cstdint>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

mesh::mesh() noexcept
{
    m_material = std::make_shared<data::material>();

    new_slot(slots::MODIFY_MESH, [this](){lazy_update(update_flags::mesh);});
    new_slot(slots::MODIFY_COLORS, [this](){lazy_update(update_flags::colors);});
    new_slot(slots::MODIFY_POINT_TEX_COORDS, [this](){lazy_update(update_flags::tex_coords);});
    new_slot(slots::MODIFY_VERTICES, [this](){lazy_update(update_flags::vertices);});
    new_slot(
        slots::CHANGE_MATERIAL,
        [this](Ogre::MaterialPtr _material)
        {
            SIGHT_ASSERT("Entity null", m_entity);
            m_entity->setMaterial(_material);

            m_material_name = _material->getName();

            SIGHT_ASSERT("Adaptor is null", m_material_adaptor);
            m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);
        });
    new_slot(
        slots::CHANGE_COLOR,
        [this]()
        {
            SIGHT_ASSERT("Material not found", m_material);
            *m_material->diffuse() = *m_color;
            m_material_adaptor->slot(service::slots::UPDATE)->run();
        });
    new_slot(
        slots::CHANGE_BOUNDING_BOX_VISIBILITY,
        [this]()
        {
            if(m_bounding_box != nullptr)
            {
                m_bounding_box->setVisible(*m_bounding_box_visible);
                this->request_render();
            }
        });
}

//-----------------------------------------------------------------------------

mesh::~mesh() noexcept
{
    if(m_entity != nullptr)
    {
        Ogre::SceneManager* scene_mgr = this->get_scene_manager();
        scene_mgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void mesh::configuring(const config_t& _config)
{
    this->configure_params();

    m_auto_reset_camera = _config.get<bool>(CONFIG + "autoresetcamera", true);

    // If a material is configured in the XML scene, we keep its name to retrieve the adaptor later
    // Else we keep the name of the configured Ogre material (if it exists),
    //      it will be passed to the created material
    if(const auto material_name = _config.get_optional<std::string>(CONFIG + "material_name");
       material_name.has_value())
    {
        m_material_name = material_name.value();
    }
    else
    {
        // An existing Ogre material will be used for this mesh
        m_material_template_name = _config.get<std::string>(CONFIG + "material_template", m_material_template_name);

        // The mesh adaptor will pass the texture name to the created material adaptor
        m_texture_name = core::ptree::get_and_deprecate(
            _config,
            CONFIG + "texture_name",
            CONFIG + "textureName",
            "26.0",
            m_texture_name
        );

        m_shading_mode = core::ptree::get_and_deprecate(
            _config,
            CONFIG + "shading",
            CONFIG + "shadingMode",
            "26.0",
            m_shading_mode
        );

        auto representation_mode = _config.get_optional<std::string>(CONFIG + "representation");
        if(representation_mode.has_value())
        {
            m_representation_mode = sight::data::material::string_to_representation_mode(representation_mode.value());
        }

        auto options_mode = _config.get_optional<std::string>(CONFIG + "options");
        if(options_mode.has_value())
        {
            m_options_mode = sight::data::material::string_to_options_mode(options_mode.value());
        }
    }

    this->set_transform_id(
        _config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );

    m_is_dynamic          = _config.get<bool>(CONFIG + "dynamic", m_is_dynamic);
    m_is_dynamic_vertices = _config.get<bool>(CONFIG + "dynamic_vertices", m_is_dynamic_vertices);

    const auto hexa_mask = core::ptree::get_and_deprecate<std::string>(
        _config,
        CONFIG + "query_flags",
        CONFIG + "queryFlags",
        "26.0"
    );
    if(not hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_flags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    SIGHT_ASSERT("Material not found", m_material);
    *m_material->diffuse() = *m_color;
}

//-----------------------------------------------------------------------------

void mesh::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    if(this->get_transform_id().empty())
    {
        this->set_transform_id(this->gen_id("TF"));
    }

    m_mesh_geometry = std::make_shared<sight::viz::scene3d::mesh>(this->get_id());
    m_mesh_geometry->set_dynamic(m_is_dynamic);
    m_mesh_geometry->set_dynamic_vertices(m_is_dynamic_vertices);

    // We have to create a new material adaptor only if this adaptor is instantiated by a reconstruction adaptor
    // or if no material adaptor uid has been configured
    m_use_new_material_adaptor = m_material_name.empty();

    if(!m_use_new_material_adaptor)
    {
        // A material adaptor has been configured in the XML scene
        auto mtl_adaptors = this->render_service()->get_adaptors<module::viz::scene3d::adaptor::material>();

        auto result =
            std::ranges::find_if(
                mtl_adaptors,
                [this](const module::viz::scene3d::adaptor::material::sptr& _srv)
            {
                return _srv->get_material_name() == m_material_name;
            });

        m_material_adaptor = *result;

        m_material_connection.connect(
            m_material_adaptor,
            module::viz::scene3d::adaptor::material::signals::CHANGED,
            this->get_sptr(),
            slots::CHANGE_MATERIAL
        );

        SIGHT_ASSERT(
            "material adaptor managing material'" + m_material_name + "' is not found",
            result != mtl_adaptors.end()
        );
        m_material = m_material_adaptor->inout<data::material>(material::MATERIAL_INOUT).lock().get_shared();
    }

    // Creating bounding box
    const std::string bb_obj_name = gen_id("bounding_box");
    m_bounding_box = this->get_scene_manager()->createManualObject(bb_obj_name);
    m_bounding_box->setRenderQueueGroup(sight::viz::scene3d::rq::SURFACE);
    m_bounding_box->setVisible(*m_bounding_box_visible);

    const auto basic_ambient_mat = Ogre::MaterialManager::getSingleton().getByName(
        "BasicAmbient",
        sight::viz::scene3d::RESOURCE_GROUP
    );
    auto bb_mat = basic_ambient_mat->clone(bb_obj_name + "_Material");
    bb_mat->setAmbient(Ogre::ColourValue(1.0F, 1.0F, 0.0F));
    bb_mat->setDiffuse(Ogre::ColourValue(1.0F, 1.0F, 0.0F));

    this->attach_node(m_bounding_box);

    const auto mesh = m_mesh.lock();
    this->update_mesh(mesh.get_shared());
}

//-----------------------------------------------------------------------------

service::connections_t mesh::auto_connections() const
{
    const service::connections_t connections = {
        {m_mesh, data::mesh::signals::VERTEX_MODIFIED, slots::MODIFY_VERTICES},
        {m_mesh, data::mesh::signals::POINT_COLORS_MODIFIED, slots::MODIFY_COLORS},
        {m_mesh, data::mesh::signals::CELL_COLORS_MODIFIED, slots::MODIFY_COLORS},
        {m_mesh, data::mesh::signals::POINT_TEX_COORDS_MODIFIED, slots::MODIFY_POINT_TEX_COORDS},
        {m_mesh, data::signals::MODIFIED, slots::MODIFY_MESH},
        {m_color, data::signals::MODIFIED, slots::CHANGE_COLOR},
        {m_bounding_box_visible, data::signals::MODIFIED, slots::CHANGE_BOUNDING_BOX_VISIBILITY}
    };
    return connections + adaptor::auto_connections();
}

//-----------------------------------------------------------------------------

void mesh::updating()
{
    if((m_is_dynamic || m_is_dynamic_vertices) && (!get_visibility() || !this->render_service()->is_shown_on_screen()))
    {
        return;
    }

    if(update_needed(update_flags::mesh))
    {
        const auto mesh = m_mesh.lock();

        if(m_mesh_geometry->has_color_layer_changed(mesh.get_shared()))
        {
            Ogre::SceneManager* scene_mgr = this->get_scene_manager();
            SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);
            m_mesh_geometry->clear_mesh(*scene_mgr);
        }

        this->update_mesh(mesh.get_shared());
    }
    else if(update_needed(update_flags::vertices))
    {
        this->modify_vertices();
    }
    else if(update_needed(update_flags::colors))
    {
        this->modify_point_colors();
    }
    else if(update_needed(update_flags::tex_coords))
    {
        this->modify_tex_coords();
    }

    this->update_done();
}

//-----------------------------------------------------------------------------

void mesh::stopping()
{
    this->render_service()->make_current();

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    this->unregister_services();

    m_mesh_geometry->clear_mesh(*scene_mgr);
    m_material_adaptor.reset();

    if(m_entity != nullptr)
    {
        scene_mgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    if(m_bounding_box != nullptr)
    {
        scene_mgr->destroyManualObject(m_bounding_box);
        m_bounding_box = nullptr;
        Ogre::MaterialManager::getSingleton().remove(
            gen_id("bounding_box") + "_Material",
            sight::viz::scene3d::RESOURCE_GROUP
        );
    }

    m_mesh_geometry.reset();

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void module::viz::scene3d::adaptor::mesh::set_visible(bool _visible)
{
    if(m_entity != nullptr)
    {
        m_entity->setVisible(_visible);

        m_mesh_geometry->set_visible(_visible);

        if(m_auto_reset_camera && _visible)
        {
            this->render_service()->reset_camera_coordinates(layer_id());
        }

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void mesh::update_mesh(data::mesh::csptr _mesh)
{
    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    const std::size_t ui_num_vertices = _mesh->num_points();
    if(ui_num_vertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");

        if(m_entity != nullptr)
        {
            scene_mgr->destroyEntity(m_entity);
            m_entity = nullptr;
        }

        if(m_bounding_box != nullptr)
        {
            m_bounding_box->clear();
        }

        m_mesh_geometry->clear_mesh(*scene_mgr);
        this->request_render();
        return;
    }

    this->render_service()->make_current();

    m_mesh_geometry->update_mesh(_mesh);

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_mesh_geometry->update_vertices(_mesh);
    m_mesh_geometry->update_colors(_mesh);
    m_mesh_geometry->update_tex_coords(_mesh);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(m_entity == nullptr)
    {
        m_entity = m_mesh_geometry->create_entity(*scene_mgr);
        m_entity->setVisible(visible());
        m_entity->setQueryFlags(m_query_flags);
    }
    else
    {
        // Re-initialize the entity in order to trigger the build of the sub-entities list
        // We need them to exist now as we will set the material after that
        m_entity->_initialise(true);
    }

    //------------------------------------------
    // Create sub-services
    //------------------------------------------

    if(m_use_new_material_adaptor)
    {
        this->update_new_material_adaptor(_mesh);
    }
    else
    {
        this->update_xml_material_adaptor();
    }

    this->attach_node(m_entity);

    auto r2vb_renderables = m_mesh_geometry->update_r2vb(
        _mesh,
        *scene_mgr,
        m_material_adaptor->get_material_name()
    );
    for(auto* renderable : r2vb_renderables.second)
    {
        if(r2vb_renderables.first)
        {
            if(renderable->material == nullptr)
            {
                const std::string mtl_name = gen_id(renderable->getName());
                renderable->material = std::make_unique<sight::viz::scene3d::material::r2vb>(
                    mtl_name,
                    m_material_template_name
                );
                renderable->set_render_to_buffer_material(mtl_name);
                renderable->material->set_primitive_type(renderable->get_input_primitive_type());
            }

            renderable->material->set_layout(*m_mesh_geometry);
            renderable->material->set_shading(m_material->get_shading_mode());
            renderable->material->set_ambient_diffuse(m_material);

            // Attach r2vb object in the scene graph
            renderable->setQueryFlags(m_query_flags);
            this->attach_node(renderable);
        }
        else
        {
            this->get_scene_manager()->destroyMovableObject(renderable);
        }
    }

    m_mesh_geometry->set_visible(visible());

    if(m_auto_reset_camera)
    {
        this->layer()->reset_camera_coordinates();
    }

    this->update_bounding_box();

    this->request_render();
}

//------------------------------------------------------------------------------

void mesh::update_new_material_adaptor(data::mesh::csptr _mesh)
{
    if(!m_material_adaptor)
    {
        if(m_entity != nullptr)
        {
            m_material_adaptor = this->register_service<module::viz::scene3d::adaptor::material>(
                "sight::module::viz::scene3d::adaptor::material"
            );
            m_material_adaptor->set_inout(m_material, "material", true);

            config_t material_adp_config;
            material_adp_config.put("config.<xmlattr>.material_template", m_material_template_name);

            if(!m_uniform_objects.empty())
            {
                std::size_t i = 0;
                for(const auto& uniform_object : m_uniform_objects)
                {
                    const auto index = i++;
                    m_material_adaptor->set_inout(
                        uniform_object.second->lock().get_shared(),
                        "uniform.object",
                        true,
                        {},
                        index
                    );
                    m_material_adaptor->set_input(
                        m_uniform_names[index].lock().get_shared(),
                        "uniform.name",
                        true,
                        {},
                        index
                    );
                }
            }

            const std::string mtl_name = core::id::join(_mesh->get_id(), m_material_adaptor->get_id());
            SIGHT_ASSERT("Template name empty", !m_material_template_name.empty());

            m_material_adaptor->service::base::configure(material_adp_config);
            m_material_adaptor->set_id(gen_id(m_material_adaptor->get_id()));
            m_material_adaptor->set_material_name(mtl_name);
            m_material_adaptor->set_render_service(this->render_service());
            m_material_adaptor->set_layer_id(layer_id());
            m_material_adaptor->set_shading_mode(m_shading_mode);
            m_material_adaptor->set_material_template_name(m_material_template_name);

            m_material_adaptor->set_representation_mode(
                m_representation_mode.has_value() ? *m_representation_mode : m_material->get_representation_mode()
            );
            m_material_adaptor->set_options_mode(
                m_options_mode.has_value() ? *m_options_mode : m_material->get_options_mode()
            );

            // We know that we are in the case of a R2VB material, so no need to set the diffuse texture (no FP...)
            m_material_adaptor->set_texture_name(m_texture_name);

            m_material_adaptor->start();

            m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);
            m_material_adaptor->update();

            m_entity->setMaterialName(m_material_adaptor->get_material_name(), sight::viz::scene3d::RESOURCE_GROUP);

            m_material_connection.connect(
                m_material_adaptor,
                module::viz::scene3d::adaptor::material::signals::CHANGED,
                this->get_sptr(),
                slots::CHANGE_MATERIAL
            );
        }
    }
    else if(m_material_adaptor->inout<data::material>(material::MATERIAL_INOUT).lock() != m_material)
    {
        m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);
    }
    else
    {
        m_entity->setMaterialName(m_material_adaptor->get_material_name(), sight::viz::scene3d::RESOURCE_GROUP);
        m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);
        m_material_adaptor->slot(service::slots::UPDATE)->run();
    }
}

//------------------------------------------------------------------------------

void mesh::update_xml_material_adaptor()
{
    SIGHT_THROW_IF(
        "Can not provide both a user-defined material adaptor and uniforms.",
        !m_uniform_objects.empty()
    );

    if(m_material_adaptor->updating_status() == updating_status::notupdating)
    {
        if(m_material_adaptor->get_material_name().empty())
        {
            const auto mesh       = m_mesh.lock();
            std::string mesh_name = mesh->get_id();
            m_material_adaptor->set_material_name(mesh_name + "_Material");
        }

        if(m_entity != nullptr)
        {
            m_entity->setMaterialName(m_material_adaptor->get_material_name());
            m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);

            m_material_adaptor->slot(service::slots::UPDATE)->run();
        }
    }
    else if(m_material_adaptor->inout<data::material>(material::MATERIAL_INOUT).lock() != m_material)
    {
        m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);
    }
}

//-----------------------------------------------------------------------------

void mesh::modify_vertices()
{
    if((m_is_dynamic || m_is_dynamic_vertices) && (!get_visibility() || !this->render_service()->is_shown_on_screen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->render_service()->make_current();

    const auto mesh = m_mesh.lock();

    m_mesh_geometry->update_vertices(mesh.get_shared());

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();
    m_mesh_geometry->update_r2vb(
        mesh.get_shared(),
        *scene_mgr,
        m_material_adaptor->get_material_name()
    );

    // Necessary to update the bounding box in the adaptor
    //m_materialAdaptor->slot(service::slots::UPDATE)->async_run();

    if(m_auto_reset_camera)
    {
        this->render_service()->reset_camera_coordinates(layer_id());
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void mesh::modify_point_colors()
{
    if((m_is_dynamic || m_is_dynamic_vertices) && (!get_visibility() || !this->render_service()->is_shown_on_screen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->render_service()->make_current();

    const auto mesh = m_mesh.lock();

    if(m_mesh_geometry->has_color_layer_changed(mesh.get_shared()))
    {
        Ogre::SceneManager* scene_mgr = this->get_scene_manager();
        SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);
        m_mesh_geometry->clear_mesh(*scene_mgr);
        this->update_mesh(mesh.get_shared());
    }
    else
    {
        m_mesh_geometry->update_colors(mesh.get_shared());
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void mesh::modify_tex_coords()
{
    if((m_is_dynamic || m_is_dynamic_vertices) && (!get_visibility() || !this->render_service()->is_shown_on_screen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->render_service()->make_current();

    const auto mesh = m_mesh.lock();

    m_mesh_geometry->update_tex_coords(mesh.get_shared());

    this->request_render();
}

//-----------------------------------------------------------------------------

void mesh::update_bounding_box()
{
    const auto mesh = m_mesh.lock();
    if(!mesh)
    {
        return;
    }

    const auto bb     = mesh->get_bounding_box();
    const auto bb_min = bb.min;
    const auto bb_max = bb.max;

    const std::string bb_mat_name = gen_id("bounding_box") + "_Material";

    m_bounding_box->clear();

    // Draw wireframe bounding box with lines
    m_bounding_box->begin(bb_mat_name, Ogre::RenderOperation::OT_LINE_LIST, sight::viz::scene3d::RESOURCE_GROUP);
    m_bounding_box->colour(Ogre::ColourValue(1.0F, 1.0F, 0.0F)); // Yellow

    const Ogre::Vector3 min(static_cast<float>(bb_min[0]), static_cast<float>(bb_min[1]),
                            static_cast<float>(bb_min[2]));
    const Ogre::Vector3 max(static_cast<float>(bb_max[0]), static_cast<float>(bb_max[1]),
                            static_cast<float>(bb_max[2]));

    // Bottom face
    m_bounding_box->position(min.x, min.y, min.z);
    m_bounding_box->position(max.x, min.y, min.z);
    m_bounding_box->position(max.x, min.y, min.z);
    m_bounding_box->position(max.x, max.y, min.z);
    m_bounding_box->position(max.x, max.y, min.z);
    m_bounding_box->position(min.x, max.y, min.z);
    m_bounding_box->position(min.x, max.y, min.z);
    m_bounding_box->position(min.x, min.y, min.z);

    // Top face
    m_bounding_box->position(min.x, min.y, max.z);
    m_bounding_box->position(max.x, min.y, max.z);
    m_bounding_box->position(max.x, min.y, max.z);
    m_bounding_box->position(max.x, max.y, max.z);
    m_bounding_box->position(max.x, max.y, max.z);
    m_bounding_box->position(min.x, max.y, max.z);
    m_bounding_box->position(min.x, max.y, max.z);
    m_bounding_box->position(min.x, min.y, max.z);

    // Vertical edges
    m_bounding_box->position(min.x, min.y, min.z);
    m_bounding_box->position(min.x, min.y, max.z);
    m_bounding_box->position(max.x, min.y, min.z);
    m_bounding_box->position(max.x, min.y, max.z);
    m_bounding_box->position(max.x, max.y, min.z);
    m_bounding_box->position(max.x, max.y, max.z);
    m_bounding_box->position(min.x, max.y, min.z);
    m_bounding_box->position(min.x, max.y, max.z);

    m_bounding_box->end();

    m_bounding_box->setBoundingBox(Ogre::AxisAlignedBox(min, max));
}

//-----------------------------------------------------------------------------

void mesh::attach_node(Ogre::MovableObject* _node)
{
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);

    Ogre::SceneNode* node = _node->getParentSceneNode();

    if(node != trans_node)
    {
        _node->detachFromParent();
        trans_node->attachObject(_node);
    }
}

//-----------------------------------------------------------------------------

void mesh::request_render()
{
    m_mesh_geometry->invalidate_r2vb();

    sight::viz::scene3d::adaptor::request_render();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

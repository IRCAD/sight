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

#include "modules/viz/scene3d/adaptor/mesh.hpp"

#include "modules/viz/scene3d/adaptor/material.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/mesh.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/r2vb_renderable.hpp>
#include <viz/scene3d/render.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

#include <cstdint>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t MODIFY_MESH_SLOT             = "modifyMesh";
static const core::com::slots::key_t MODIFY_COLORS_SLOT           = "modifyColors";
static const core::com::slots::key_t MODIFY_POINT_TEX_COORDS_SLOT = "modifyTexCoords";
static const core::com::slots::key_t MODIFY_VERTICES_SLOT         = "modifyVertices";
static const core::com::slots::key_t CHANGE_MATERIAL_SLOT         = "change_material";

//-----------------------------------------------------------------------------

mesh::mesh() noexcept
{
    m_material = std::make_shared<data::material>();

    new_slot(MODIFY_MESH_SLOT, [this](){lazy_update(update_flags::MESH);});
    new_slot(MODIFY_COLORS_SLOT, [this](){lazy_update(update_flags::COLORS);});
    new_slot(MODIFY_POINT_TEX_COORDS_SLOT, [this](){lazy_update(update_flags::TEX_COORDS);});
    new_slot(MODIFY_VERTICES_SLOT, [this](){lazy_update(update_flags::VERTICES);});
    new_slot(
        CHANGE_MATERIAL_SLOT,
        [this](Ogre::MaterialPtr _material)
        {
            SIGHT_ASSERT("Entity null", m_entity);
            m_entity->setMaterial(_material);

            m_material_name = _material->getName();

            SIGHT_ASSERT("Adaptor is null", m_material_adaptor);
            m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);
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

void mesh::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    const std::string color = config.get<std::string>(CONFIG + "color", "");

    SIGHT_ASSERT("Material not found", m_material);
    m_material->diffuse()->from_string(color.empty() ? "#FFFFFFFF" : color);

    m_auto_reset_camera = config.get<bool>(CONFIG + "autoresetcamera", true);

    // If a material is configured in the XML scene, we keep its name to retrieve the adaptor later
    // Else we keep the name of the configured Ogre material (if it exists),
    //      it will be passed to the created material
    if(const auto material_name = config.get_optional<std::string>(CONFIG + "materialName");
       material_name.has_value())
    {
        m_material_name = material_name.value();
    }
    else
    {
        // An existing Ogre material will be used for this mesh
        m_material_template_name = config.get<std::string>(CONFIG + "materialTemplate", m_material_template_name);

        // The mesh adaptor will pass the texture name to the created material adaptor
        m_texture_name = config.get<std::string>(CONFIG + "textureName", m_texture_name);

        m_shading_mode = config.get<std::string>(CONFIG + "shadingMode", m_shading_mode);
    }

    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );

    m_is_dynamic          = config.get<bool>(CONFIG + "dynamic", m_is_dynamic);
    m_is_dynamic_vertices = config.get<bool>(CONFIG + "dynamicVertices", m_is_dynamic_vertices);

    if(const auto hexa_mask = config.get_optional<std::string>(CONFIG + "queryFlags"); hexa_mask.has_value())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask.value(),
            hexa_mask->length() > 2
            && hexa_mask->substr(0, 2) == "0x"
        );
        m_query_flags = static_cast<std::uint32_t>(std::stoul(hexa_mask.value(), nullptr, 16));
    }
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
    m_use_new_material_adaptor = m_is_reconstruction_managed || m_material_name.empty();

    if(!m_use_new_material_adaptor)
    {
        // A material adaptor has been configured in the XML scene
        auto mtl_adaptors = this->render_service()->get_adaptors<module::viz::scene3d::adaptor::material>();

        auto result =
            std::find_if(
                mtl_adaptors.begin(),
                mtl_adaptors.end(),
                [this](const module::viz::scene3d::adaptor::material::sptr& _srv)
            {
                return _srv->get_material_name() == m_material_name;
            });

        m_material_adaptor = *result;

        m_material_connection.connect(
            m_material_adaptor,
            module::viz::scene3d::adaptor::material::signals::CHANGED,
            this->get_sptr(),
            CHANGE_MATERIAL_SLOT
        );

        SIGHT_ASSERT(
            "material adaptor managing material'" + m_material_name + "' is not found",
            result != mtl_adaptors.end()
        );
        m_material = m_material_adaptor->inout<data::material>(material::MATERIAL_INOUT).lock().get_shared();
    }

    const auto mesh = m_mesh.lock();
    this->update_mesh(mesh.get_shared());
}

//-----------------------------------------------------------------------------

service::connections_t mesh::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(MESH_IN, data::mesh::VERTEX_MODIFIED_SIG, MODIFY_VERTICES_SLOT);
    connections.push(MESH_IN, data::mesh::POINT_COLORS_MODIFIED_SIG, MODIFY_COLORS_SLOT);
    connections.push(MESH_IN, data::mesh::CELL_COLORS_MODIFIED_SIG, MODIFY_COLORS_SLOT);
    connections.push(MESH_IN, data::mesh::POINT_TEX_COORDS_MODIFIED_SIG, MODIFY_POINT_TEX_COORDS_SLOT);
    connections.push(MESH_IN, data::mesh::MODIFIED_SIG, MODIFY_MESH_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void mesh::updating()
{
    if((m_is_dynamic || m_is_dynamic_vertices) && (!get_visibility() || !this->render_service()->is_shown_on_screen()))
    {
        return;
    }

    if(update_needed(update_flags::MESH))
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
    else if(update_needed(update_flags::VERTICES))
    {
        this->modify_vertices();
    }
    else if(update_needed(update_flags::COLORS))
    {
        this->modify_point_colors();
    }
    else if(update_needed(update_flags::TEX_COORDS))
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

        m_mesh_geometry->clear_mesh(*scene_mgr);
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

    this->request_render();
}

//------------------------------------------------------------------------------

adaptor::material::sptr mesh::create_material_service(
    data::mesh::csptr _mesh,
    const std::string& _material_suffix
)
{
    auto material_adaptor = this->register_service<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    material_adaptor->set_inout(m_material, "material", true);

    const std::string mtl_name = core::id::join(_mesh->get_id(), material_adaptor->get_id(), _material_suffix);
    SIGHT_ASSERT("Template name empty", !m_material_template_name.empty());

    material_adaptor->configure(
        gen_id(material_adaptor->get_id()),
        mtl_name,
        this->render_service(),
        m_layer_id,
        m_shading_mode,
        m_material_template_name
    );
    material_adaptor->set_representation_mode(m_material->get_representation_mode());

    if(_material_suffix.empty())
    {
        // We know that we are in the case of a R2VB material, so no need to set the diffuse texture (no FP...)
        material_adaptor->set_texture_name(m_texture_name);
    }

    return material_adaptor;
}

//------------------------------------------------------------------------------

void mesh::update_new_material_adaptor(data::mesh::csptr _mesh)
{
    if(!m_material_adaptor)
    {
        if(m_entity != nullptr)
        {
            m_material_adaptor = this->create_material_service(_mesh);
            m_material_adaptor->start();

            m_material_adaptor->get_material_impl()->set_layout(*m_mesh_geometry);
            m_material_adaptor->update();

            m_entity->setMaterialName(m_material_adaptor->get_material_name(), sight::viz::scene3d::RESOURCE_GROUP);

            m_material_connection.connect(
                m_material_adaptor,
                module::viz::scene3d::adaptor::material::signals::CHANGED,
                this->get_sptr(),
                CHANGE_MATERIAL_SLOT
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
        this->render_service()->reset_camera_coordinates(m_layer_id);
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

/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

static const core::com::slots::key_t MODIFY_COLORS_SLOT           = "modifyColors";
static const core::com::slots::key_t MODIFY_POINT_TEX_COORDS_SLOT = "modifyTexCoords";
static const core::com::slots::key_t MODIFY_VERTICES_SLOT         = "modifyVertices";

//-----------------------------------------------------------------------------

mesh::mesh() noexcept
{
    m_material = std::make_shared<data::material>();

    new_slot(MODIFY_COLORS_SLOT, &mesh::modifyPointColors, this);
    new_slot(MODIFY_POINT_TEX_COORDS_SLOT, &mesh::modifyTexCoords, this);
    new_slot(MODIFY_VERTICES_SLOT, &mesh::modifyVertices, this);
}

//-----------------------------------------------------------------------------

mesh::~mesh() noexcept
{
    if(m_entity != nullptr)
    {
        Ogre::SceneManager* scene_mgr = this->getSceneManager();
        scene_mgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void mesh::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    const std::string color = config.get<std::string>(s_CONFIG + "color", "");

    SIGHT_ASSERT("Material not found", m_material);
    m_material->diffuse()->setRGBA(color.empty() ? "#FFFFFFFF" : color);

    m_autoResetCamera = config.get<bool>(s_CONFIG + "autoresetcamera", true);

    // If a material is configured in the XML scene, we keep its name to retrieve the adaptor later
    // Else we keep the name of the configured Ogre material (if it exists),
    //      it will be passed to the created material
    if(const auto material_name = config.get_optional<std::string>(s_CONFIG + "materialName");
       material_name.has_value())
    {
        m_materialName = material_name.value();
    }
    else
    {
        // An existing Ogre material will be used for this mesh
        m_materialTemplateName = config.get<std::string>(s_CONFIG + "materialTemplate", m_materialTemplateName);

        // The mesh adaptor will pass the texture name to the created material adaptor
        m_textureName = config.get<std::string>(s_CONFIG + "textureName", m_textureName);

        m_shadingMode = config.get<std::string>(s_CONFIG + "shadingMode", m_shadingMode);
    }

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    m_isDynamic         = config.get<bool>(s_CONFIG + "dynamic", m_isDynamic);
    m_isDynamicVertices = config.get<bool>(s_CONFIG + "dynamicVertices", m_isDynamicVertices);

    if(const auto hexa_mask = config.get_optional<std::string>(s_CONFIG + "queryFlags"); hexa_mask.has_value())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask.value(),
            hexa_mask->length() > 2
            && hexa_mask->substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexa_mask.value(), nullptr, 16));
    }
}

//-----------------------------------------------------------------------------

void mesh::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    if(this->getTransformId().empty())
    {
        this->setTransformId(this->get_id() + "_TF");
    }

    m_meshGeometry = std::make_shared<sight::viz::scene3d::mesh>(this->get_id());
    m_meshGeometry->setDynamic(m_isDynamic);
    m_meshGeometry->setDynamicVertices(m_isDynamicVertices);

    // We have to create a new material adaptor only if this adaptor is instantiated by a reconstruction adaptor
    // or if no material adaptor uid has been configured
    m_useNewMaterialAdaptor = m_isReconstructionManaged || m_materialName.empty();

    if(!m_useNewMaterialAdaptor)
    {
        // A material adaptor has been configured in the XML scene
        auto mtl_adaptors = this->getRenderService()->getAdaptors<module::viz::scene3d::adaptor::material>();

        auto result =
            std::find_if(
                mtl_adaptors.begin(),
                mtl_adaptors.end(),
                [this](const module::viz::scene3d::adaptor::material::sptr& _srv)
            {
                return _srv->getMaterialName() == m_materialName;
            });

        m_materialAdaptor = *result;

        SIGHT_ASSERT(
            "material adaptor managing material'" + m_materialName + "' is not found",
            result != mtl_adaptors.end()
        );
        m_material = m_materialAdaptor->inout<data::material>(material::s_MATERIAL_INOUT).lock().get_shared();
    }

    const auto mesh = m_mesh.lock();
    this->updateMesh(mesh.get_shared());
}

//-----------------------------------------------------------------------------

service::connections_t mesh::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_MESH_IN, data::mesh::VERTEX_MODIFIED_SIG, MODIFY_VERTICES_SLOT);
    connections.push(s_MESH_IN, data::mesh::POINT_COLORS_MODIFIED_SIG, MODIFY_COLORS_SLOT);
    connections.push(s_MESH_IN, data::mesh::CELL_COLORS_MODIFIED_SIG, MODIFY_COLORS_SLOT);
    connections.push(s_MESH_IN, data::mesh::POINT_TEX_COORDS_MODIFIED_SIG, MODIFY_POINT_TEX_COORDS_SLOT);
    connections.push(s_MESH_IN, data::mesh::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void mesh::updating()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    const auto mesh = m_mesh.lock();

    if(m_meshGeometry->hasColorLayerChanged(mesh.get_shared()))
    {
        Ogre::SceneManager* scene_mgr = this->getSceneManager();
        SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);
        m_meshGeometry->clearMesh(*scene_mgr);
    }

    this->updateMesh(mesh.get_shared());
}

//-----------------------------------------------------------------------------

void mesh::stopping()
{
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* scene_mgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    this->unregisterServices();

    m_meshGeometry->clearMesh(*scene_mgr);
    m_materialAdaptor.reset();

    if(m_entity != nullptr)
    {
        scene_mgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    m_meshGeometry.reset();
}

//-----------------------------------------------------------------------------

void module::viz::scene3d::adaptor::mesh::setVisible(bool _visible)
{
    if(m_entity != nullptr)
    {
        m_entity->setVisible(_visible);

        m_meshGeometry->setVisible(_visible);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void mesh::updateMesh(data::mesh::csptr _mesh)
{
    Ogre::SceneManager* scene_mgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    const std::size_t ui_num_vertices = _mesh->numPoints();
    if(ui_num_vertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");

        if(m_entity != nullptr)
        {
            scene_mgr->destroyEntity(m_entity);
            m_entity = nullptr;
        }

        m_meshGeometry->clearMesh(*scene_mgr);
        return;
    }

    this->getRenderService()->makeCurrent();

    m_meshGeometry->updateMesh(_mesh);

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_meshGeometry->updateVertices(_mesh);
    m_meshGeometry->updateColors(_mesh);
    m_meshGeometry->updateTexCoords(_mesh);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(m_entity == nullptr)
    {
        m_entity = m_meshGeometry->createEntity(*scene_mgr);
        m_entity->setVisible(m_isVisible);
        m_entity->setQueryFlags(m_queryFlags);
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

    if(m_useNewMaterialAdaptor)
    {
        this->updateNewMaterialAdaptor(_mesh);
    }
    else
    {
        this->updateXMLMaterialAdaptor();
    }

    this->attachNode(m_entity);

    auto r2vb_renderables = m_meshGeometry->updateR2VB(
        _mesh,
        *scene_mgr,
        m_materialAdaptor->getMaterialName()
    );
    for(auto* renderable : r2vb_renderables.second)
    {
        auto adaptor = renderable->m_materialAdaptor.lock();

        if(r2vb_renderables.first)
        {
            if(adaptor)
            {
                auto r2vb_mtl_adaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::material>(adaptor);
                m_meshGeometry->updateMaterial(r2vb_mtl_adaptor->getMaterialFw(), true);
                // Update the material *synchronously* otherwise the r2vb will be rendered before the shader switch
                r2vb_mtl_adaptor->slot(service::slots::UPDATE)->run();
            }
            else
            {
                // Instantiate a material adaptor for the r2vb process for this primitive type
                adaptor = this->createMaterialService(_mesh, renderable->getName());

                auto r2vb_mtl_adaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::material>(adaptor);
                r2vb_mtl_adaptor->setR2VBObject(renderable);
                r2vb_mtl_adaptor->start();
                m_meshGeometry->updateMaterial(r2vb_mtl_adaptor->getMaterialFw(), true);
                r2vb_mtl_adaptor->update();

                renderable->setRenderToBufferMaterial(r2vb_mtl_adaptor->getMaterialName());
                renderable->m_materialAdaptor = r2vb_mtl_adaptor;
            }

            // Attach r2vb object in the scene graph
            renderable->setQueryFlags(m_queryFlags);
            this->attachNode(renderable);
        }
        else
        {
            // Unregister the service if it has been removed
            this->unregisterService(adaptor);

            this->getSceneManager()->destroyMovableObject(renderable);
        }
    }

    m_meshGeometry->setVisible(m_isVisible);

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

adaptor::material::sptr mesh::createMaterialService(
    data::mesh::csptr _mesh,
    const std::string& _material_suffix
)
{
    auto material_adaptor = this->registerService<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    material_adaptor->set_inout(m_material, "material", true);

    const std::string mesh_name = _mesh->get_id();
    const std::string mtl_name  = mesh_name + "_" + (material_adaptor->get_id()) + _material_suffix;
    const auto template_name    =
        !m_materialTemplateName.empty() ? m_materialTemplateName : sight::viz::scene3d::material::
        DEFAULT_MATERIAL_TEMPLATE_NAME;

    material_adaptor->configure(
        this->get_id() + "_" + material_adaptor->get_id(),
        mtl_name,
        this->getRenderService(),
        m_layerID,
        m_shadingMode,
        template_name
    );

    if(_material_suffix.empty())
    {
        // We know that we are in the case of a R2VB material, so no need to set the diffuse texture (no FP...)
        material_adaptor->setTextureName(m_textureName);
    }

    return material_adaptor;
}

//------------------------------------------------------------------------------

void mesh::updateNewMaterialAdaptor(data::mesh::csptr _mesh)
{
    if(!m_materialAdaptor)
    {
        if(m_entity != nullptr)
        {
            m_materialAdaptor = this->createMaterialService(_mesh);
            m_materialAdaptor->start();

            m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
            m_materialAdaptor->update();

            m_entity->setMaterialName(m_materialAdaptor->getMaterialName(), sight::viz::scene3d::RESOURCE_GROUP);
        }
    }
    else if(m_materialAdaptor->inout<data::material>(material::s_MATERIAL_INOUT).lock() != m_material)
    {
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
    }
    else
    {
        m_entity->setMaterialName(m_materialAdaptor->getMaterialName(), sight::viz::scene3d::RESOURCE_GROUP);
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
        m_materialAdaptor->slot(service::slots::UPDATE)->run();
    }
}

//------------------------------------------------------------------------------

void mesh::updateXMLMaterialAdaptor()
{
    if(m_materialAdaptor->updating_status() == UpdatingStatus::NOTUPDATING)
    {
        if(m_materialAdaptor->getMaterialName().empty())
        {
            const auto mesh       = m_mesh.lock();
            std::string mesh_name = mesh->get_id();
            m_materialAdaptor->setMaterialName(mesh_name + "_Material");
        }

        if(m_entity != nullptr)
        {
            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());
            m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);

            m_materialAdaptor->slot(service::slots::UPDATE)->run();
        }
    }
    else if(m_materialAdaptor->inout<data::material>(material::s_MATERIAL_INOUT).lock() != m_material)
    {
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
    }
}

//-----------------------------------------------------------------------------

void mesh::modifyVertices()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->getRenderService()->makeCurrent();

    const auto mesh = m_mesh.lock();

    m_meshGeometry->updateVertices(mesh.get_shared());

    Ogre::SceneManager* const scene_mgr = this->getSceneManager();
    m_meshGeometry->updateR2VB(
        mesh.get_shared(),
        *scene_mgr,
        m_materialAdaptor->getMaterialName()
    );

    // Necessary to update the bounding box in the adaptor
    //m_materialAdaptor->slot(service::slots::UPDATE)->async_run();

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void mesh::modifyPointColors()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->getRenderService()->makeCurrent();

    const auto mesh = m_mesh.lock();

    if(m_meshGeometry->hasColorLayerChanged(mesh.get_shared()))
    {
        Ogre::SceneManager* scene_mgr = this->getSceneManager();
        SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);
        m_meshGeometry->clearMesh(*scene_mgr);
        this->updateMesh(mesh.get_shared());
    }
    else
    {
        m_meshGeometry->updateColors(mesh.get_shared());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void mesh::modifyTexCoords()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->getRenderService()->makeCurrent();

    const auto mesh = m_mesh.lock();

    m_meshGeometry->updateTexCoords(mesh.get_shared());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void mesh::attachNode(Ogre::MovableObject* _node)
{
    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->getOrCreateTransformNode(root_scene_node);

    Ogre::SceneNode* node = _node->getParentSceneNode();

    if(node != trans_node)
    {
        _node->detachFromParent();
        trans_node->attachObject(_node);
    }
}

//-----------------------------------------------------------------------------

void mesh::requestRender()
{
    m_meshGeometry->invalidateR2VB();

    sight::viz::scene3d::adaptor::requestRender();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

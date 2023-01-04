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

#include "modules/viz/scene3d/adaptor/SMesh.hpp"

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <geometry/data/Mesh.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/op/Get.hpp>

#include <viz/scene3d/helper/Scene.hpp>
#include <viz/scene3d/R2VBRenderable.hpp>
#include <viz/scene3d/SRender.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

#include <cstdint>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_MODIFY_COLORS_SLOT           = "modifyColors";
static const core::com::Slots::SlotKeyType s_MODIFY_POINT_TEX_COORDS_SLOT = "modifyTexCoords";
static const core::com::Slots::SlotKeyType s_MODIFY_VERTICES_SLOT         = "modifyVertices";

//-----------------------------------------------------------------------------

SMesh::SMesh() noexcept
{
    m_material = data::Material::New();

    newSlot(s_MODIFY_COLORS_SLOT, &SMesh::modifyPointColors, this);
    newSlot(s_MODIFY_POINT_TEX_COORDS_SLOT, &SMesh::modifyTexCoords, this);
    newSlot(s_MODIFY_VERTICES_SLOT, &SMesh::modifyVertices, this);
}

//-----------------------------------------------------------------------------

SMesh::~SMesh() noexcept
{
    if(m_entity != nullptr)
    {
        Ogre::SceneManager* sceneMgr = this->getSceneManager();
        sceneMgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void SMesh::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    const std::string color = config.get<std::string>(s_CONFIG + "color", "");

    SIGHT_ASSERT("Material not found", m_material);
    m_material->diffuse()->setRGBA(color.empty() ? "#FFFFFFFF" : color);

    m_autoResetCamera = config.get<bool>(s_CONFIG + "autoresetcamera", true);

    // If a material is configured in the XML scene, we keep its name to retrieve the adaptor later
    // Else we keep the name of the configured Ogre material (if it exists),
    //      it will be passed to the created SMaterial
    if(const auto materialName = config.get_optional<std::string>(s_CONFIG + "materialName"); materialName.has_value())
    {
        m_materialName = materialName.value();
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
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    m_isDynamic         = config.get<bool>(s_CONFIG + "dynamic", m_isDynamic);
    m_isDynamicVertices = config.get<bool>(s_CONFIG + "dynamicVertices", m_isDynamicVertices);

    if(const auto hexaMask = config.get_optional<std::string>(s_CONFIG + "queryFlags"); hexaMask.has_value())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask.value(),
            hexaMask->length() > 2
            && hexaMask->substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexaMask.value(), nullptr, 16));
    }
}

//-----------------------------------------------------------------------------

void SMesh::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    if(this->getTransformId().empty())
    {
        this->setTransformId(this->getID() + "_TF");
    }

    m_meshGeometry = std::make_shared<sight::viz::scene3d::Mesh>(this->getID());
    m_meshGeometry->setDynamic(m_isDynamic);
    m_meshGeometry->setDynamicVertices(m_isDynamicVertices);

    // We have to create a new material adaptor only if this adaptor is instantiated by a reconstruction adaptor
    // or if no material adaptor uid has been configured
    m_useNewMaterialAdaptor = m_isReconstructionManaged || m_materialName.empty();

    if(!m_useNewMaterialAdaptor)
    {
        // A material adaptor has been configured in the XML scene
        auto mtlAdaptors = this->getRenderService()->getAdaptors<module::viz::scene3d::adaptor::SMaterial>();

        auto result =
            std::find_if(
                mtlAdaptors.begin(),
                mtlAdaptors.end(),
                [this](const module::viz::scene3d::adaptor::SMaterial::sptr& srv)
            {
                return srv->getMaterialName() == m_materialName;
            });

        m_materialAdaptor = *result;

        SIGHT_ASSERT(
            "SMaterial adaptor managing material'" + m_materialName + "' is not found",
            result != mtlAdaptors.end()
        );
        m_material = m_materialAdaptor->getInOut<data::Material>(SMaterial::s_MATERIAL_INOUT).lock().get_shared();
    }

    const auto mesh = m_mesh.lock();
    this->updateMesh(mesh.get_shared());
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMesh::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_MESH_IN, data::Mesh::s_VERTEX_MODIFIED_SIG, s_MODIFY_VERTICES_SLOT);
    connections.push(s_MESH_IN, data::Mesh::s_POINT_COLORS_MODIFIED_SIG, s_MODIFY_COLORS_SLOT);
    connections.push(s_MESH_IN, data::Mesh::s_CELL_COLORS_MODIFIED_SIG, s_MODIFY_COLORS_SLOT);
    connections.push(s_MESH_IN, data::Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG, s_MODIFY_POINT_TEX_COORDS_SLOT);
    connections.push(s_MESH_IN, data::Mesh::s_MODIFIED_SIG, IService::slots::s_UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void SMesh::updating()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    const auto mesh = m_mesh.lock();

    if(m_meshGeometry->hasColorLayerChanged(mesh.get_shared()))
    {
        Ogre::SceneManager* sceneMgr = this->getSceneManager();
        SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);
        m_meshGeometry->clearMesh(*sceneMgr);
    }

    this->updateMesh(mesh.get_shared());
}

//-----------------------------------------------------------------------------

void SMesh::stopping()
{
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);

    this->unregisterServices();

    m_meshGeometry->clearMesh(*sceneMgr);
    m_materialAdaptor.reset();

    if(m_entity != nullptr)
    {
        sceneMgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    m_meshGeometry.reset();
}

//-----------------------------------------------------------------------------

void module::viz::scene3d::adaptor::SMesh::setVisible(bool _visible)
{
    if(m_entity != nullptr)
    {
        m_entity->setVisible(_visible);

        m_meshGeometry->setVisible(_visible);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SMesh::updateMesh(data::Mesh::csptr _mesh)
{
    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);

    const std::size_t uiNumVertices = _mesh->numPoints();
    if(uiNumVertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");

        if(m_entity != nullptr)
        {
            sceneMgr->destroyEntity(m_entity);
            m_entity = nullptr;
        }

        m_meshGeometry->clearMesh(*sceneMgr);
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
        m_entity = m_meshGeometry->createEntity(*sceneMgr);
        m_entity->setVisible(m_isVisible);
        m_entity->setQueryFlags(m_queryFlags);
        sceneMgr->getRootSceneNode()->detachObject(m_entity);
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

    auto r2vbRenderables = m_meshGeometry->updateR2VB(
        _mesh,
        *sceneMgr,
        m_materialAdaptor->getMaterialName()
    );
    for(auto* renderable : r2vbRenderables.second)
    {
        auto adaptor = renderable->m_materialAdaptor.lock();

        if(r2vbRenderables.first)
        {
            if(adaptor)
            {
                auto r2vbMtlAdaptor = module::viz::scene3d::adaptor::SMaterial::dynamicCast(adaptor);
                m_meshGeometry->updateMaterial(r2vbMtlAdaptor->getMaterialFw(), true);
                // Update the material *synchronously* otherwise the r2vb will be rendered before the shader switch
                r2vbMtlAdaptor->slot(module::viz::scene3d::adaptor::SMaterial::IService::slots::s_UPDATE)->run();
            }
            else
            {
                // Instantiate a material adaptor for the r2vb process for this primitive type
                adaptor = this->createMaterialService(_mesh, renderable->getName());

                auto r2vbMtlAdaptor = module::viz::scene3d::adaptor::SMaterial::dynamicCast(adaptor);
                r2vbMtlAdaptor->setR2VBObject(renderable);
                r2vbMtlAdaptor->start();
                m_meshGeometry->updateMaterial(r2vbMtlAdaptor->getMaterialFw(), true);
                r2vbMtlAdaptor->update();

                renderable->setRenderToBufferMaterial(r2vbMtlAdaptor->getMaterialName());
                renderable->m_materialAdaptor = r2vbMtlAdaptor;
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

adaptor::SMaterial::sptr SMesh::createMaterialService(
    data::Mesh::csptr _mesh,
    const std::string& _materialSuffix
)
{
    auto materialAdaptor = this->registerService<module::viz::scene3d::adaptor::SMaterial>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    materialAdaptor->setInOut(m_material, "material", true);

    const std::string meshName = _mesh->getID();
    const std::string mtlName  = meshName + "_" + (materialAdaptor->getID()) + _materialSuffix;
    const auto tplName         =
        !m_materialTemplateName.empty() ? m_materialTemplateName : sight::viz::scene3d::Material::
        DEFAULT_MATERIAL_TEMPLATE_NAME;

    materialAdaptor->configure(
        this->getID() + "_" + materialAdaptor->getID(),
        mtlName,
        this->getRenderService(),
        m_layerID,
        m_shadingMode,
        tplName
    );

    if(_materialSuffix.empty())
    {
        // We know that we are in the case of a R2VB material, so no need to set the diffuse texture (no FP...)
        materialAdaptor->setTextureName(m_textureName);
    }

    return materialAdaptor;
}

//------------------------------------------------------------------------------

void SMesh::updateNewMaterialAdaptor(data::Mesh::csptr _mesh)
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
    else if(m_materialAdaptor->getInOut<data::Material>(SMaterial::s_MATERIAL_INOUT).lock() != m_material)
    {
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
    }
    else
    {
        m_entity->setMaterialName(m_materialAdaptor->getMaterialName(), sight::viz::scene3d::RESOURCE_GROUP);
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
        m_materialAdaptor->slot(module::viz::scene3d::adaptor::SMaterial::IService::slots::s_UPDATE)->run();
    }
}

//------------------------------------------------------------------------------

void SMesh::updateXMLMaterialAdaptor()
{
    if(m_materialAdaptor->getUpdatingStatus() == UpdatingStatus::NOTUPDATING)
    {
        if(m_materialAdaptor->getMaterialName().empty())
        {
            const auto mesh      = m_mesh.lock();
            std::string meshName = mesh->getID();
            m_materialAdaptor->setMaterialName(meshName + "_Material");
        }

        if(m_entity != nullptr)
        {
            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());
            m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);

            m_materialAdaptor->slot(module::viz::scene3d::adaptor::SMaterial::IService::slots::s_UPDATE)->run();
        }
    }
    else if(m_materialAdaptor->getInOut<data::Material>(SMaterial::s_MATERIAL_INOUT).lock() != m_material)
    {
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
    }
}

//-----------------------------------------------------------------------------

void SMesh::modifyVertices()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    // Keep the make current outside to avoid too many context changes when we update multiple attributes
    this->getRenderService()->makeCurrent();

    const auto mesh = m_mesh.lock();

    m_meshGeometry->updateVertices(mesh.get_shared());

    Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    m_meshGeometry->updateR2VB(
        mesh.get_shared(),
        *sceneMgr,
        m_materialAdaptor->getMaterialName()
    );

    // Necessary to update the bounding box in the adaptor
    //m_materialAdaptor->slot(module::viz::scene3d::adaptor::SMaterial::IService::slots::s_UPDATE)->asyncRun();

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::modifyPointColors()
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
        Ogre::SceneManager* sceneMgr = this->getSceneManager();
        SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);
        m_meshGeometry->clearMesh(*sceneMgr);
        this->updateMesh(mesh.get_shared());
    }
    else
    {
        m_meshGeometry->updateColors(mesh.get_shared());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::modifyTexCoords()
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

void SMesh::attachNode(Ogre::MovableObject* _node)
{
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);

    Ogre::SceneNode* node = _node->getParentSceneNode();

    if(node != transNode)
    {
        _node->detachFromParent();
        transNode->attachObject(_node);
    }
}

//-----------------------------------------------------------------------------

void SMesh::requestRender()
{
    m_meshGeometry->invalidateR2VB();

    sight::viz::scene3d::IAdaptor::requestRender();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

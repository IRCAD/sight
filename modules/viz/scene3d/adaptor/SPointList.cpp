/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SPointList.hpp"

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"

#include "viz/scene3d/ogre.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/String.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/op/Get.hpp>

#include <viz/scene3d/helper/Font.hpp>
#include <viz/scene3d/helper/Scene.hpp>
#include <viz/scene3d/R2VBRenderable.hpp>
#include <viz/scene3d/SRender.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

#include <cstdint>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

static const std::string s_COLOR_CONFIG             = "color";
static const std::string s_VISIBLE_CONFIG           = "visible";
static const std::string s_AUTORESET_CAMERA_CONFIG  = "autoresetcamera";
static const std::string s_MATERIAL_TEMPLATE_CONFIG = "materialTemplate";
static const std::string s_FIXED_SIZE_CONFIG        = "fixedSize";
static const std::string s_TEXTURE_NAME_CONFIG      = "textureName";
static const std::string s_QUERY_CONFIG             = "queryFlags";
static const std::string s_RADIUS_CONFIG            = "radius";
static const std::string s_DISPLAY_LABEL_CONFIG     = "displayLabel";
static const std::string s_LABEL_COLOR_CONFIG       = "labelColor";
static const std::string s_FONT_SOURCE_CONFIG       = "fontSource";
static const std::string s_FONT_SIZE_CONFIG         = "fontSize";

//-----------------------------------------------------------------------------

SPointList::SPointList() noexcept
{
    m_material = data::Material::New();
}

//-----------------------------------------------------------------------------

SPointList::~SPointList() noexcept
{
    if(m_entity)
    {
        Ogre::SceneManager* sceneMgr = this->getSceneManager();
        sceneMgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void SPointList::setVisible(bool _visible)
{
    if(m_entity)
    {
        this->getRenderService()->makeCurrent();

        m_entity->setVisible(_visible);

        m_meshGeometry->setVisible(_visible);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SPointList::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const std::string color = config.get<std::string>(s_COLOR_CONFIG, "");

    const bool visible = config.get<bool>(s_VISIBLE_CONFIG, m_isVisible);
    this->updateVisibility(visible);

    SIGHT_ASSERT("Material not found", m_material);
    m_material->diffuse()->setRGBA(color.empty() ? "#FFFFFFFF" : color);

    m_autoResetCamera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    if(config.count(s_MATERIAL_TEMPLATE_CONFIG))
    {
        // An existing Ogre material will be used for this mesh
        m_customMaterial       = true;
        m_materialTemplateName = config.get<std::string>(s_MATERIAL_TEMPLATE_CONFIG);
    }
    else if(config.get(s_FIXED_SIZE_CONFIG, false))
    {
        m_materialTemplateName = "Billboard_FixedSize";
    }

    // The mesh adaptor will pass the texture name to the created material adaptor
    m_textureName = config.get(s_TEXTURE_NAME_CONFIG, m_textureName);

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    const std::string hexaMask = config.get<std::string>(s_QUERY_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    m_fontSource = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize   = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);

    m_radius       = config.get(s_RADIUS_CONFIG, m_radius);
    m_displayLabel = config.get(s_DISPLAY_LABEL_CONFIG, m_displayLabel);

    const std::string labelColor = config.get(s_LABEL_COLOR_CONFIG, "#FFFFFF");
    m_labelColor = data::Color::New();
    m_labelColor->setRGBA(labelColor);
}

//-----------------------------------------------------------------------------

void SPointList::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    m_meshGeometry = std::make_shared<sight::viz::scene3d::Mesh>(this->getID());
    m_meshGeometry->setDynamic(true);
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_sceneNode = this->getTransformNode(rootSceneNode);

    const auto pointList = m_pointList.lock();
    if(pointList)
    {
        this->updateMesh(pointList.get_shared());
    }
    else
    {
        const auto mesh = m_mesh.lock();
        if(mesh)
        {
            if(!m_customMaterial && mesh->has<data::Mesh::Attributes::POINT_COLORS>())
            {
                m_materialTemplateName += "_PerPointColor";
            }

            this->updateMesh(mesh.get_shared());
        }
        else
        {
            SIGHT_ERROR("No '" << s_POINTLIST_INPUT << "' or '" << s_MESH_INPUT << "' specified.")
        }
    }
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SPointList::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_POINTLIST_INPUT, data::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_INPUT, data::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_INPUT, data::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);

    connections.push(s_MESH_INPUT, data::Mesh::s_VERTEX_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MESH_INPUT, data::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SPointList::stopping()
{
    this->getRenderService()->makeCurrent();

    this->unregisterServices();

    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);
    m_meshGeometry->clearMesh(*sceneMgr);

    if(m_entity)
    {
        sceneMgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    m_meshGeometry.reset();
}

//-----------------------------------------------------------------------------

void SPointList::updating()
{
    this->getRenderService()->makeCurrent();

    if((!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    this->destroyLabel();

    const auto pointList = m_pointList.lock();
    if(pointList)
    {
        this->updateMesh(pointList.get_shared());
    }
    else
    {
        const auto mesh = m_mesh.lock();
        if(mesh)
        {
            this->updateMesh(mesh.get_shared());
        }
        else
        {
            SIGHT_ERROR("No '" << s_POINTLIST_INPUT << "' or '" << s_MESH_INPUT << "' specified.")
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SPointList::createLabel(const data::PointList::csptr& _pointList)
{
    Ogre::SceneManager* sceneMgr          = this->getSceneManager();
    Ogre::OverlayContainer* textContainer = this->getLayer()->getOverlayTextPanel();
    Ogre::Camera* cam                     = this->getLayer()->getDefaultCamera();
    SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);

    const float dpi = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();

    std::size_t i           = 0;
    std::string labelNumber = std::to_string(i);
    for(auto& point : _pointList->getPoints())
    {
        const auto label = point->getLabel();
        if(!label.empty())
        {
            labelNumber = label;
        }
        else
        {
            labelNumber = std::to_string(i);
        }

        m_labels.push_back(
            sight::viz::scene3d::Text::New(
                this->getID() + labelNumber,
                sceneMgr,
                textContainer,
                m_fontSource,
                m_fontSize,
                dpi,
                cam
            )
        );
        m_labels[i]->setText(labelNumber);
        m_labels[i]->setTextColor(
            Ogre::ColourValue(
                m_labelColor->red(),
                m_labelColor->green(),
                m_labelColor->blue()
            )
        );
        m_nodes.push_back(m_sceneNode->createChildSceneNode(this->getID() + labelNumber));
        m_nodes[i]->attachObject(m_labels[i]);
        data::Point::PointCoordArrayType coord = point->getCoord();
        m_nodes[i]->translate(static_cast<float>(coord[0]), static_cast<float>(coord[1]), static_cast<float>(coord[2]));
        i++;
    }
}

//------------------------------------------------------------------------------

void SPointList::destroyLabel()
{
    Ogre::SceneManager* sceneMgr = this->getSceneManager();

    for(Ogre::SceneNode* node : m_nodes)
    {
        m_sceneNode->removeAndDestroyChild(node);
    }

    m_nodes.clear();

    for(sight::viz::scene3d::Text* label : m_labels)
    {
        sceneMgr->destroyMovableObject(label);
    }

    m_labels.clear();
}

//-----------------------------------------------------------------------------

void SPointList::updateMesh(const data::PointList::csptr& _pointList)
{
    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);

    detachAndDestroyEntity();

    const std::size_t uiNumVertices = _pointList->getPoints().size();
    if(uiNumVertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");
        m_meshGeometry->clearMesh(*sceneMgr);

        return;
    }

    if(m_displayLabel)
    {
        this->createLabel(_pointList);
    }

    this->getRenderService()->makeCurrent();

    m_meshGeometry->updateMesh(_pointList);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(!m_entity)
    {
        m_entity = m_meshGeometry->createEntity(*sceneMgr);
        m_entity->setVisible(m_isVisible);
        m_entity->setQueryFlags(m_queryFlags);
    }

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_meshGeometry->updateVertices(_pointList);

    //------------------------------------------
    // Create sub-services
    //------------------------------------------
    this->updateMaterialAdaptor();

    this->attachNode(m_entity);

    m_meshGeometry->setVisible(m_isVisible);

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
}

//------------------------------------------------------------------------------

void SPointList::updateMesh(const data::Mesh::csptr& _mesh)
{
    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", sceneMgr);

    detachAndDestroyEntity();

    const std::size_t uiNumVertices = _mesh->numPoints();
    if(uiNumVertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");

        m_meshGeometry->clearMesh(*sceneMgr);
        return;
    }

    this->getRenderService()->makeCurrent();

    m_meshGeometry->updateMesh(std::const_pointer_cast<data::Mesh>(_mesh), true);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(!m_entity)
    {
        m_entity = m_meshGeometry->createEntity(*sceneMgr);
        m_entity->setVisible(m_isVisible);
        m_entity->setQueryFlags(m_queryFlags);
    }

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_meshGeometry->updateVertices(_mesh);
    m_meshGeometry->updateColors(_mesh);

    //------------------------------------------
    // Create sub-services
    //------------------------------------------
    this->updateMaterialAdaptor();

    this->attachNode(m_entity);

    m_meshGeometry->setVisible(m_isVisible);

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
}

//------------------------------------------------------------------------------

scene3d::adaptor::SMaterial::sptr SPointList::createMaterialService(const std::string& _materialSuffix)
{
    auto materialAdaptor = this->registerService<module::viz::scene3d::adaptor::SMaterial>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    materialAdaptor->setInOut(m_material, "material", true);

    materialAdaptor->setID(this->getID() + "_" + materialAdaptor->getID());
    materialAdaptor->setRenderService(this->getRenderService());
    materialAdaptor->setLayerID(m_layerID);

    if(!m_materialTemplateName.empty())
    {
        materialAdaptor->setMaterialTemplateName(m_materialTemplateName);
    }

    std::string meshName;
    const auto pointList = m_pointList.lock();
    if(pointList)
    {
        meshName = pointList->getID();
    }
    else
    {
        const auto mesh = m_mesh.lock();
        if(mesh)
        {
            meshName = mesh->getID();
        }
    }

    const std::string mtlName = meshName + "_" + materialAdaptor->getID() + _materialSuffix;

    materialAdaptor->setMaterialName(mtlName);

    return materialAdaptor;
}

//------------------------------------------------------------------------------

void SPointList::updateMaterialAdaptor()
{
    if(!m_materialAdaptor)
    {
        if(m_entity)
        {
            m_materialAdaptor = this->createMaterialService();
            m_materialAdaptor->start();

            auto materialFw = m_materialAdaptor->getMaterialFw();
            m_meshGeometry->updateMaterial(materialFw, false);
            materialFw->setMeshSize(m_radius);

            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());

            if(!m_textureName.empty())
            {
                const auto texture = Ogre::TextureManager::getSingleton().load(
                    m_textureName,
                    sight::viz::scene3d::RESOURCE_GROUP
                );
                Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(
                    m_materialAdaptor->getMaterialName(),
                    sight::viz::scene3d::RESOURCE_GROUP
                );

                Ogre::TextureUnitState* texUnitState = material->getTechnique(0)->getPass(0)->getTextureUnitState(
                    "sprite"
                );
                texUnitState->setTexture(texture);
            }

            m_materialAdaptor->update();
        }
    }
    else if(m_materialAdaptor->getInOut<data::Material>(SMaterial::s_MATERIAL_INOUT).lock()
            != m_material)
    {
        auto materialFw = m_materialAdaptor->getMaterialFw();
        m_meshGeometry->updateMaterial(materialFw, false);
        materialFw->setMeshSize(m_radius);
    }
    else
    {
        auto materialFw = m_materialAdaptor->getMaterialFw();
        m_meshGeometry->updateMaterial(materialFw, false);
        materialFw->setMeshSize(m_radius);

        m_entity->setMaterialName(m_materialAdaptor->getMaterialName());

        m_materialAdaptor->slot(module::viz::scene3d::adaptor::SMaterial::s_UPDATE_SLOT)->run();
    }
}

//-----------------------------------------------------------------------------

void SPointList::attachNode(Ogre::MovableObject* _node)
{
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    transNode->attachObject(_node);

    // Needed to recompute world bounding boxes of the scene node using its attached mesh bounds
    transNode->_update(true, false);
}

//-----------------------------------------------------------------------------

void SPointList::detachAndDestroyEntity()
{
    if(m_entity)
    {
        Ogre::SceneManager* const sceneMgr   = this->getSceneManager();
        Ogre::SceneNode* const rootSceneNode = sceneMgr->getRootSceneNode();
        Ogre::SceneNode* const transNode     = this->getTransformNode(rootSceneNode);
        transNode->detachObject(m_entity);
        sceneMgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

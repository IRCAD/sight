/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SPointList.hpp"
#include <fwRenderOgre/helper/Font.hpp>
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwRenderOgre/helper/Scene.hpp>
#include <fwRenderOgre/R2VBRenderable.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

#include <cstdint>

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SPointList, ::fwData::PointList );
//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
static const ::fwServices::IService::KeyType s_POINTLIST_INPUT    = "pointList";
static const ::fwServices::IService::KeyType s_MESH_INPUT         = "mesh";
static const ::fwServices::IService::KeyType s_RADIUS             = "radius";
static const ::fwServices::IService::KeyType s_DISPLAY_LABEL_BOOL = "displayLabel";
static const ::fwServices::IService::KeyType s_CHARACTER_HEIGHT   = "charHeight";
static const ::fwServices::IService::KeyType s_LABEL_COLOR        = "labelColor";

//-----------------------------------------------------------------------------

SPointList::SPointList() noexcept :
    m_autoResetCamera(true),
    m_entity(nullptr),
    m_materialTemplateName("Billboard_Default"),
    m_isVisible(true)
{
    m_material = ::fwData::Material::New();
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SPointList::updateVisibility, this);
}

//-----------------------------------------------------------------------------

SPointList::~SPointList() noexcept
{
    if(m_entity)
    {
        ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
        sceneMgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void visuOgreAdaptor::SPointList::updateVisibility(bool isVisible)
{
    m_isVisible = isVisible;
    if(m_entity)
    {
        m_entity->setVisible(isVisible);

        m_meshGeometry->setVisible(isVisible);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SPointList::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string color = config.get<std::string>("color", "");

    SLM_ASSERT("Material not found", m_material);
    m_material->diffuse()->setRGBA(color.empty() ? "#ffffffff" : color);

    if(config.count("autoresetcamera"))
    {
        m_autoResetCamera = config.get<std::string>("autoresetcamera") == "yes";
    }

    if(config.get("fixedSize", false))
    {
        m_materialTemplateName = "Billboard_FixedSize";
    }
    // An existing Ogre material will be used for this mesh
    if( config.count("materialTemplate"))
    {
        m_materialTemplateName = config.get<std::string>("materialTemplate");
    }

    // The mesh adaptor will pass the texture name to the created material adaptor
    if ( config.count("textureName"))
    {
        m_textureName = config.get<std::string>("textureName");
    }

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG,
                                                  this->getID() + "_transform"));

    m_queryFlags   = config.get<std::uint32_t>("queryFlags", m_queryFlags);
    m_radius       = config.get(s_RADIUS, 1.f);
    m_displayLabel = config.get(s_DISPLAY_LABEL_BOOL, m_displayLabel);
    m_charHeight   = config.get(s_CHARACTER_HEIGHT, m_charHeight);

    const std::string labelColor = config.get(s_LABEL_COLOR, "#ffffff");
    m_labelColor = ::fwData::Color::New();
    m_labelColor->setRGBA(labelColor);
}

//-----------------------------------------------------------------------------

void SPointList::starting()
{
    this->initialize();

    m_meshGeometry = ::std::make_shared< ::fwRenderOgre::Mesh>(this->getID());
    m_meshGeometry->setDynamic(true);
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_sceneNode                      = this->getTransformNode(rootSceneNode);

    const auto pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    if(pointList)
    {
        this->updateMesh(pointList);
    }
    else
    {
        const auto mesh = this->getInput< ::fwData::Mesh >(s_MESH_INPUT);
        if(mesh)
        {
            this->updateMesh(mesh);
        }
        else
        {
            OSLM_ERROR("No '" + s_POINTLIST_INPUT + "' or '" + s_MESH_INPUT + "' specified.")
        }
    }
}

//-----------------------------------------------------------------------------

void SPointList::stopping()
{
    this->unregisterServices();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);
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
    if((!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    auto pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    this->destroyLabel();
    if(pointList)
    {
        this->updateMesh(pointList);
    }
    else
    {
        auto mesh = this->getInput< ::fwData::Mesh >(s_MESH_INPUT);
        if(mesh)
        {
            this->updateMesh(mesh);
        }
        else
        {
            OSLM_ERROR("No '" + s_POINTLIST_INPUT + "' or '" + s_MESH_INPUT + "' specified.")
        }
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPointList::createLabel(const ::fwData::PointList::csptr& _pointList)
{
    ::Ogre::SceneManager* sceneMgr          = this->getSceneManager();
    ::Ogre::OverlayContainer* textContainer = this->getRenderService()->getOverlayTextPanel();
    ::Ogre::FontPtr dejaVuSansFont          = ::fwRenderOgre::helper::Font::getFont("DejaVuSans.ttf", 32);
    ::Ogre::Camera* cam                     = this->getLayer()->getDefaultCamera();
    SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);

    size_t i                = 0;
    std::string labelNumber = std::to_string(i);
    for(auto& point: _pointList->getPoints())
    {
        fwData::String::sptr strField =
            point->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId);
        if(strField)
        {
            labelNumber = strField->value();
        }
        else
        {
            labelNumber = std::to_string(i);
        }
        m_labels.push_back(::fwRenderOgre::Text::New(this->getID() + labelNumber, sceneMgr, textContainer,
                                                     dejaVuSansFont, cam));
        m_labels[i]->setText(labelNumber);
        m_labels[i]->setCharHeight(m_charHeight);
        m_labels[i]->setTextColor(::Ogre::ColourValue(m_labelColor->red(), m_labelColor->green(),
                                                      m_labelColor->blue()));
        m_nodes.push_back(m_sceneNode->createChildSceneNode(this->getID() + labelNumber));
        m_nodes[i]->attachObject(m_labels[i]);
        ::fwData::Point::PointCoordArrayType coord = point->getCoord();
        m_nodes[i]->translate(static_cast<float>(coord[0]), static_cast<float>(coord[1]), static_cast<float>(coord[2]));
        i++;
    }
}

//------------------------------------------------------------------------------

void SPointList::destroyLabel()
{
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    for(::Ogre::SceneNode* node : m_nodes)
    {
        m_sceneNode->removeAndDestroyChild(node);
    }
    m_nodes.clear();

    for(::fwRenderOgre::Text* label : m_labels)
    {
        sceneMgr->destroyMovableObject(label);
    }
    m_labels.clear();
}
//-----------------------------------------------------------------------------

void SPointList::updateMesh(const ::fwData::PointList::csptr& _pointList)
{
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);
    ::fwData::mt::ObjectReadLock lock(_pointList);

    detachAndDestroyEntity();

    const size_t uiNumVertices = _pointList->getPoints().size();
    if(uiNumVertices == 0)
    {
        SLM_DEBUG("Empty mesh");
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
        m_entity->addQueryFlags(m_queryFlags);
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

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
}

//------------------------------------------------------------------------------

void SPointList::updateMesh(const ::fwData::Mesh::csptr& _mesh)
{
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);

    ::fwData::mt::ObjectReadLock lock(_mesh);

    detachAndDestroyEntity();

    const size_t uiNumVertices = _mesh->getNumberOfPoints();
    if(uiNumVertices == 0)
    {
        SLM_DEBUG("Empty mesh");

        m_meshGeometry->clearMesh(*sceneMgr);
        return;
    }
    this->getRenderService()->makeCurrent();

    m_meshGeometry->updateMesh(::std::const_pointer_cast< ::fwData::Mesh >(_mesh), true);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(!m_entity)
    {
        m_entity = m_meshGeometry->createEntity(*sceneMgr);
        m_entity->setVisible(m_isVisible);
        m_entity->addQueryFlags(m_queryFlags);
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

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
}

//------------------------------------------------------------------------------

::visuOgreAdaptor::SMaterial::sptr SPointList::createMaterialService(const std::string& _materialSuffix)
{
    auto materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, "material", true);

    materialAdaptor->setID(this->getID() + "_" + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(m_layerID);

    if (!m_materialTemplateName.empty())
    {
        materialAdaptor->setMaterialTemplateName(m_materialTemplateName);
    }
    std::string meshName;
    auto pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    if(pointList)
    {
        meshName = pointList->getID();
    }
    else
    {
        auto mesh = this->getInput< ::fwData::Mesh >(s_MESH_INPUT);
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
                const auto texture = ::Ogre::TextureManager::getSingleton().load(m_textureName, "Materials");
                ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(
                    m_materialAdaptor->getMaterialName());

                ::Ogre::TextureUnitState* texUnitState = material->getTechnique(0)->getPass(0)->getTextureUnitState(
                    "sprite");
                texUnitState->setTexture(texture);
            }
            m_materialAdaptor->update();
        }
    }
    else if(m_materialAdaptor->getInOut< ::fwData::Material >(SMaterial::s_MATERIAL_INOUT) != m_material)
    {
        auto materialFw = m_materialAdaptor->getMaterialFw();
        m_meshGeometry->updateMaterial(materialFw, false);
        materialFw->setMeshSize(m_radius);
        m_materialAdaptor->swap(m_material);
    }
    else
    {
        auto materialFw = m_materialAdaptor->getMaterialFw();
        m_meshGeometry->updateMaterial(materialFw, false);
        materialFw->setMeshSize(m_radius);

        m_entity->setMaterialName(m_materialAdaptor->getMaterialName());

        m_materialAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->run();
    }
}

//-----------------------------------------------------------------------------

void SPointList::attachNode(::Ogre::MovableObject* _node)
{
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    transNode->attachObject(_node);

    // Needed to recompute world bounding boxes of the scene node using its attached mesh bounds
    transNode->_update(true, false);
}

//-----------------------------------------------------------------------------

void SPointList::detachAndDestroyEntity()
{
    if(m_entity)
    {
        ::Ogre::SceneManager* const sceneMgr   = this->getSceneManager();
        ::Ogre::SceneNode* const rootSceneNode = sceneMgr->getRootSceneNode();
        ::Ogre::SceneNode* const transNode     = this->getTransformNode(rootSceneNode);
        transNode->detachObject(m_entity);
        sceneMgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPointList::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_POINTLIST_INPUT, ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT );
    connections.push( s_POINTLIST_INPUT, ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT );
    connections.push( s_POINTLIST_INPUT, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT );

    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_VERTEX_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

void SPointList::requestRender()
{
    ::fwRenderOgre::IAdaptor::requestRender();
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor

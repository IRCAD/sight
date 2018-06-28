/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SPointList.hpp"

#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/mt/ObjectReadLock.hpp>

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

static const ::fwServices::IService::KeyType s_POINTLIST_INPUT = "pointList";
static const ::fwServices::IService::KeyType s_MESH_INPUT      = "mesh";

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

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_CONFIG_TRANSFORM,
                                                  this->getID() + "_transform"));

    m_queryFlags = config.get<std::uint32_t>("queryFlags", m_queryFlags);
    m_radius     = config.get("radius", 1.f);
}

//-----------------------------------------------------------------------------

void SPointList::starting()
{
    this->initialize();

    m_meshGeometry = ::std::make_shared< ::fwRenderOgre::Mesh>(this->getID());
    m_meshGeometry->setDynamic(true);

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

//-----------------------------------------------------------------------------

void SPointList::updateMesh(const ::fwData::PointList::csptr& _pointList)
{
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);

    ::fwData::mt::ObjectReadLock lock(_pointList);

    const size_t uiNumVertices = _pointList->getPoints().size();
    if(uiNumVertices == 0)
    {
        SLM_DEBUG("Empty mesh");

        if(m_entity)
        {
            sceneMgr->destroyEntity(m_entity);
            m_entity = nullptr;
        }
        m_meshGeometry->clearMesh(*sceneMgr);
        return;
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
        sceneMgr->getRootSceneNode()->detachObject(m_entity);
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

    const size_t uiNumVertices = _mesh->getNumberOfPoints();
    if(uiNumVertices == 0)
    {
        SLM_DEBUG("Empty mesh");

        if(m_entity)
        {
            sceneMgr->destroyEntity(m_entity);
            m_entity = nullptr;
        }
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
        sceneMgr->getRootSceneNode()->detachObject(m_entity);
    }

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_meshGeometry->updateVertices(_mesh);

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
    auto transformService = ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    ::Ogre::SceneNode* node          = _node->getParentSceneNode();
    if ((node != transNode) && transNode)
    {
        _node->detachFromParent();
        transNode->attachObject(_node);
    }

    // Needed to recompute world bounding boxes of the scene node using its attached mesh bounds
    transNode->_update(true, false);
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

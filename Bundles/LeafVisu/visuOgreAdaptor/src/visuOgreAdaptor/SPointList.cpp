/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
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

static const std::string s_POINTLIST_INOUT = "pointList";

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

    if(config.count("transform"))
    {
        this->setTransformId(config.get<std::string>("transform"));
    }
    else
    {
        this->setTransformId(this->getID() + "_TF");
    }

    m_radius = config.get("radius", 1.f);
}

//-----------------------------------------------------------------------------

void SPointList::starting()
{
    this->initialize();

    if (this->getTransformId().empty())
    {
        this->setTransformId(this->getID() + "_TF");
    }

    m_meshGeometry = ::std::make_shared< ::fwRenderOgre::Mesh>(this->getID());
    m_meshGeometry->setDynamic(true);

    auto pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INOUT);
    this->updateMesh(pointList);
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
    auto pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INOUT);
    ::fwData::mt::ObjectReadLock lock(pointList);

    this->updateMesh(pointList);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SPointList::updateMesh(const ::fwData::PointList::csptr& _pointList)
{
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);

    ::fwData::mt::ObjectReadLock lock(_pointList);

    const size_t uiNumVertices = _pointList->getCRefPoints().size();
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

    const std::string meshName = this->getInput< ::fwData::PointList >(s_POINTLIST_INOUT)->getID();
    const std::string mtlName  = meshName + "_" + materialAdaptor->getID() + _materialSuffix;

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
    else if(m_materialAdaptor->getObject< ::fwData::Material >() != m_material)
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
        m_materialAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->run();
    }
}

//-----------------------------------------------------------------------------

void SPointList::attachNode(::Ogre::MovableObject* _node)
{
    auto transformService = ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);

    if (transNode == nullptr)
    {
        transNode = rootSceneNode->createChildSceneNode(this->getTransformId());
    }
    ::Ogre::SceneNode* node = _node->getParentSceneNode();
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
    connections.push( s_POINTLIST_INOUT, ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT );
    connections.push( s_POINTLIST_INOUT, ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT );
    connections.push( s_POINTLIST_INOUT, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SPointList::requestRender()
{
    ::fwRenderOgre::IAdaptor::requestRender();
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor

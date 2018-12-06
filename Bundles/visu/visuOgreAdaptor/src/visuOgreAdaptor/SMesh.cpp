/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "visuOgreAdaptor/SMesh.hpp"

#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/mt/ObjectReadLock.hpp>

#include <fwDataTools/Mesh.hpp>

#include <fwRenderOgre/helper/Scene.hpp>
#include <fwRenderOgre/R2VBRenderable.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

#include <cstdint>

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SMesh, ::fwData::Mesh );

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT       = "updateVisibility";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_COLORS_SLOT           = "modifyColors";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_POINT_TEX_COORDS_SLOT = "modifyTexCoords";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_VERTICES_SLOT         = "modifyVertices";

static const std::string s_MESH_INOUT = "mesh";

//-----------------------------------------------------------------------------

SMesh::SMesh() noexcept :
    m_autoResetCamera(true),
    m_entity(nullptr),
    m_materialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_isReconstructionManaged(false),
    m_useNewMaterialAdaptor(false),
    m_isVisible(true)
{
    m_material = ::fwData::Material::New();

    newSlot(s_UPDATE_VISIBILITY_SLOT, &SMesh::updateVisibility, this);
    newSlot(s_MODIFY_COLORS_SLOT, &SMesh::modifyPointColors, this);
    newSlot(s_MODIFY_POINT_TEX_COORDS_SLOT, &SMesh::modifyTexCoords, this);
    newSlot(s_MODIFY_VERTICES_SLOT, &SMesh::modifyVertices, this);
}

//-----------------------------------------------------------------------------

SMesh::~SMesh() noexcept
{
    if(m_entity)
    {
        ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
        sceneMgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void visuOgreAdaptor::SMesh::updateVisibility(bool isVisible)
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

void SMesh::configuring()
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

    // If a material is configured in the XML scene, we keep its name to retrieve the adaptor later
    // Else we keep the name of the configured Ogre material (if it exists),
    //      it will be passed to the created SMaterial
    if ( config.count("materialName"))
    {
        m_materialName = config.get<std::string>("materialName");
    }
    else
    {
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

        if( config.count("shadingMode"))
        {
            m_shadingMode = config.get<std::string>("shadingMode");
        }
    }

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG,
                                                  this->getID() + "_transform"));

    m_isDynamic         = config.get<bool>("dynamic", m_isDynamic);
    m_isDynamicVertices = config.get<bool>("dynamicVertices", m_isDynamicVertices);
    m_queryFlags        = config.get<std::uint32_t>("queryFlags", m_queryFlags);
}

//-----------------------------------------------------------------------------

void SMesh::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    if (this->getTransformId().empty())
    {
        this->setTransformId(this->getID() + "_TF");
    }

    m_meshGeometry = ::std::make_shared< ::fwRenderOgre::Mesh>(this->getID());
    m_meshGeometry->setDynamic(m_isDynamic);
    m_meshGeometry->setDynamicVertices(m_isDynamicVertices);

    // We have to create a new material adaptor only if this adaptor is instanciated by a reconstruction adaptor
    // or if no material adaptor uid has been configured
    m_useNewMaterialAdaptor = m_isReconstructionManaged || m_materialName.empty();

    if(!m_useNewMaterialAdaptor)
    {
        // A material adaptor has been configured in the XML scene
        auto mtlAdaptors = this->getRenderService()->getAdaptors< ::visuOgreAdaptor::SMaterial>();

        auto result =
            std::find_if(mtlAdaptors.begin(), mtlAdaptors.end(), [this](const ::visuOgreAdaptor::SMaterial::sptr& srv)
            {
                return srv->getMaterialName() == m_materialName;
            });

        m_materialAdaptor = *result;

        SLM_ASSERT("SMaterial adaptor managing material'" + m_materialName + "' is not found",
                   result != mtlAdaptors.end());
        m_material = m_materialAdaptor->getInOut< ::fwData::Material >(SMaterial::s_MATERIAL_INOUT);
    }

    ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >(s_MESH_INOUT);
    this->updateMesh(mesh);
}

//-----------------------------------------------------------------------------

void SMesh::stopping()
{
    this->getRenderService()->makeCurrent();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);

    this->unregisterServices();

    m_meshGeometry->clearMesh(*sceneMgr);
    m_materialAdaptor.reset();

    if(m_entity)
    {
        sceneMgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    m_meshGeometry.reset();
}

//-----------------------------------------------------------------------------

void SMesh::updating()
{
    if((m_isDynamic || m_isDynamicVertices) && (!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }
    ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >(s_MESH_INOUT);
    ::fwData::mt::ObjectReadLock lock(mesh);

    if(m_meshGeometry->hasColorLayerChanged(mesh))
    {
        ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
        SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);
        m_meshGeometry->clearMesh(*sceneMgr);
    }
    this->updateMesh(mesh);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::updateMesh(const ::fwData::Mesh::sptr& _mesh)
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

    if(!m_entity)
    {
        m_entity = m_meshGeometry->createEntity(*sceneMgr);
        m_entity->setVisible(m_isVisible);
        m_entity->addQueryFlags(m_queryFlags);
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
        this->updateNewMaterialAdaptor();
    }
    else
    {
        this->updateXMLMaterialAdaptor();
    }
    this->attachNode(m_entity);

    auto r2vbRenderables = m_meshGeometry->updateR2VB(_mesh, *sceneMgr,
                                                      m_materialAdaptor->getMaterialName(),
                                                      m_materialAdaptor->hasDiffuseTexture());
    for(auto renderable : r2vbRenderables.second)
    {
        auto adaptor = renderable->m_materialAdaptor.lock();

        if(r2vbRenderables.first)
        {
            if(adaptor)
            {
                auto r2vbMtlAdaptor = ::visuOgreAdaptor::SMaterial::dynamicCast(adaptor);
                m_meshGeometry->updateMaterial(r2vbMtlAdaptor->getMaterialFw(), true);
                // Update the material *synchronously* otherwise the r2vb will be rendered before the shader switch
                r2vbMtlAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->run();
            }
            else
            {
                // Instantiate a material adaptor for the r2vb process for this primitive type
                adaptor = this->createMaterialService(renderable->getName());

                auto r2vbMtlAdaptor = ::visuOgreAdaptor::SMaterial::dynamicCast(adaptor);
                r2vbMtlAdaptor->setR2VBObject(renderable);
                r2vbMtlAdaptor->start();
                m_meshGeometry->updateMaterial(r2vbMtlAdaptor->getMaterialFw(), true);
                r2vbMtlAdaptor->update();

                renderable->setRenderToBufferMaterial(r2vbMtlAdaptor->getMaterialName());
                renderable->m_materialAdaptor = r2vbMtlAdaptor;
            }
            // Attach r2vb object in the scene graph
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

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
}

//------------------------------------------------------------------------------

::visuOgreAdaptor::SMaterial::sptr SMesh::createMaterialService(const std::string& _materialSuffix)
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

    const std::string meshName = this->getInOut< ::fwData::Mesh >(s_MESH_INOUT)->getID();
    const std::string mtlName  = meshName + "_" + materialAdaptor->getID() + _materialSuffix;

    materialAdaptor->setMaterialName(mtlName);
    materialAdaptor->setTextureName(m_textureName);
    materialAdaptor->setShadingMode(m_shadingMode);

    return materialAdaptor;
}

//------------------------------------------------------------------------------

void SMesh::updateNewMaterialAdaptor()
{
    if(!m_materialAdaptor)
    {
        if(m_entity)
        {
            m_materialAdaptor = this->createMaterialService();
            m_materialAdaptor->start();

            m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
            m_materialAdaptor->update();

            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());
        }
    }
    else if(m_materialAdaptor->getInOut< ::fwData::Material >(SMaterial::s_MATERIAL_INOUT) != m_material)
    {
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
        m_materialAdaptor->swap(m_material);
    }
    else
    {
        m_entity->setMaterialName(m_materialAdaptor->getMaterialName());
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
        m_materialAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->run();
    }
}

//------------------------------------------------------------------------------

void SMesh::updateXMLMaterialAdaptor()
{
    if(m_materialAdaptor->getUpdatingStatus() == UpdatingStatus::NOTUPDATING)
    {
        if(m_materialAdaptor->getMaterialName().empty())
        {
            std::string meshName = this->getInOut< ::fwData::Mesh >(s_MESH_INOUT)->getID();
            m_materialAdaptor->setMaterialName(meshName + "_Material");
        }

        if(m_entity)
        {
            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());
            m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);

            m_materialAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->run();
        }
    }
    else if(m_materialAdaptor->getInOut< ::fwData::Material >(SMaterial::s_MATERIAL_INOUT) != m_material)
    {
        m_meshGeometry->updateMaterial(m_materialAdaptor->getMaterialFw(), false);
        m_materialAdaptor->swap(m_material);
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

    ::fwData::Mesh::csptr mesh = this->getInOut< ::fwData::Mesh >(s_MESH_INOUT);
    ::fwData::mt::ObjectReadLock lock(mesh);
    m_meshGeometry->updateVertices(mesh);

    // Necessary to update the bounding box in the adaptor
    //m_materialAdaptor->slot(::visuOgreAdaptor::SMaterial::s_UPDATE_SLOT)->asyncRun();

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

    ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >(s_MESH_INOUT);
    ::fwData::mt::ObjectReadLock lock(mesh);

    if(m_meshGeometry->hasColorLayerChanged(mesh))
    {
        ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
        SLM_ASSERT("::Ogre::SceneManager is null", sceneMgr);
        m_meshGeometry->clearMesh(*sceneMgr);
        this->updateMesh(mesh);
    }
    else
    {
        m_meshGeometry->updateColors(mesh);
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

    ::fwData::Mesh::csptr mesh = this->getInOut< ::fwData::Mesh >(s_MESH_INOUT);
    ::fwData::mt::ObjectReadLock lock(mesh);
    m_meshGeometry->updateTexCoords(mesh);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMesh::attachNode(::Ogre::MovableObject* _node)
{
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    transNode->attachObject(_node);
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMesh::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_MESH_INOUT, ::fwData::Mesh::s_VERTEX_MODIFIED_SIG, s_MODIFY_VERTICES_SLOT );
    connections.push( s_MESH_INOUT, ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG, s_MODIFY_COLORS_SLOT );
    connections.push( s_MESH_INOUT, ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG, s_MODIFY_COLORS_SLOT );
    connections.push( s_MESH_INOUT, ::fwData::Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG, s_MODIFY_POINT_TEX_COORDS_SLOT );
    connections.push( s_MESH_INOUT, ::fwData::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SMesh::requestRender()
{
    m_meshGeometry->invalidateR2VB();

    ::fwRenderOgre::IAdaptor::requestRender();
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor

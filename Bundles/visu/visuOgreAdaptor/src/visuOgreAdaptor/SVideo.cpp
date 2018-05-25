/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SVideo.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>

#include <OGRE/OgreCamera.h>
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
#include <OGRE/OgreTextureManager.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVideo, ::fwData::Image);

static const std::string VIDEO_MATERIAL_NAME            = "Video";
static const std::string VIDEO_WITHTF_MATERIAL_NAME     = "VideoWithTF";
static const std::string VIDEO_WITHTF_INT_MATERIAL_NAME = "VideoWithTF_Int";

//------------------------------------------------------------------------------

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "image";
static const ::fwServices::IService::KeyType s_TF_INPUT    = "tf";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_SLOT = "updateTF";

//------------------------------------------------------------------------------
SVideo::SVideo() noexcept
{
    newSlot(s_UPDATE_TF_SLOT, &SVideo::updateTF, this);
}

//------------------------------------------------------------------------------

SVideo::~SVideo() noexcept
{
}

//------------------------------------------------------------------------------

void SVideo::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_reverse = config.get<bool>("reverse", false);
}

//------------------------------------------------------------------------------

void SVideo::starting()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SVideo::stopping()
{
    this->clearEntity();

    m_material.reset();
    m_texture.reset();
    m_gpuTF.reset();

    m_isTextureInit = false;
}

//------------------------------------------------------------------------------

void SVideo::updating()
{
    this->getRenderService()->makeCurrent();

    // Getting FW4SPL Image
    ::fwData::Image::csptr imageF4s = this->getInput< ::fwData::Image>(s_IMAGE_INPUT);
    SLM_ASSERT("Problem getting the image", imageF4s);

    {
        ::fwData::mt::ObjectReadLock lock(imageF4s);

        auto type = imageF4s->getType();

        if (!m_isTextureInit || type != m_previousType )
        {
            // /////////////////////////////////////////////////////////////////////
            // Create the appropriate material according to the texture format
            // /////////////////////////////////////////////////////////////////////
            if(!m_texture)
            {
                auto texture = ::Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(
                    this->getID() + "_VideoTexture",
                    ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    true).first;

                m_texture = ::Ogre::dynamic_pointer_cast< ::Ogre::Texture>( texture );
            }

            auto& mtlMgr = ::Ogre::MaterialManager::getSingleton();
            auto tf      = this->getInput< ::fwData::TransferFunction>(s_TF_INPUT);

            ::Ogre::MaterialPtr defaultMat;
            if(tf)
            {
                if(type == ::fwTools::Type::s_FLOAT || type == ::fwTools::Type::s_DOUBLE)
                {
                    defaultMat = mtlMgr.getByName(VIDEO_WITHTF_MATERIAL_NAME);
                }
                else
                {
                    defaultMat = mtlMgr.getByName(VIDEO_WITHTF_INT_MATERIAL_NAME);
                }
            }
            else
            {
                defaultMat = mtlMgr.getByName(VIDEO_MATERIAL_NAME);
            }

            // Duplicate the template material to create our own material
            auto material = ::Ogre::MaterialManager::getSingletonPtr()->createOrRetrieve(
                this->getID() + "_VideoMaterial",
                ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                true).first;
            m_material = ::Ogre::dynamic_pointer_cast< ::Ogre::Material>( material );

            defaultMat->copyDetailsTo(m_material);

            // Set the texture to the main material pass
            ::Ogre::Pass* ogrePass = m_material->getTechnique(0)->getPass(0);
            ogrePass->getTextureUnitState("image")->setTexture(m_texture);

            if(tf)
            {
                // TF texture initialization
                m_gpuTF = ::boost::make_unique< ::fwRenderOgre::TransferFunction>();
                m_gpuTF->createTexture(this->getID());

                this->updateTF();
            }
            m_previousType = type;
        }

        ::fwData::Image::SizeType size = imageF4s->getSize();
        ::fwRenderOgre::Utils::loadOgreTexture(imageF4s, m_texture, ::Ogre::TEX_TYPE_2D, true);

        if (!m_isTextureInit || size[0] != m_previousWidth || size[1] != m_previousHeight )
        {
            this->clearEntity();

            // /////////////////////////////////////////////////////////////////////
            // Create the plane entity
            // /////////////////////////////////////////////////////////////////////
            const std::string thisID        = this->getID();
            const std::string videoMeshName = thisID + "_VideoMesh";
            const std::string entityName    = thisID + "_VideoEntity";
            const std::string nodeName      = thisID + "_VideoSceneNode";

            ::Ogre::MovablePlane plane( ::Ogre::Vector3::UNIT_Z, 0 );

            ::Ogre::SceneManager* sceneManager = this->getSceneManager();
            ::Ogre::MeshManager& meshManager   = ::Ogre::MeshManager::getSingleton();

            m_mesh = meshManager.createPlane(videoMeshName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                             plane, static_cast< ::Ogre::Real >(size[0]),
                                             static_cast< ::Ogre::Real >(size[1]));

            // Create Ogre Entity
            m_entity = sceneManager->createEntity(entityName, videoMeshName);
            m_entity->setMaterial(m_material);

            // Add the entity to the scene
            m_sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
            m_sceneNode->attachObject(m_entity);

            // Slightly offset the plane in Z to allow some space for other entities, thus they can be rendered on top
            m_sceneNode->setPosition(0, 0, -1);

            ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();
            SLM_ASSERT("Default camera not found", cam);
            cam->setProjectionType(::Ogre::PT_ORTHOGRAPHIC);
            cam->setOrthoWindowHeight(static_cast< ::Ogre::Real >(size[1]));

            m_isTextureInit = true;
        }

        m_previousWidth  = size[0];
        m_previousHeight = size[1];
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::updateTF()
{
    ::fwData::TransferFunction::csptr tf = this->getInput< ::fwData::TransferFunction>(s_TF_INPUT);
    SLM_ASSERT("input '" + s_TF_INPUT + "' is missing.", tf);

    m_gpuTF->updateTexture(tf);

    ::Ogre::Pass* ogrePass = m_material->getTechnique(0)->getPass(0);
    m_gpuTF->bind(ogrePass, "tf", ogrePass->getFragmentProgramParameters());

    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::clearEntity()
{
    if(m_entity)
    {
        m_entity->detachFromParent();
        this->getSceneManager()->destroyEntity(m_entity);
    }
    if(m_sceneNode)
    {
        m_sceneNode->removeAndDestroyAllChildren();
        this->getSceneManager()->destroySceneNode(m_sceneNode);
        m_sceneNode = nullptr;
    }
    if (m_mesh)
    {
        ::Ogre::MeshManager& meshManager = ::Ogre::MeshManager::getSingleton();
        meshManager.remove(m_mesh->getName());
        m_mesh.reset();
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVideo::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );

    connections.push( s_TF_INPUT, ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG, s_UPDATE_TF_SLOT);
    connections.push( s_TF_INPUT, ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG, s_UPDATE_TF_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor

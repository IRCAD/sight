/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SVideo.hpp"

#include <arData/Camera.hpp>

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
    m_texture.reset();
    m_material.reset();
    m_gpuTF.reset();
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

            m_texture = ::Ogre::TextureManager::getSingletonPtr()->create(
                this->getID() + "_VideoTexture",
                ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                true);

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
            // Duplicate default material to create Video material
            m_material = mtlMgr.create( this->getID() + "_VideoMaterial",
                                        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

            defaultMat->copyDetailsTo(m_material);

            // Set the texture to the main material pass
            ::Ogre::Pass* ogrePass = m_material->getTechnique(0)->getPass(0);
            ogrePass->getTextureUnitState("image")->setTexture(m_texture);

            if(tf)
            {
                // TF texture initialization
                m_gpuTF = ::boost::make_unique< ::fwRenderOgre::TransferFunction>();
                m_gpuTF->createTexture(this->getID());

                ::Ogre::Pass* ogrePass = m_material->getTechnique(0)->getPass(0);
                ogrePass->getTextureUnitState("tf")->setTexture(m_gpuTF->getTexture());

                this->updateTF();
            }
            m_previousType = type;
        }

        ::fwData::Image::SizeType size = imageF4s->getSize();
        ::fwRenderOgre::Utils::loadOgreTexture(imageF4s, m_texture, ::Ogre::TEX_TYPE_2D, true);

        if (!m_isTextureInit || size[0] != m_previousWidth || size[1] != m_previousHeight )
        {
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

            // Delete deprecated Ogre resources if necessary
            if (meshManager.resourceExists(videoMeshName))
            {
                ::Ogre::ResourcePtr mesh = meshManager.getResourceByName(videoMeshName);
                meshManager.remove(mesh);
                sceneManager->destroyEntity(entityName);
                sceneManager->getRootSceneNode()->removeAndDestroyChild(nodeName);
            }

            meshManager.createPlane(videoMeshName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                    plane, static_cast< ::Ogre::Real >(size[0]), static_cast< ::Ogre::Real >(size[1]));

            // Create Ogre Entity
            ::Ogre::Entity* ent = sceneManager->createEntity(entityName, videoMeshName);

            ent->setMaterial(m_material);

            // Add the entity to the scene
            ::Ogre::SceneNode* sn = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
            sn->attachObject(ent);
            sn->setPosition(0, 0, 0);

            ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();
            cam->setProjectionType(::Ogre::PT_ORTHOGRAPHIC);
            cam->setOrthoWindowHeight(static_cast< ::Ogre::Real >(size[1]));

            m_isTextureInit = true;

            ::arData::Camera::csptr camera = this->getInput< ::arData::Camera>("camera");
            if(camera)
            {
                if(camera->getIsCalibrated())
                {
                    const float shiftX = static_cast<float>(size[0] ) / 2.f - static_cast<float>(camera->getCx());
                    const float shiftY = static_cast<float>(size[1] ) / 2.f - static_cast<float>(camera->getCy());

                    auto node = cam->getSceneManager()->getSceneNode(::fwRenderOgre::Layer::s_DEFAULT_CAMERA_NODE_NAME);
                    if (m_reverse)
                    {
                        node->setPosition(shiftX, -shiftY, 0);
                    }
                    else
                    {
                        node->setPosition(-shiftX, shiftY, 0);
                    }
                }
            }
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

    this->requestRender();
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

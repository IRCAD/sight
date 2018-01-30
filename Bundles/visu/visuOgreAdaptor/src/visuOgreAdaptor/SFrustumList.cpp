/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SFrustumList.hpp"

#include <arData/Camera.hpp>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/helper/Scene.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SFrustumList);

const ::fwCom::Slots::SlotKeyType SFrustumList::s_CLEAR_SLOT             = "clear";
const ::fwCom::Slots::SlotKeyType SFrustumList::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SFrustumList::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const ::fwCom::Slots::SlotKeyType SFrustumList::s_ADD_FRUSTUM_SLOT       = "addFrustum";

static const std::string s_IN_CAMERA_NAME = "camera";
static const std::string s_IN_TRANSFORM   = "transform";

//-----------------------------------------------------------------------------

SFrustumList::SFrustumList() noexcept :
    m_visibility(true),
    m_near(1.f),
    m_far(100.f),
    m_color("#00ff00ff"),
    m_capacity(50),
    m_currentCamIndex(0)
{

    newSlot(s_UPDATE_VISIBILITY_SLOT, &SFrustumList::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SFrustumList::toggleVisibility, this);
    newSlot(s_CLEAR_SLOT, &SFrustumList::clear, this);
    newSlot(s_ADD_FRUSTUM_SLOT, &SFrustumList::addFrustum, this);

}

//-----------------------------------------------------------------------------

SFrustumList::~SFrustumList() noexcept
{

}

//-----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SFrustumList::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_IN_TRANSFORM, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_ADD_FRUSTUM_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SFrustumList::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_near = config.get<float>("near", 1.f);
    m_far  = config.get<float>("far", 100.f);

    m_color    = config.get< std::string >("color", "#00ff00ff");
    m_capacity = config.get< unsigned int > ("nbMax", 50);

}

//-----------------------------------------------------------------------------

void SFrustumList::starting()
{
    this->initialize();

    m_frustumList.set_capacity(m_capacity);

    // Create material
    ::fwData::Material::sptr material = ::fwData::Material::New();

    material->diffuse()->setRGBA(m_color);

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(material, ::visuOgreAdaptor::SMaterial::s_INOUT_MATERIAL, true);
    m_materialAdaptor->setID(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(this->m_layerID);
    m_materialAdaptor->setShadingMode("ambient");
    m_materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();
    m_materialAdaptor->update();

}

//-----------------------------------------------------------------------------

void SFrustumList::updateVisibility(bool isVisible)
{
    m_visibility = isVisible;
    this->updateAllVisibility();
}

//-----------------------------------------------------------------------------

void SFrustumList::toggleVisibility()
{
    m_visibility = !m_visibility;
    this->updateAllVisibility();
}

//-----------------------------------------------------------------------------

void SFrustumList::addFrustum()
{

    //Get camera parameters
    const std::shared_ptr< const ::arData::Camera > fwCamera = this->getInput< ::arData::Camera >(s_IN_CAMERA_NAME);

    SLM_ASSERT("Required input '" + s_IN_CAMERA_NAME + "' is not set", fwCamera);

    const auto h    = static_cast<float>(fwCamera->getHeight());
    const auto fy   = fwCamera->getFy();
    const auto fovY = 2.f * std::atan( static_cast<float>( h / (2.f * fy)));

    ::Ogre::Camera* camera;
    camera = this->getSceneManager()->createCamera(::Ogre::String(this->getID()+"_camera"
                                                                  + std::to_string(m_currentCamIndex)));

    camera->setFOVy(::Ogre::Radian( ::Ogre::Real(fovY)));
    camera->setAspectRatio(::Ogre::Real(fwCamera->getWidth()/fwCamera->getHeight()));
    camera->setVisible(m_visibility);

    camera->setMaterial(m_materialAdaptor->getMaterial());
    camera->setDirection(::Ogre::Vector3(::Ogre::Real(0), ::Ogre::Real(0), ::Ogre::Real(1)));
    camera->setDebugDisplayEnabled(m_visibility);

    ::Ogre::Matrix4 ogreMat;
    auto fwTransform = this->getInput< ::fwData::TransformationMatrix3D >(s_IN_TRANSFORM);

    // Multithreaded lock
    {
        ::fwData::mt::ObjectReadLock lock(fwTransform);

        for(size_t lt = 0; lt < 4; lt++)
        {
            for(size_t ct = 0; ct < 4; ct++)
            {
                ogreMat[ct][lt] = static_cast< ::Ogre::Real >(fwTransform->getCoefficient(ct, lt));
            }
        }
    }

    // Decompose the matrix
    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    ogreMat.decomposition(position, scale, orientation);

    camera->setOrientation(orientation);
    camera->setPosition(position);

    // Clipping
    camera->setNearClipDistance(m_near);
    camera->setFarClipDistance(m_far);

    //if the list is not full
    if(!m_frustumList.full())
    {
        m_frustumList.push_front(camera);
    }
    else
    {
        //Remove the oldest one
        this->getSceneManager()->destroyCamera(m_frustumList.back());
        //Add the new one
        m_frustumList.push_front(camera);
    }

    m_currentCamIndex++;

    this->updating();
}

//-----------------------------------------------------------------------------

void SFrustumList::updateAllVisibility()
{
    for(auto it : m_frustumList)
    {
        it->setDebugDisplayEnabled(m_visibility);
    }
}

//-----------------------------------------------------------------------------

void SFrustumList::clear()
{
    for(auto it : m_frustumList)
    {
        this->getSceneManager()->destroyCamera(it);
    }

    m_frustumList.clear();
}

//-----------------------------------------------------------------------------

void SFrustumList::updating()
{
    // Add camera to ogre scene
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);
    if (transNode == nullptr)
    {
        transNode = rootSceneNode->createChildSceneNode(this->getID() + "_transform_"
                                                        + std::to_string(m_currentCamIndex));
    }
    transNode->attachObject(m_frustumList.front());

    this->requestRender();

}

//-----------------------------------------------------------------------------

void SFrustumList::stopping()
{
    this->unregisterServices();
    //m_materialAdaptor->stop();
    this->clear();
}

//-----------------------------------------------------------------------------

}

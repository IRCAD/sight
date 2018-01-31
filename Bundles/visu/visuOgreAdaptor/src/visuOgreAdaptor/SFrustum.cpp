/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SFrustum.hpp"

#include "fwRenderOgre/helper/Scene.hpp"

#include "visuOgreAdaptor/SCamera.hpp"
#include <visuOgreAdaptor/SMaterial.hpp>

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreMeshManager.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SFrustum);

const ::fwCom::Slots::SlotKeyType SFrustum::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SFrustum::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

const std::string SFrustum::s_IN_CAMERA_NAME = "camera";
const std::string SFrustum::s_CONFIG_NEAR    = "near";
const std::string SFrustum::s_CONFIG_FAR     = "far";
const std::string SFrustum::s_CONFIG_COLOR   = "color";

//-----------------------------------------------------------------------------

SFrustum::SFrustum() noexcept :
    m_materialAdaptor(nullptr),
    m_visibility(true),
    m_near(0),
    m_far(0),
    m_color()
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SFrustum::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SFrustum::toggleVisibility, this);
}

//-----------------------------------------------------------------------------

SFrustum::~SFrustum() noexcept
{
}

//-----------------------------------------------------------------------------

void SFrustum::updateVisibility(bool isVisible)
{
    m_visibility = isVisible;
    updating();
}

//-----------------------------------------------------------------------------

void SFrustum::toggleVisibility()
{
    m_visibility = !m_visibility;
    updating();
}

//------------------------------------------------------------------------------

void SFrustum::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count(::fwRenderOgre::ITransformable::s_CONFIG_TRANSFORM))
    {
        this->setTransformId(config.get<std::string>(::fwRenderOgre::ITransformable::s_CONFIG_TRANSFORM));
    }
    else
    {
        this->setTransformId(this->getID());
    }

    if(config.count(s_CONFIG_NEAR))
    {
        m_near = config.get<float>(s_CONFIG_NEAR);
    }

    if(config.count(s_CONFIG_FAR))
    {
        m_far = config.get<float>(s_CONFIG_FAR);
    }

    if(config.count(s_CONFIG_COLOR))
    {
        m_color = config.get< std::string >(s_CONFIG_COLOR);
    }
}

//-----------------------------------------------------------------------------

void SFrustum::starting()
{
    this->initialize();

    // Create material
    ::fwData::Material::sptr material = ::fwData::Material::New();

    material->diffuse()->setRGBA(m_color);

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(material, ::visuOgreAdaptor::SMaterial::s_INOUT_MATERIAL, true);
    m_materialAdaptor->setID(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(this->m_layerID);
    m_materialAdaptor->setShadingMode("ambient");
    m_materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();
    m_materialAdaptor->update();

    // Create camera
    m_ogreCam = this->getSceneManager()->createCamera(::Ogre::String(this->getID() + s_IN_CAMERA_NAME));
    m_ogreCam->setPosition(Ogre::Vector3(0, 0, 0));
    m_ogreCam->setMaterial(m_materialAdaptor->getMaterial());
    m_ogreCam->setDirection(::Ogre::Vector3(::Ogre::Real(0), ::Ogre::Real(0), ::Ogre::Real(1)));
    m_ogreCam->setDebugDisplayEnabled(true);

    // Clipping
    if(m_near != 0.f)
    {
        m_ogreCam->setNearClipDistance(m_near);
    }
    if(m_far != 0.f)
    {
        m_ogreCam->setFarClipDistance(m_far);
    }

    // Set data to camera
    setDataToOgreCam();

    // Add camera to ogre scene
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);
    if (transNode == nullptr)
    {
        transNode = rootSceneNode->createChildSceneNode(this->getTransformId());
    }
    transNode->attachObject(m_ogreCam);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SFrustum::updating()
{
    setDataToOgreCam();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SFrustum::stopping()
{
    this->unregisterServices();
    m_materialAdaptor.reset();
}

//-----------------------------------------------------------------------------

void SFrustum::setDataToOgreCam()
{
    const std::shared_ptr< const ::arData::Camera > camera = this->getInput< ::arData::Camera >(s_IN_CAMERA_NAME);
    if(camera != nullptr)
    {
        const auto h    = static_cast<float>(camera->getHeight());
        const auto fy   = camera->getFy();
        const auto fovY = 2.f * std::atan( static_cast<float>( h / (2.f * fy)));

        m_ogreCam->setFOVy(::Ogre::Radian( ::Ogre::Real(fovY)));
        m_ogreCam->setAspectRatio(::Ogre::Real(camera->getWidth()/camera->getHeight()));
        m_ogreCam->setVisible(m_visibility);
    }
    else
    {
        SLM_WARN("the input '" + s_IN_CAMERA_NAME + "' is not set");
    }
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor

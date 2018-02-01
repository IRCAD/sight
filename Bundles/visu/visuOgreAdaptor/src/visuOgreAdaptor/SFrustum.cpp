/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SFrustum.hpp"

#include "visuOgreAdaptor/SCamera.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>

#include <fwRenderOgre/helper/Scene.hpp>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreMeshManager.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SFrustum);

const ::fwCom::Slots::SlotKeyType SFrustum::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SFrustum::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

const std::string SFrustum::s_IN_CAMERA    = "camera";
const std::string SFrustum::s_CONFIG_NEAR  = "near";
const std::string SFrustum::s_CONFIG_FAR   = "far";
const std::string SFrustum::s_CONFIG_COLOR = "color";

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

//------------------------------------------------------------------------------

void SFrustum::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_CONFIG_TRANSFORM, this->getID() ));

    m_near  = config.get<float>(s_CONFIG_NEAR, 0.f);
    m_far   = config.get<float>(s_CONFIG_FAR, 0.f);
    m_color = config.get< std::string >(s_CONFIG_COLOR, "#ff0000ff");
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
    m_ogreCam = this->getSceneManager()->createCamera(::Ogre::String(this->getID() + s_IN_CAMERA));
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
    this->setOgreCamFromData();

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
    this->setOgreCamFromData();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SFrustum::stopping()
{
    this->unregisterServices();
    m_materialAdaptor.reset();
}

//-----------------------------------------------------------------------------

void SFrustum::setOgreCamFromData()
{
    const std::shared_ptr< const ::arData::Camera > camera = this->getInput< ::arData::Camera >(s_IN_CAMERA);
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
        SLM_WARN("the input '" + s_IN_CAMERA + "' is not set");
    }
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor

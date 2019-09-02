/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SVolumeRender.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRenderOgre/helper/Scene.hpp>
#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_NEW_IMAGE_SLOT            = "newImage";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT         = "updateImage";
static const ::fwCom::Slots::SlotKeyType s_TOGGLE_WIDGETS_SLOT       = "toggleWidgets";
static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT    = "updateVisibility";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_CLIPPING_BOX_SLOT  = "updateClippingBox";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT           = "image";
static const ::fwServices::IService::KeyType s_VOLUME_TF_INOUT       = "tf";
static const ::fwServices::IService::KeyType s_CLIPPING_MATRIX_INOUT = "clippingMatrix";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() noexcept :
    m_helperVolumeTF(std::bind(&SVolumeRender::updateVolumeTF, this))
{
    /// Handle connections between the layer and the volume renderer.
    newSlot(s_NEW_IMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_UPDATE_IMAGE_SLOT, &SVolumeRender::updateImage, this);
    newSlot(s_TOGGLE_WIDGETS_SLOT, &SVolumeRender::toggleWidgets, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SVolumeRender::setBoolParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SVolumeRender::setIntParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SVolumeRender::setDoubleParameter, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SVolumeRender::updateVisibility, this);
    newSlot(s_UPDATE_CLIPPING_BOX_SLOT, &SVolumeRender::updateClippingBox, this);
}

//-----------------------------------------------------------------------------

SVolumeRender::~SVolumeRender() noexcept
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_autoResetCamera        = config.get<std::string>("autoresetcamera", "yes") == "yes";
    m_preIntegratedRendering = config.get<std::string>("preintegration", "no") == "yes";
    m_widgetVisibilty        = config.get<std::string>("widgets", "yes") == "yes";
    m_nbSamples              = config.get<std::uint16_t>("samples", m_nbSamples);

    // Advanced illumination parameters.
    m_satSizeRatio        = config.get<float>("satSizeRatio", m_satSizeRatio);
    m_satShells           = config.get<int>("satShells", m_satShells);
    m_satShellRadius      = config.get<int>("satShellRadius", m_satShellRadius);
    m_satConeAngle        = config.get<float>("satConeAngle", m_satConeAngle);
    m_satConeSamples      = config.get<int>("satConeSamples", m_satConeSamples);
    m_aoFactor            = config.get<double>("aoFactor", m_aoFactor);
    m_colorBleedingFactor = config.get<double>("colorBleedingFactor", m_colorBleedingFactor);
    m_ambientOcclusion    = config.get<bool>("ao", false);
    m_colorBleeding       = config.get<bool>("colorBleeding", false);
    m_shadows             = config.get<bool>("shadows", false);

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG,
                                                  this->getID() + "_transform"));
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateVolumeTF()
{
    this->getRenderService()->makeCurrent();

    ::fwData::TransferFunction::sptr tf = m_helperVolumeTF.getTransferFunction();
    {
        ::fwData::mt::ObjectWriteLock tfLock(tf);

        m_gpuVolumeTF->updateTexture(tf);

        if(m_preIntegratedRendering)
        {
            m_preIntegrationTable.tfUpdate(tf, m_volumeRenderer->getSamplingRate());
        }

        m_volumeRenderer->updateVolumeTF();

        if(m_ambientOcclusion || m_colorBleeding || m_shadows)
        {
            this->updateVolumeIllumination();
        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVolumeRender::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;

    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT );
    connections.push( s_CLIPPING_MATRIX_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG,
                      s_UPDATE_CLIPPING_BOX_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    m_gpuVolumeTF = std::make_shared< ::fwRenderOgre::TransferFunction>();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT +"' is missing.", image);

    ::fwData::TransferFunction::sptr volumeTF = this->getInOut< ::fwData::TransferFunction>(s_VOLUME_TF_INOUT);
    m_helperVolumeTF.setOrCreateTF(volumeTF, image);

    m_sceneManager = this->getSceneManager();

    ::Ogre::SceneNode* rootSceneNode = m_sceneManager->getRootSceneNode();
    ::Ogre::SceneNode* transformNode = this->getTransformNode(rootSceneNode);
    m_volumeSceneNode                = transformNode->createChildSceneNode(this->getID() + "_transform_origin");
    m_volumeSceneNode->setVisible(true, false);

    m_camera = this->getLayer()->getDefaultCamera();

    // Create textures
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    m_gpuVolumeTF->createTexture(this->getID() + "_VolumeGpuTF");

    m_preIntegrationTable.createTexture(this->getID());

    ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);

    m_volumeRenderer = new ::fwRenderOgre::vr::RayTracingVolumeRenderer(this->getID(),
                                                                        layer,
                                                                        m_volumeSceneNode,
                                                                        m_3DOgreTexture,
                                                                        m_gpuVolumeTF,
                                                                        m_preIntegrationTable,
                                                                        m_ambientOcclusion,
                                                                        m_colorBleeding);

    // Initially focus on the image center.
    this->setFocalDistance(50);

    m_gpuVolumeTF->setSampleDistance(m_volumeRenderer->getSamplingRate());

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    const bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);
    if (isValid)
    {
        this->newImage();
    }

    m_volumeRenderer->updateVolumeTF();

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::stopping()
{
    this->getRenderService()->makeCurrent();

    m_helperVolumeTF.removeTFConnections();

    m_volumeConnection.disconnect();
    delete m_volumeRenderer;
    m_volumeRenderer = nullptr;

    this->getSceneManager()->destroySceneNode(m_volumeSceneNode);

    ::Ogre::SceneNode* rootSceneNode = m_sceneManager->getRootSceneNode();
    auto transformNode = this->getTransformNode(rootSceneNode);

    m_sceneManager->getRootSceneNode()->removeChild(transformNode);
    this->getSceneManager()->destroySceneNode(static_cast< ::Ogre::SceneNode*>(transformNode));

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture->getHandle());
    m_3DOgreTexture.reset();

    m_gpuVolumeTF.reset();

    m_preIntegrationTable.removeTexture();

    m_illum.reset();

    this->destroyWidget();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updating()
{
}

//------------------------------------------------------------------------------

void SVolumeRender::swapping(const KeyType& key)
{
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    if (key == s_VOLUME_TF_INOUT)
    {
        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_VOLUME_TF_INOUT);
        m_helperVolumeTF.setOrCreateTF(tf, image);

        this->updateVolumeTF();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::newImage()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    ::fwData::TransferFunction::sptr volumeTF = this->getInOut< ::fwData::TransferFunction>(s_VOLUME_TF_INOUT);
    SLM_ASSERT("inout '" + s_VOLUME_TF_INOUT + "' is missing", volumeTF);

    m_helperVolumeTF.setOrCreateTF(volumeTF, image);

    m_gpuVolumeTF->updateTexture(volumeTF);

    this->updateImage();
}

//-----------------------------------------------------------------------------

void SVolumeRender::resetCameraPosition(const ::fwData::Image::csptr& image)
{
    if (m_autoResetCamera || image->getField("resetCamera"))
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
    else
    {
        this->getLayer()->computeCameraParameters();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateImage()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    this->getRenderService()->makeCurrent();

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
    {
        m_gpuVolumeTF->getTexture();

        if(m_preIntegratedRendering)
        {
            m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
        }

        if(m_ambientOcclusion || m_colorBleeding || m_shadows)
        {
            if(m_illum == nullptr)
            {
                m_illum = std::make_shared< ::fwRenderOgre::vr::SATVolumeIllumination>(this->getID(), m_sceneManager,
                                                                                       m_satSizeRatio,
                                                                                       (m_ambientOcclusion ||
                                                                                        m_colorBleeding), m_shadows,
                                                                                       m_satShells, m_satShellRadius,
                                                                                       m_satConeAngle,
                                                                                       m_satConeSamples);
            }
            this->updateVolumeIllumination();
        }
        m_volumeRenderer->imageUpdate(image, volumeTF);
    }

    // Create widgets on image update to take the image's size into account.
    this->createWidget();
    this->resetCameraPosition(image);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSampling(int nbSamples)
{
    this->getRenderService()->makeCurrent();

    OSLM_ASSERT("Sampling rate must fit in a 16 bit uint.", nbSamples < 65536 && nbSamples >= 0);
    m_nbSamples = static_cast<std::uint16_t>(nbSamples);

    m_volumeRenderer->setSampling(m_nbSamples);
    m_gpuVolumeTF->setSampleDistance(m_volumeRenderer->getSamplingRate());

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->updateVolumeIllumination();
    }

    if(m_preIntegratedRendering)
    {
        ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
        ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
        m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateOpacityCorrection(int opacityCorrection)
{
    m_volumeRenderer->setOpacityCorrection(opacityCorrection);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateAOFactor(double aoFactor)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_aoFactor = aoFactor;

        m_volumeRenderer->setAOFactor(m_aoFactor);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateColorBleedingFactor(double colorBleedingFactor)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_colorBleedingFactor = colorBleedingFactor;

        m_volumeRenderer->setColorBleedingFactor(m_colorBleedingFactor);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatSizeRatio(int sizeRatio)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        float scaleCoef(.25f);
        m_satSizeRatio = static_cast<float>(sizeRatio) * scaleCoef;
        m_illum->updateSatFromRatio(m_satSizeRatio);

        this->updateVolumeIllumination();

        if(m_preIntegratedRendering)
        {
            ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
            SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

            ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
            ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
            m_volumeRenderer->imageUpdate(image, volumeTF);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatShellsNumber(int shellsNumber)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satShells = shellsNumber;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setNbShells(m_satShells);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatShellRadius(int shellRadius)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satShellRadius = shellRadius;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setShellRadius(m_satShellRadius);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatConeAngle(int coneAngle)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satConeAngle = static_cast<float>(coneAngle) / 100;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setConeAngle(m_satConeAngle);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatConeSamples(int nbConeSamples)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satConeSamples = nbConeSamples;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setSamplesAlongCone(m_satConeSamples);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::togglePreintegration(bool preintegration)
{
    this->getRenderService()->makeCurrent();

    m_preIntegratedRendering = preintegration;

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    if(m_preIntegratedRendering)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

        ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
        ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
        m_volumeRenderer->imageUpdate(image, volumeTF);
        m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleAmbientOcclusion(bool ambientOcclusion)
{
    m_ambientOcclusion = ambientOcclusion;
    this->toggleVREffect(::visuOgreAdaptor::SVolumeRender::VR_AMBIENT_OCCLUSION);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleColorBleeding(bool colorBleeding)
{
    m_colorBleeding = colorBleeding;
    this->toggleVREffect(::visuOgreAdaptor::SVolumeRender::VR_COLOR_BLEEDING);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleShadows(bool shadows)
{
    m_shadows = shadows;
    this->toggleVREffect(::visuOgreAdaptor::SVolumeRender::VR_SHADOWS);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleWidgets(bool visible)
{
    m_widgetVisibilty = visible;

    if(m_widget)
    {
        m_widget->setVisibility(m_widgetVisibilty && m_volumeRenderer->isVisible());

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setFocalDistance(int focalDistance)
{
    if(this->getRenderService()->getLayer(m_layerID)->getStereoMode() !=
       ::fwRenderOgre::compositor::Core::StereoModeType::NONE)
    {
        m_volumeRenderer->setFocalLength(static_cast<float>(focalDistance) / 100);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setBoolParameter(bool val, std::string key)
{
    this->getRenderService()->makeCurrent();

    if(key == "preIntegration")
    {
        this->togglePreintegration(val);
    }
    else if(key == "ambientOcclusion")
    {
        this->toggleAmbientOcclusion(val);
    }
    else if(key == "colorBleeding")
    {
        this->toggleColorBleeding(val);
    }
    else if(key == "shadows")
    {
        this->toggleShadows(val);
    }
    else if(key == "widgets")
    {
        this->toggleWidgets(val);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setIntParameter(int val, std::string key)
{
    this->getRenderService()->makeCurrent();

    if(key == "sampling")
    {
        this->updateSampling(val);
    }
    else if(key == "opacityCorrection")
    {
        this->updateOpacityCorrection(val);
    }
    else if(key == "satSizeRatio")
    {
        this->updateSatSizeRatio(val);
    }
    else if(key == "satShellsNumber")
    {
        this->updateSatShellsNumber(val);
    }
    else if(key == "satShellRadius")
    {
        this->updateSatShellRadius(val);
    }
    else if(key == "satConeAngle")
    {
        this->updateSatConeAngle(val);
    }
    else if(key == "satConeSamples")
    {
        this->updateSatConeSamples(val);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setDoubleParameter(double val, std::string key)
{
    this->getRenderService()->makeCurrent();

    if(key == "aoFactor")
    {
        this->updateAOFactor(val);
    }
    else if(key == "colorBleedingFactor")
    {
        this->updateColorBleedingFactor(val);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::createWidget()
{
    auto clippingMatrix = this->getInOut< ::fwData::TransformationMatrix3D>(s_CLIPPING_MATRIX_INOUT);

    auto clippingMxUpdate = std::bind(&SVolumeRender::updateClippingTM3D, this);

    ::Ogre::Matrix4 ogreClippingMx = ::Ogre::Matrix4::IDENTITY;

    if(clippingMatrix)
    {
        ogreClippingMx = ::fwRenderOgre::Utils::convertTM3DToOgreMx(clippingMatrix);
    }

    this->destroyWidget(); // Destroys the old widgets if they were created.
    m_widget = std::make_shared< ::fwRenderOgre::ui::VRWidget>(this->getID(), m_volumeSceneNode,
                                                               m_camera, m_sceneManager,
                                                               ogreClippingMx, clippingMxUpdate);

    ::fwRenderOgre::Layer::sptr layer                        = this->getLayer();
    ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getMoveInteractor();

    auto vrInteractor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::VRWidgetsInteractor >(interactor);

    if(vrInteractor)
    {
        vrInteractor->initPicker();
        vrInteractor->setWidget(m_widget);
    }

    m_volumeRenderer->clipImage(m_widget->getClippingBox());

    m_widget->setVisibility(m_widgetVisibilty && m_volumeRenderer->isVisible());
}

//-----------------------------------------------------------------------------

void SVolumeRender::destroyWidget()
{
    if(m_widget)
    {
        ::fwRenderOgre::Layer::sptr layer                        = this->getLayer();
        ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getMoveInteractor();

        auto vrInteractor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::VRWidgetsInteractor >(interactor);

        if(vrInteractor)
        {
            vrInteractor->setWidget(nullptr);
        }

        SLM_ASSERT("There should be only one remaining instance of 'm_widget' at this points.",
                   m_widget.use_count() == 1);

        m_widget = nullptr;
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateVolumeIllumination()
{
    this->getRenderService()->makeCurrent();

    m_illum->SATUpdate(m_3DOgreTexture, m_gpuVolumeTF, m_volumeRenderer->getSamplingRate());

    m_volumeRenderer->setIlluminationVolume(m_illum);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleVREffect(::visuOgreAdaptor::SVolumeRender::VREffectType vrEffect)
{
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    // Volume illumination is only implemented for raycasting rendering
    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        if((m_ambientOcclusion || m_colorBleeding || m_shadows) && !m_illum)
        {
            m_illum = ::std::make_shared< ::fwRenderOgre::vr::SATVolumeIllumination>(this->getID(), m_sceneManager,
                                                                                     m_satSizeRatio,
                                                                                     (m_ambientOcclusion ||
                                                                                      m_colorBleeding), m_shadows,
                                                                                     m_satShells, m_satShellRadius);
            this->updateVolumeIllumination();
        }
        else if(m_illum)
        {
            switch(vrEffect)
            {
                case ::visuOgreAdaptor::SVolumeRender::VR_AMBIENT_OCCLUSION:
                case ::visuOgreAdaptor::SVolumeRender::VR_COLOR_BLEEDING:
                    m_illum->setAO(m_ambientOcclusion || m_colorBleeding);
                    break;
                case ::visuOgreAdaptor::SVolumeRender::VR_SHADOWS:
                    m_illum->setShadows(m_shadows);
                    break;
            }

        }

        switch(vrEffect)
        {
            case ::visuOgreAdaptor::SVolumeRender::VR_AMBIENT_OCCLUSION:
                m_volumeRenderer->setAmbientOcclusion(m_ambientOcclusion);
                break;
            case ::visuOgreAdaptor::SVolumeRender::VR_COLOR_BLEEDING:
                m_volumeRenderer->setColorBleeding(m_colorBleeding);
                break;
            case ::visuOgreAdaptor::SVolumeRender::VR_SHADOWS:
                m_volumeRenderer->setShadows(m_shadows);
                break;
        }

        this->updateSampling(m_nbSamples);
        this->updateSatSizeRatio(static_cast<int>(m_satSizeRatio * 4));
        this->updateSatShellsNumber(m_satShells);
        this->updateSatShellRadius(m_satShellRadius);
        this->updateSatConeAngle(static_cast<int>(m_satConeAngle * 100));
        this->updateSatConeSamples(m_satConeSamples);

        if(m_preIntegratedRendering)
        {
            ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
            ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
            m_volumeRenderer->imageUpdate(image, volumeTF);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateClippingBox()
{
    if(m_widget)
    {
        this->getRenderService()->makeCurrent();

        auto clippingMatrix = this->getInOut< ::fwData::TransformationMatrix3D>(s_CLIPPING_MATRIX_INOUT);
        SLM_ASSERT("Can't update the 'clippingMatrix' if it doesn't exist.", clippingMatrix);

        const ::Ogre::Matrix4 clippingMx = ::fwRenderOgre::Utils::convertTM3DToOgreMx(clippingMatrix);

        m_widget->updateFromTransform(clippingMx);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateClippingTM3D()
{
    auto clippingMatrix = this->getInOut< ::fwData::TransformationMatrix3D>(s_CLIPPING_MATRIX_INOUT);

    if(clippingMatrix)
    {
        ::fwRenderOgre::Utils::copyOgreMxToTM3D(m_widget->getClippingTransform(), clippingMatrix);

        auto sig = clippingMatrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);

        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_CLIPPING_BOX_SLOT)));

        sig->asyncEmit();
    }

    m_volumeRenderer->clipImage(m_widget->getClippingBox());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateVisibility(bool visibility)
{
    if(m_volumeSceneNode)
    {
        m_volumeSceneNode->setVisible(visibility);

        if(m_widget)
        {
            m_widget->setVisibility(visibility && m_widgetVisibilty);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

}

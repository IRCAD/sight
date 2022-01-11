/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SVolumeRender.hpp"

#include "modules/viz/scene3d/adaptor/STransform.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <geometry/data/Matrix4.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <viz/scene3d/helper/Scene.hpp>
#include <viz/scene3d/helper/Shading.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>

//-----------------------------------------------------------------------------

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

static const core::com::Slots::SlotKeyType s_NEW_IMAGE_SLOT            = "newImage";
static const core::com::Slots::SlotKeyType s_BUFFER_IMAGE_SLOT         = "bufferImage";
static const core::com::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT         = "updateImage";
static const core::com::Slots::SlotKeyType s_TOGGLE_WIDGETS_SLOT       = "toggleWidgets";
static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
static const core::com::Slots::SlotKeyType s_UPDATE_CLIPPING_BOX_SLOT  = "updateClippingBox";

static const std::string s_AUTORESET_CAMERA_CONFIG      = "autoresetcamera";
static const std::string s_PREINTEGRATION_CONFIG        = "preintegration";
static const std::string s_DYNAMIC_CONFIG               = "dynamic";
static const std::string s_WIDGETS_CONFIG               = "widgets";
static const std::string s_PRIORITY_CONFIG              = "priority";
static const std::string s_LAYER_ORDER_DEPENDANT_CONFIG = "layerOrderDependant";
static const std::string s_SAMPLES_CONFIG               = "samples";
static const std::string s_SAT_SIZE_RATIO_CONFIG        = "satSizeRatio";
static const std::string s_SAT_SHELLS_CONFIG            = "satShells";
static const std::string s_SAT_SHELL_RADIUS_CONFIG      = "satShellRadius";
static const std::string s_SAT_CONE_ANGLE_CONFIG        = "satConeAngle";
static const std::string s_SAT_CONE_SAMPLES_CONFIG      = "satConeSamples";
static const std::string s_AO_FACOTR_CONFIG             = "aoFactor";
static const std::string s_COLOR_BLEEDING_FACTOR_CONFIG = "colorBleedingFactor";
static const std::string s_AO_CONFIG                    = "ao";
static const std::string s_COLOR_BLEEDING_CONFIG        = "colorBleeding";
static const std::string s_SHADOES_CONFIG               = "shadows";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() noexcept :
    m_helperVolumeTF(std::bind(&SVolumeRender::updateVolumeTF, this))
{
    /// Handle connections between the layer and the volume renderer.
    newSlot(s_NEW_IMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_BUFFER_IMAGE_SLOT, &SVolumeRender::bufferImage, this);
    newSlot(s_UPDATE_IMAGE_SLOT, &SVolumeRender::updateImage, this);
    newSlot(s_TOGGLE_WIDGETS_SLOT, &SVolumeRender::toggleWidgets, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SVolumeRender::setBoolParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SVolumeRender::setIntParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SVolumeRender::setDoubleParameter, this);
    newSlot(s_UPDATE_CLIPPING_BOX_SLOT, &SVolumeRender::updateClippingBox, this);
}

//-----------------------------------------------------------------------------

SVolumeRender::~SVolumeRender() noexcept
{
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SVolumeRender::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;

    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_BUFFER_IMAGE_SLOT);
    connections.push(
        s_CLIPPING_MATRIX_INOUT,
        data::Matrix4::s_MODIFIED_SIG,
        s_UPDATE_CLIPPING_BOX_SLOT
    );

    return connections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_autoResetCamera        = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);
    m_preIntegratedRendering = config.get<bool>(s_PREINTEGRATION_CONFIG, false);
    m_dynamic                = config.get<bool>(s_DYNAMIC_CONFIG, m_dynamic);
    m_widgetVisibilty        = config.get<bool>(s_WIDGETS_CONFIG, true);
    m_priority               = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_layerOrderDependant    = config.get<bool>(s_LAYER_ORDER_DEPENDANT_CONFIG, m_layerOrderDependant);
    m_nbSamples              = config.get<std::uint16_t>(s_SAMPLES_CONFIG, m_nbSamples);

    // Advanced illumination parameters.
    m_satSizeRatio        = config.get<float>(s_SAT_SIZE_RATIO_CONFIG, m_satSizeRatio);
    m_satShells           = config.get<int>(s_SAT_SHELLS_CONFIG, m_satShells);
    m_satShellRadius      = config.get<int>(s_SAT_SHELL_RADIUS_CONFIG, m_satShellRadius);
    m_satConeAngle        = config.get<float>(s_SAT_CONE_ANGLE_CONFIG, m_satConeAngle);
    m_satConeSamples      = config.get<int>(s_SAT_CONE_SAMPLES_CONFIG, m_satConeSamples);
    m_aoFactor            = config.get<double>(s_AO_FACOTR_CONFIG, m_aoFactor);
    m_colorBleedingFactor = config.get<double>(s_COLOR_BLEEDING_FACTOR_CONFIG, m_colorBleedingFactor);
    m_ambientOcclusion    = config.get<bool>(s_AO_CONFIG, false);
    m_colorBleeding       = config.get<bool>(s_COLOR_BLEEDING_CONFIG, false);
    m_shadows             = config.get<bool>(s_SHADOES_CONFIG, false);

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );
}

//-----------------------------------------------------------------------------

void SVolumeRender::starting()
{
    this->initialize();

    auto renderService = this->getRenderService();
    renderService->makeCurrent();

    m_gpuVolumeTF = std::make_shared<sight::viz::scene3d::TransferFunction>();

    {
        const auto image = m_image.lock();
        const auto tf    = m_tf.lock();
        m_helperVolumeTF.setOrCreateTF(tf.get_shared(), image.get_shared());
    }

    m_sceneManager = this->getSceneManager();

    ::Ogre::SceneNode* rootSceneNode = m_sceneManager->getRootSceneNode();
    ::Ogre::SceneNode* transformNode = this->getTransformNode(rootSceneNode);
    m_volumeSceneNode = transformNode->createChildSceneNode(this->getID() + "_transform_origin");

    m_camera = this->getLayer()->getDefaultCamera();

    // Create textures
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        sight::viz::scene3d::RESOURCE_GROUP,
        true
    );

    if(m_dynamic)
    {
        m_bufferingTexture = ::Ogre::TextureManager::getSingleton().create(
            this->getID() + "_Texture2",
            sight::viz::scene3d::RESOURCE_GROUP,
            true
        );
    }

    m_gpuVolumeTF->createTexture(this->getID() + "_VolumeGpuTF");

    m_preIntegrationTable.createTexture(this->getID());

    sight::viz::scene3d::Layer::sptr layer = renderService->getLayer(m_layerID);

    m_volumeRenderer = new sight::viz::scene3d::vr::RayTracingVolumeRenderer(
        this->getID(),
        layer,
        m_volumeSceneNode,
        m_3DOgreTexture,
        m_gpuVolumeTF,
        m_preIntegrationTable,
        m_ambientOcclusion,
        m_colorBleeding
    );

    m_volumeSceneNode->setVisible(m_isVisible);

    // Initially focus on the image center.
    this->setFocalDistance(50);

    m_volumeRenderer->setSampling(m_nbSamples);
    m_gpuVolumeTF->setSampleDistance(m_volumeRenderer->getSamplingRate());

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    bool isValid = false;
    {
        const auto image = m_image.lock();
        isValid = data::helper::MedicalImage::checkImageValidity(image.get_shared());
    }
    if(isValid)
    {
        this->newImage();
    }

    m_volumeRenderer->updateVolumeTF();

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updating()
{
}

//------------------------------------------------------------------------------

void SVolumeRender::swapping(std::string_view _key)
{
    if(_key == s_VOLUME_TF_INOUT)
    {
        this->getRenderService()->makeCurrent();
        {
            const auto image = m_image.lock();
            const auto tf    = m_tf.lock();
            m_helperVolumeTF.setOrCreateTF(tf.get_shared(), image.get_shared());
        }
        this->updateVolumeTF();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::stopping()
{
    this->getRenderService()->makeCurrent();

    // First wait on all pending buffering tasks and destroy the worker.
    m_bufferingWorker.reset();

    m_helperVolumeTF.removeTFConnections();

    delete m_volumeRenderer;
    m_volumeRenderer = nullptr;

    this->getSceneManager()->destroySceneNode(m_volumeSceneNode);

    ::Ogre::SceneNode* rootSceneNode = m_sceneManager->getRootSceneNode();
    auto transformNode               = this->getTransformNode(rootSceneNode);

    m_sceneManager->getRootSceneNode()->removeChild(transformNode);
    this->getSceneManager()->destroySceneNode(static_cast< ::Ogre::SceneNode*>(transformNode));

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture->getHandle());
    m_3DOgreTexture.reset();

    if(m_bufferingTexture)
    {
        ::Ogre::TextureManager::getSingleton().remove(m_bufferingTexture->getHandle());
        m_bufferingTexture.reset();
    }

    m_gpuVolumeTF.reset();

    m_preIntegrationTable.removeTexture();

    m_ambientOcclusionSAT.reset();

    this->destroyWidget();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateVolumeTF()
{
    this->getRenderService()->makeCurrent();
    std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);

    const data::TransferFunction::sptr tf = m_helperVolumeTF.getTransferFunction();
    {
        const data::mt::locked_ptr lock(tf);

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

void SVolumeRender::newImage()
{
    auto renderService = this->getRenderService();
    {
        if(m_dynamic)
        {
            // Destroy the worker to wait for all pending buffering tasks to be cleared.
            m_bufferingWorker.reset();

            auto* newWorker = renderService->getInteractorManager()->createGraphicsWorker();
            m_bufferingWorker = std::unique_ptr<sight::viz::scene3d::IGraphicsWorker>(newWorker);
        }

        renderService->makeCurrent();
        {
            const auto image = m_image.lock();
            const auto tf    = m_tf.lock();
            m_helperVolumeTF.setOrCreateTF(tf.get_shared(), image.get_shared());

            sight::viz::scene3d::Utils::convertImageForNegato(m_3DOgreTexture.get(), image.get_shared());
        }
        this->updateVolumeTF();
    }

    this->updateImage();
}

//-----------------------------------------------------------------------------

void SVolumeRender::bufferImage()
{
    if(m_dynamic)
    {
        auto bufferingFn = [this]()
                           {
                               const auto image = m_image.lock();

                               sight::viz::scene3d::Utils::convertImageForNegato(
                                   m_bufferingTexture.get(),
                                   image.get_shared()
                               );

                               // Swap texture pointers.
                               {
                                   std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);
                                   std::swap(m_3DOgreTexture, m_bufferingTexture);
                               }

                               // Switch back to the main thread to compute the proxy geometry.
                               // Ogre can't handle parallel rendering.
                               this->slot(s_UPDATE_IMAGE_SLOT)->asyncRun();
                           };

        m_bufferingWorker->pushTask(bufferingFn);
    }
    else
    {
        this->getRenderService()->makeCurrent();
        {
            const auto image = m_image.lock();
            sight::viz::scene3d::Utils::convertImageForNegato(m_3DOgreTexture.get(), image.get_shared());
        }
        this->updateImage();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateImage()
{
    const auto image = m_image.lock();

    this->getRenderService()->makeCurrent();

    std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);
    m_volumeRenderer->set3DTexture(m_3DOgreTexture);

    const data::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
    {
        data::mt::locked_ptr lock(volumeTF);

        if(m_preIntegratedRendering)
        {
            m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
        }

        if(m_ambientOcclusion || m_colorBleeding || m_shadows)
        {
            if(m_ambientOcclusionSAT == nullptr)
            {
                m_ambientOcclusionSAT = std::make_shared<sight::viz::scene3d::vr::IllumAmbientOcclusionSAT>(
                    this->getID(),
                    m_sceneManager,
                    m_satSizeRatio,
                    (
                        m_ambientOcclusion
                        || m_colorBleeding),
                    m_shadows,
                    m_satShells,
                    m_satShellRadius,
                    m_satConeAngle,
                    m_satConeSamples
                );
            }

            this->updateVolumeIllumination();
        }

        m_volumeRenderer->imageUpdate(image.get_shared(), volumeTF);
    }

    // Create widgets on image update to take the image's size into account.
    this->createWidget();
    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
    else
    {
        this->getLayer()->computeCameraParameters();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSampling(int _nbSamples)
{
    this->getRenderService()->makeCurrent();

    SIGHT_ASSERT("Sampling rate must fit in a 16 bit uint.", _nbSamples < 65536 && _nbSamples >= 0);
    m_nbSamples = static_cast<std::uint16_t>(_nbSamples);

    m_volumeRenderer->setSampling(m_nbSamples);
    m_gpuVolumeTF->setSampleDistance(m_volumeRenderer->getSamplingRate());

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->updateVolumeIllumination();
    }

    if(m_preIntegratedRendering)
    {
        const data::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
        const data::mt::locked_ptr lock(volumeTF);
        m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateOpacityCorrection(int _opacityCorrection)
{
    m_volumeRenderer->setOpacityCorrection(_opacityCorrection);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateAOFactor(double _aoFactor)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_aoFactor = _aoFactor;

        m_volumeRenderer->setAOFactor(m_aoFactor);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateColorBleedingFactor(double _colorBleedingFactor)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_colorBleedingFactor = _colorBleedingFactor;

        m_volumeRenderer->setColorBleedingFactor(m_colorBleedingFactor);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatSizeRatio(int _sizeRatio)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        float scaleCoef(.25f);
        m_satSizeRatio = static_cast<float>(_sizeRatio) * scaleCoef;
        m_ambientOcclusionSAT->updateSatFromRatio(m_satSizeRatio);

        this->updateVolumeIllumination();

        if(m_preIntegratedRendering)
        {
            const auto image = m_image.lock();

            const data::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
            const data::mt::locked_ptr lock(volumeTF);
            m_volumeRenderer->imageUpdate(image.get_shared(), volumeTF);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatShellsNumber(int _shellsNumber)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satShells = _shellsNumber;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setNbShells(m_satShells);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatShellRadius(int _shellRadius)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satShellRadius = _shellRadius;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setShellRadius(m_satShellRadius);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatConeAngle(int _coneAngle)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satConeAngle = static_cast<float>(_coneAngle) / 100;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setConeAngle(m_satConeAngle);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatConeSamples(int _nbConeSamples)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->getRenderService()->makeCurrent();

        m_satConeSamples = _nbConeSamples;

        auto illumVolume = m_volumeRenderer->getIllumVolume();
        illumVolume->setSamplesAlongCone(m_satConeSamples);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::togglePreintegration(bool _preintegration)
{
    this->getRenderService()->makeCurrent();

    m_preIntegratedRendering = _preintegration;

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    if(m_preIntegratedRendering)
    {
        const auto image = m_image.lock();

        const data::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
        const data::mt::locked_ptr lock(volumeTF);

        m_volumeRenderer->imageUpdate(image.get_shared(), volumeTF);
        m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleAmbientOcclusion(bool _ambientOcclusion)
{
    m_ambientOcclusion = _ambientOcclusion;
    this->toggleVREffect(module::viz::scene3d::adaptor::SVolumeRender::VR_AMBIENT_OCCLUSION);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleColorBleeding(bool _colorBleeding)
{
    m_colorBleeding = _colorBleeding;
    this->toggleVREffect(module::viz::scene3d::adaptor::SVolumeRender::VR_COLOR_BLEEDING);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleShadows(bool _shadows)
{
    m_shadows = _shadows;
    this->toggleVREffect(module::viz::scene3d::adaptor::SVolumeRender::VR_SHADOWS);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleWidgets(bool _visible)
{
    m_widgetVisibilty = _visible;

    if(m_widget)
    {
        m_widget->setBoxVisibility(m_widgetVisibilty && m_volumeRenderer->isVisible());

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setFocalDistance(int _focalDistance)
{
    if(this->getRenderService()->getLayer(m_layerID)->getStereoMode()
       != sight::viz::scene3d::compositor::Core::StereoModeType::NONE)
    {
        m_volumeRenderer->setFocalLength(static_cast<float>(_focalDistance) / 100);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setBoolParameter(bool _val, std::string _key)
{
    this->getRenderService()->makeCurrent();
    std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);

    if(_key == "preIntegration")
    {
        this->togglePreintegration(_val);
    }
    else if(_key == "ambientOcclusion")
    {
        this->toggleAmbientOcclusion(_val);
    }
    else if(_key == "colorBleeding")
    {
        this->toggleColorBleeding(_val);
    }
    else if(_key == "shadows")
    {
        this->toggleShadows(_val);
    }
    else if(_key == "widgets")
    {
        this->toggleWidgets(_val);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setIntParameter(int _val, std::string _key)
{
    this->getRenderService()->makeCurrent();
    std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);

    if(_key == "sampling")
    {
        this->updateSampling(_val);
    }
    else if(_key == "opacityCorrection")
    {
        this->updateOpacityCorrection(_val);
    }
    else if(_key == "satSizeRatio")
    {
        this->updateSatSizeRatio(_val);
    }
    else if(_key == "satShellsNumber")
    {
        this->updateSatShellsNumber(_val);
    }
    else if(_key == "satShellRadius")
    {
        this->updateSatShellRadius(_val);
    }
    else if(_key == "satConeAngle")
    {
        this->updateSatConeAngle(_val);
    }
    else if(_key == "satConeSamples")
    {
        this->updateSatConeSamples(_val);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setDoubleParameter(double _val, std::string _key)
{
    this->getRenderService()->makeCurrent();
    std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);

    if(_key == "aoFactor")
    {
        this->updateAOFactor(_val);
    }
    else if(_key == "colorBleedingFactor")
    {
        this->updateColorBleedingFactor(_val);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::createWidget()
{
    auto clippingMxUpdate = std::bind(&SVolumeRender::updateClippingTM3D, this);

    ::Ogre::Matrix4 ogreClippingMx = ::Ogre::Matrix4::IDENTITY;

    const auto clippingMatrix = m_clippingMatrix.lock();
    if(clippingMatrix)
    {
        ogreClippingMx = sight::viz::scene3d::Utils::convertTM3DToOgreMx(clippingMatrix.get_shared());
    }

    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    this->destroyWidget(); // Destroys the old widgets if they were created.
    m_widget = std::make_shared<sight::viz::scene3d::interactor::ClippingBoxInteractor>(
        layer,
        m_layerOrderDependant,
        this->getID(),
        m_volumeSceneNode,
        ogreClippingMx,
        clippingMxUpdate,
        "BasicAmbient",
        "BasicPhong"
    );

    layer->addInteractor(m_widget, m_priority);

    m_volumeRenderer->clipImage(m_widget->getClippingBox());

    m_widget->setBoxVisibility(m_widgetVisibilty && m_volumeRenderer->isVisible());
}

//-----------------------------------------------------------------------------

void SVolumeRender::destroyWidget()
{
    if(m_widget)
    {
        sight::viz::scene3d::Layer::sptr layer = this->getLayer();
        layer->removeInteractor(m_widget);
        m_widget.reset();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateVolumeIllumination()
{
    this->getRenderService()->makeCurrent();

    m_ambientOcclusionSAT->SATUpdate(m_3DOgreTexture, m_gpuVolumeTF, m_volumeRenderer->getSamplingRate());

    m_volumeRenderer->setAmbientOcclusionSAT(m_ambientOcclusionSAT);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleVREffect(module::viz::scene3d::adaptor::SVolumeRender::VREffectType _vrEffect)
{
    this->getRenderService()->makeCurrent();

    bool isValid = false;
    {
        const auto image = m_image.lock();
        isValid = data::helper::MedicalImage::checkImageValidity(image.get_shared());
    }

    // Volume illumination is only implemented for raycasting rendering
    if(isValid)
    {
        if((m_ambientOcclusion || m_colorBleeding || m_shadows) && !m_ambientOcclusionSAT)
        {
            m_ambientOcclusionSAT = ::std::make_shared<sight::viz::scene3d::vr::IllumAmbientOcclusionSAT>(
                this->getID(),
                m_sceneManager,
                m_satSizeRatio,
                (
                    m_ambientOcclusion
                    || m_colorBleeding),
                m_shadows,
                m_satShells,
                m_satShellRadius
            );
            this->updateVolumeIllumination();
        }
        else if(m_ambientOcclusionSAT)
        {
            switch(_vrEffect)
            {
                case module::viz::scene3d::adaptor::SVolumeRender::VR_AMBIENT_OCCLUSION:
                case module::viz::scene3d::adaptor::SVolumeRender::VR_COLOR_BLEEDING:
                    m_ambientOcclusionSAT->setAO(m_ambientOcclusion || m_colorBleeding);
                    break;

                case module::viz::scene3d::adaptor::SVolumeRender::VR_SHADOWS:
                    m_ambientOcclusionSAT->setShadows(m_shadows);
                    break;
            }
        }

        switch(_vrEffect)
        {
            case module::viz::scene3d::adaptor::SVolumeRender::VR_AMBIENT_OCCLUSION:
                m_volumeRenderer->setAmbientOcclusion(m_ambientOcclusion);
                break;

            case module::viz::scene3d::adaptor::SVolumeRender::VR_COLOR_BLEEDING:
                m_volumeRenderer->setColorBleeding(m_colorBleeding);
                break;

            case module::viz::scene3d::adaptor::SVolumeRender::VR_SHADOWS:
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
            const auto image = m_image.lock();

            const data::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
            const data::mt::locked_ptr lock(volumeTF);
            m_volumeRenderer->imageUpdate(image.get_shared(), volumeTF);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateClippingBox()
{
    if(m_widget)
    {
        bool matrixSet = false;
        ::Ogre::Matrix4 clippingMx;
        {
            const auto clippingMatrix = m_clippingMatrix.lock();
            if(clippingMatrix)
            {
                clippingMx = sight::viz::scene3d::Utils::convertTM3DToOgreMx(clippingMatrix.get_shared());
                matrixSet  = true;
            }
        }

        if(matrixSet)
        {
            this->getRenderService()->makeCurrent();

            // updateFromTransform is called outside of the lock of the InOut data to prevent a deadlock
            m_widget->updateFromTransform(clippingMx);
        }
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateClippingTM3D()
{
    auto clippingMatrix = m_clippingMatrix.lock();
    if(clippingMatrix)
    {
        sight::viz::scene3d::Utils::copyOgreMxToTM3D(m_widget->getClippingTransform(), clippingMatrix.get_shared());

        const auto sig =
            clippingMatrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);

        core::com::Connection::Blocker blocker(sig->getConnection(this->slot(s_UPDATE_CLIPPING_BOX_SLOT)));

        sig->asyncEmit();
    }

    std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);
    m_volumeRenderer->clipImage(m_widget->getClippingBox());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setVisible(bool _visible)
{
    if(m_volumeSceneNode)
    {
        m_volumeSceneNode->setVisible(_visible);

        if(m_widget)
        {
            m_widget->setBoxVisibility(_visible && m_widgetVisibilty);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

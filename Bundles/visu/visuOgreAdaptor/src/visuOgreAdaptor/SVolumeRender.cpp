/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_IMAGE_SLOT                    = "newImage";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_MASK_SLOT                     = "newMask";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_IMAGE_SLOT                 = "updateImage";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAMPLING_SLOT              = "updateSampling";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_AO_FACTOR_SLOT             = "updateAOFactor";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_COLOR_BLEEDING_FACTOR_SLOT = "updateColorBleedingFactor";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SIZE_RATIO_SLOT        = "updateSatSizeRatio";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SHELLS_SLOT            = "updateSatShellsNumber";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SHELL_RADIUS_SLOT      = "updateSatShellRadius";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_CONE_ANGLE_SLOT        = "updateSatConeAngle";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_CONE_SAMPLES_SLOT      = "updateSatConeSamplesNumber";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_PREINTEGRATION_SLOT        = "togglePreintegration";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_AMBIENT_OCCLUSION_SLOT     = "toggleAmbientOcclusion";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_COLOR_BLEEDING_SLOT        = "toggleColorBleeding";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_SHADOWS_SLOT               = "toggleShadows";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_WIDGETS_SLOT               = "toggleWidgets";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_RESIZE_VIEWPORT_SLOT              = "resizeViewport";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_STEREO_MODE_SLOT              = "setStereoMode";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_BOOL_PARAMETER_SLOT           = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_INT_PARAMETER_SLOT            = "setIntParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_DOUBLE_PARAMETER_SLOT         = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_ENUM_PARAMETER_SLOT           = "setEnumParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_COLOR_PARAMETER_SLOT          = "setColorParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_VISIBILITY_SLOT            = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_CLIPPING_BOX_SLOT          = "updateClippingBox";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT           = "image";
static const ::fwServices::IService::KeyType s_VOLUME_TF_INOUT       = "tf";
static const ::fwServices::IService::KeyType s_CSG_TF_INOUT          = "CSGTF";
static const ::fwServices::IService::KeyType s_CLIPPING_MATRIX_INOUT = "clippingMatrix";
static const ::fwServices::IService::KeyType s_MASK_INOUT            = "mask";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() noexcept :
    m_helperVolumeTF(std::bind(&SVolumeRender::updateVolumeTF, this)),
    m_helperCSGTF(std::bind(&SVolumeRender::updateCSGTF, this))
{
    /// Handle connections between the layer and the volume renderer.
    ::fwCom::helper::SigSlotConnection m_volumeConnection;
    newSlot(s_NEW_IMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_NEW_MASK_SLOT, &SVolumeRender::newMask, this);
    newSlot(s_UPDATE_IMAGE_SLOT, &SVolumeRender::updateImage, this);
    newSlot(s_UPDATE_SAMPLING_SLOT, &SVolumeRender::updateSampling, this);
    newSlot(s_UPDATE_AO_FACTOR_SLOT, &SVolumeRender::updateAOFactor, this);
    newSlot(s_UPDATE_COLOR_BLEEDING_FACTOR_SLOT, &SVolumeRender::updateColorBleedingFactor, this);
    newSlot(s_UPDATE_SAT_SIZE_RATIO_SLOT, &SVolumeRender::updateSatSizeRatio, this);
    newSlot(s_UPDATE_SAT_SHELLS_SLOT, &SVolumeRender::updateSatShellsNumber, this);
    newSlot(s_UPDATE_SAT_SHELL_RADIUS_SLOT, &SVolumeRender::updateSatShellRadius, this);
    newSlot(s_UPDATE_SAT_CONE_ANGLE_SLOT, &SVolumeRender::updateSatConeAngle, this);
    newSlot(s_UPDATE_SAT_CONE_SAMPLES_SLOT, &SVolumeRender::updateSatConeSamples, this);
    newSlot(s_TOGGLE_PREINTEGRATION_SLOT, &SVolumeRender::togglePreintegration, this);
    newSlot(s_TOGGLE_AMBIENT_OCCLUSION_SLOT, &SVolumeRender::toggleAmbientOcclusion, this);
    newSlot(s_TOGGLE_COLOR_BLEEDING_SLOT, &SVolumeRender::toggleColorBleeding, this);
    newSlot(s_TOGGLE_SHADOWS_SLOT, &SVolumeRender::toggleShadows, this);
    newSlot(s_TOGGLE_WIDGETS_SLOT, &SVolumeRender::toggleWidgets, this);
    newSlot(s_RESIZE_VIEWPORT_SLOT, &SVolumeRender::resizeViewport, this);
    newSlot(s_SET_STEREO_MODE_SLOT, &SVolumeRender::setStereoMode, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SVolumeRender::setBoolParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SVolumeRender::setIntParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SVolumeRender::setDoubleParameter, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SVolumeRender::setEnumParameter, this);
    newSlot(s_SET_COLOR_PARAMETER_SLOT, &SVolumeRender::setColorParameter, this);
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
    m_IDVRMethod             = config.get<std::string>("idvrMethod", "None");
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

    const std::string transformId = config.get<std::string>(::visuOgreAdaptor::STransform::s_CONFIG_TRANSFORM,
                                                            this->getID() + "_transform");
    this->setTransformId(transformId);
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

//DONE
void SVolumeRender::updateCSGTF()
{
    this->getRenderService()->makeCurrent();

    ::fwData::TransferFunction::sptr tf = m_helperCSGTF.getTransferFunction();
    {
        ::fwData::mt::ObjectWriteLock tfLock(tf);

        m_gpuCSGTF->updateTexture(tf);

//        TODO
//        if(m_preIntegratedRendering)
//        {
//            m_preIntegrationTable.tfUpdate(tf, m_volumeRenderer->getSamplingRate());
//        }

//        m_volumeRenderer->tfUpdate(tf);

//        if(m_ambientOcclusion || m_colorBleeding || m_shadows)
//        {
//            this->updateVolumeIllumination();
//        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVolumeRender::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;

    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT );
    connections.push( s_MASK_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEW_MASK_SLOT );
    connections.push( s_MASK_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEW_MASK_SLOT );
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
    //DONE
    m_gpuCSGTF = std::make_shared< ::fwRenderOgre::TransferFunction>();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT +"' is missing.", image);

    ::fwData::TransferFunction::sptr volumeTF = this->getInOut< ::fwData::TransferFunction>(s_VOLUME_TF_INOUT);
    m_helperVolumeTF.setOrCreateTF(volumeTF, image);

    //DONE
    ::fwData::TransferFunction::sptr CSGTF = this->getInOut< ::fwData::TransferFunction>(s_CSG_TF_INOUT);
    m_helperCSGTF.setOrCreateTF(CSGTF, image);

    m_sceneManager = this->getSceneManager();

    ::Ogre::SceneNode* rootSceneNode = m_sceneManager->getRootSceneNode();
    ::Ogre::SceneNode* transformNode =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);
    if (transformNode == nullptr)
    {
        transformNode = rootSceneNode->createChildSceneNode(this->getTransformId());
    }
    m_volumeSceneNode = transformNode->createChildSceneNode(this->getID() + "_transform_origin");
    m_volumeSceneNode->setVisible(true, false);

    m_camera = this->getLayer()->getDefaultCamera();

    // Create textures
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    m_maskTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_MaskTexture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    m_gpuVolumeTF->createTexture(this->getID() + "_VolumeGpuTF");
    //DONE
    m_gpuCSGTF->createTexture(this->getID() + "_CSGGpuTF");

    m_preIntegrationTable.createTexture(this->getID());

    ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);

    m_volumeRenderer = new ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer(this->getID(),
                                                                              layer,
                                                                              m_volumeSceneNode,
                                                                              m_3DOgreTexture,
                                                                              m_maskTexture,
                                                                              m_gpuVolumeTF,
                                                                              m_gpuCSGTF,
                                                                              m_preIntegrationTable,
                                                                              m_ambientOcclusion,
                                                                              m_colorBleeding);

    // Initially focus on the image center.
    this->setFocalDistance(50);

    m_volumeRenderer->setIDVRMethod(m_IDVRMethod);

    if(m_IDVRMethod != "None")
    {
        this->newMask();
    }

    m_gpuVolumeTF->setSampleDistance(m_volumeRenderer->getSamplingRate());
    //DONE
    m_gpuCSGTF->setSampleDistance(m_volumeRenderer->getSamplingRate());

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_RESIZE_VIEWPORT_SLOT);
    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_STEREO_MODE_CHANGED_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_SET_STEREO_MODE_SLOT);

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
    //DONE
    m_helperCSGTF.removeTFConnections();

    m_volumeConnection.disconnect();
    delete m_volumeRenderer;
    m_volumeRenderer = nullptr;

    this->getSceneManager()->destroySceneNode(m_volumeSceneNode);

    auto transformNode = m_sceneManager->getRootSceneNode()->getChild(this->getTransformId());
    m_sceneManager->getRootSceneNode()->removeChild(transformNode);
    this->getSceneManager()->destroySceneNode(static_cast< ::Ogre::SceneNode*>(transformNode));

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture->getHandle());
    m_3DOgreTexture.reset();

    ::Ogre::TextureManager::getSingleton().remove(m_maskTexture->getHandle());
    m_maskTexture.reset();

    m_gpuVolumeTF.reset();
    //DONE
    m_gpuCSGTF.reset();

    m_preIntegrationTable.removeTexture();

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
    //DONE
    else if (key == s_CSG_TF_INOUT)
    {
        ::fwData::TransferFunction::sptr CSGTF = this->getInOut< ::fwData::TransferFunction>(s_CSG_TF_INOUT);
        m_helperCSGTF.setOrCreateTF(CSGTF, image);

        this->updateCSGTF();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::newImage()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    this->updateImage();

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

    // Retrieves or creates the slice index fields
    this->setImageSpacing();

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    m_helperVolumeTF.createTransferFunction(image);

    ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
    {
        ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
        m_gpuVolumeTF->updateTexture(volumeTF);

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

    //DONE
    m_helperCSGTF.createTransferFunction(image);

    ::fwData::TransferFunction::sptr CSGTF = m_helperCSGTF.getTransferFunction();
    {
        ::fwData::mt::ObjectWriteLock tfLock(CSGTF);
        m_gpuCSGTF->updateTexture(CSGTF);

//        TODO
//        if(m_preIntegratedRendering)
//        {
//            m_preIntegrationTable.tfUpdate(CSGTF, m_volumeRenderer->getSamplingRate());
//        }

//        if(m_ambientOcclusion || m_colorBleeding || m_shadows)
//        {
//            if(m_illum == nullptr)
//            {
//                m_illum = std::make_shared< ::fwRenderOgre::vr::SATVolumeIllumination>(this->getID(), m_sceneManager,
//                                                                                       m_satSizeRatio,
//                                                                                       (m_ambientOcclusion ||
//                                                                                        m_colorBleeding), m_shadows,
//                                                                                       m_satShells, m_satShellRadius,
//                                                                                       m_satConeAngle,
//                                                                                       m_satConeSamples);
//            }
//            this->updateVolumeIllumination();
//        }
//        m_volumeRenderer->imageUpdate(image, CSGTF);
    }

    // Create widgets on image update to take the image's size into account.
    this->createWidget();

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::newMask()
{
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr mask = this->getInOut< ::fwData::Image>(s_MASK_INOUT);
    SLM_ASSERT("No 'mask' inout.", mask);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(mask))
    {
        ::fwRenderOgre::Utils::convertImageForNegato(m_maskTexture.get(), mask);
    }

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
    //DONE
    m_gpuCSGTF->setSampleDistance(m_volumeRenderer->getSamplingRate());

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->updateVolumeIllumination();
    }

    if(m_preIntegratedRendering)
    {
        {
            ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
            ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
            m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
        }
//        TODO
//        {
//            ::fwData::TransferFunction::sptr CSGTF = m_helperCSGTF.getTransferFunction();
//            ::fwData::mt::ObjectWriteLock tfLock(CSGTF);
//            m_preIntegrationTable.tfUpdate(CSGTF, m_volumeRenderer->getSamplingRate());
//        }
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

            {
                ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
                ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
                m_volumeRenderer->imageUpdate(image, volumeTF);
            }
//            TODO
//            {
//                ::fwData::TransferFunction::sptr CSGTF = m_helperCSGTF.getTransferFunction();
//                ::fwData::mt::ObjectWriteLock tfLock(CSGTF);
//                m_volumeRenderer->imageUpdate(image, CSGTF);
//            }
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

        {
            ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
            ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
            m_volumeRenderer->imageUpdate(image, volumeTF);
            m_preIntegrationTable.tfUpdate(volumeTF, m_volumeRenderer->getSamplingRate());
        }

//        TODO
//        {
//            ::fwData::TransferFunction::sptr CSGTF = m_helperCSGTF.getTransferFunction();
//            ::fwData::mt::ObjectWriteLock tfLock(CSGTF);
//            m_volumeRenderer->imageUpdate(image, tfCSGTF
//            m_preIntegrationTable.tfUpdate(CSGTF, m_volumeRenderer->getSamplingRate());
//        }

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

void SVolumeRender::resizeViewport(int w, int h)
{
    this->getRenderService()->makeCurrent();

    if(m_volumeRenderer)
    {
        m_volumeRenderer->resizeViewport(w, h);
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setFocalDistance(int focalDistance)
{
    if(this->getRenderService()->getLayer(m_layerID)->getStereoMode() != ::fwRenderOgre::Layer::StereoModeType::NONE)
    {
        m_volumeRenderer->setFocalLength(static_cast<float>(focalDistance) / 100);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setStereoMode(::fwRenderOgre::Layer::StereoModeType)
{
    this->getRenderService()->makeCurrent();

    this->stopping();
    this->starting();
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
    else if(key == "idvrCSG")
    {
        m_volumeRenderer->toggleIDVRCountersinkGeometry(val);
    }
    else if(key == "idvrCSGBorder")
    {
        m_volumeRenderer->toggleIDVRCSGBorder(val);
    }
    else if(key == "idvrCSGGrayScale")
    {
        m_volumeRenderer->toggleIDVRCSGGrayScale(val);
    }
    else if(key == "idvrCSGModulation")
    {
        m_volumeRenderer->toggleIDVRCSGModulation(val);
    }
    else if(key == "idvrCSGOpacityDecrease")
    {
        m_volumeRenderer->toggleIDVRCSGOpacityDecrease(val);
    }
    else if(key == "idvrCSGDepthLines")
    {
        m_volumeRenderer->toggleIDVRDepthLines(val);
    }
    else if(key == "idvrCSGDisableContext")
    {
        m_volumeRenderer->toggleIDVRCSGDisableContext(val);
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
    else if(key == "idvrDepthLinesSpacing")
    {
        m_volumeRenderer->setIDVRDepthLinesSpacing(val);
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
    else if(key == "idvrCSGAngle")
    {
        m_volumeRenderer->setIDVRCountersinkAngle(val);
    }
    else if(key == "idvrCSGBlurWeight")
    {
        m_volumeRenderer->setIDVRCSGBlurWeight(val);
    }
    else if(key == "idvrCSGBorderThickness")
    {
        m_volumeRenderer->setIDVRCSGBorderThickness(val);
    }
    else if(key == "idvrCSGModulationFactor")
    {
        m_volumeRenderer->setIDVRCSGModulationFactor(val);
    }
    else if(key == "idvrCSGOpacityDecreaseFactor")
    {
        m_volumeRenderer->setIDVRCSGOpacityDecreaseFactor(val);
    }
    else if(key == "idvrVPImCAlphaCorrection")
    {
        m_volumeRenderer->setIDVRVPImCAlphaCorrection(val);
    }
    else if(key == "idvrAImCAlphaCorrection")
    {
        m_volumeRenderer->setIDVRAImCAlphaCorrection(val);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setEnumParameter(std::string val, std::string key)
{
    this->getRenderService()->makeCurrent();

    if(key == "idvrMethod")
    {
        m_volumeRenderer->setIDVRMethod(val);
        this->requestRender();
    }
    else if(key == "idvrCSGModulationMethod")
    {
        if(val == "Brightness")
        {
            m_volumeRenderer->setIDVRCSGModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::COLOR1);
        }
        else if(val == "Saturation")
        {
            m_volumeRenderer->setIDVRCSGModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::COLOR2);
        }
        else if(val == "SaturationBrightness")
        {
            m_volumeRenderer->setIDVRCSGModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::COLOR3);
        }
    }
    else if(key == "idvrCSGGrayScaleMethod")
    {
        if(val == "Average_grayscale")
        {
            m_volumeRenderer->setIDVRCSGGrayScaleMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGGrayScaleMethod::AVERAGE_GRAYSCALE);
        }
        else if(val == "Lightness_grayscale")
        {
            m_volumeRenderer->setIDVRCSGGrayScaleMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGGrayScaleMethod::LIGHTNESS_GRAYSCALE);
        }
        else if(val == "Luminosity_grayscale")
        {
            m_volumeRenderer->setIDVRCSGGrayScaleMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGGrayScaleMethod::LUMINOSITY_GRAYSCALE);
        }
    }
    else if(key == "idvrDepthLinesSpacing")
    {
        const bool toggle = val != "Off";
        m_volumeRenderer->toggleIDVRDepthLines(toggle);

        if(toggle)
        {
            const int spacing = std::stoi(val);
            m_volumeRenderer->setIDVRDepthLinesSpacing(spacing);
        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setColorParameter(std::array<std::uint8_t, 4> color, std::string key)
{
    this->getRenderService()->makeCurrent();

    if(key == "idvrCSGBorderColor")
    {
        m_volumeRenderer->setIDVRCSGBorderColor(color);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setImageSpacing()
{
    auto img = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    const bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(img);

    if(isValid)
    {
        const auto& spacing = img->getSpacing();

        SLM_ASSERT("Image must be 3D.", spacing.size() == 3);

        m_volumeRenderer->setImageSpacing(::Ogre::Vector3(static_cast<float>(spacing[0]),
                                                          static_cast<float>(spacing[1]),
                                                          static_cast<float>(spacing[2])));
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
        else
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
            {
                ::fwData::TransferFunction::sptr volumeTF = m_helperVolumeTF.getTransferFunction();
                ::fwData::mt::ObjectWriteLock tfLock(volumeTF);
                m_volumeRenderer->imageUpdate(image, volumeTF);
            }
//            TODO
//            {
//                ::fwData::TransferFunction::sptr CSGTF = m_helperCSGTF.getTransferFunction();
//                ::fwData::mt::ObjectWriteLock tfLock(CSGTF);
//                m_volumeRenderer->imageUpdate(image, CSGTF);
//            }
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

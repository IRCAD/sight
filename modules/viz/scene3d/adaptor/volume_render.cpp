/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/volume_render.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <viz/scene3d/helper/Scene.hpp>
#include <viz/scene3d/helper/Shading.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>

#include <memory>

//-----------------------------------------------------------------------------

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

volume_render::volume_render() noexcept
{
    // Handle connections between the layer and the volume renderer.
    new_slot(NEW_IMAGE_SLOT, &volume_render::newImage, this);
    new_slot(BUFFER_IMAGE_SLOT, &volume_render::bufferImage, this);
    new_slot(UPDATE_IMAGE_SLOT, &volume_render::updateImage, this);
    new_slot(TOGGLE_WIDGETS_SLOT, &volume_render::toggleWidgets, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &volume_render::setBoolParameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &volume_render::setIntParameter, this);
    new_slot(SET_DOUBLE_PARAMETER_SLOT, &volume_render::setDoubleParameter, this);
    new_slot(UPDATE_CLIPPING_BOX_SLOT, &volume_render::updateClippingBox, this);
    new_slot(UPDATE_MASK_SLOT, &volume_render::updateMask, this);
    new_slot(UPDATE_TF_SLOT, &volume_render::updateVolumeTF, this);
}

//-----------------------------------------------------------------------------

volume_render::~volume_render() noexcept =
    default;

//-----------------------------------------------------------------------------

service::connections_t volume_render::auto_connections() const
{
    return {
        {objects::IMAGE_IN, data::image::MODIFIED_SIG, NEW_IMAGE_SLOT},
        {objects::IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, BUFFER_IMAGE_SLOT},
        {objects::MASK_IN, data::image::MODIFIED_SIG, NEW_IMAGE_SLOT},
        {objects::MASK_IN, data::image::BUFFER_MODIFIED_SIG, UPDATE_MASK_SLOT},
        {objects::CLIPPING_MATRIX_INOUT, data::matrix4::MODIFIED_SIG, UPDATE_CLIPPING_BOX_SLOT},
        {objects::VOLUME_TF_IN, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {objects::VOLUME_TF_IN, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {objects::VOLUME_TF_IN, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT},
    };
}

//-----------------------------------------------------------------------------

void volume_render::configuring(const config_t& _config)
{
    this->configureParams();

    //TODO: When MSVC finally supports designated initialization, use it.
    {
        //Global parameters
        {
            m_config.camera_autoreset = _config.get<bool>(config::AUTORESET_CAMERA, true);
            m_config.preintegration   = _config.get<bool>(config::PREINTEGRATION, false);
            m_config.dynamic          = _config.get<bool>(config::DYNAMIC, false);
            m_config.visible          = _config.get<bool>(config::WIDGETS, true);
            m_config.priority         = _config.get<int>(config::PRIORITY, 2);
            m_config.order_dependent  = _config.get<bool>(config::LAYER_ORDER_DEPENDANT, true);
            m_config.samples          = _config.get<std::uint16_t>(config::SAMPLES, 512);
        }

        //SAT
        {
            m_config.sat.size_ratio = _config.get<float>(config::SAT_SIZE_RATIO, 0.25F);
            m_config.sat.shells     = static_cast<unsigned>(_config.get<int>(config::SAT_SHELLS, 4));
            m_config.sat.radius     = static_cast<unsigned>(_config.get<int>(config::SAT_SHELL_RADIUS, 4));
            m_config.sat.angle      = _config.get<float>(config::SAT_CONE_ANGLE, 0.1F);
            m_config.sat.samples    = static_cast<unsigned>(_config.get<int>(config::SAT_CONE_SAMPLES, 50));
        }

        //Shadows
        {
            m_config.shadows.soft_shadows = _config.get<bool>(config::SHADOWS, false);

            //AO
            {
                m_config.shadows.ao.enabled = _config.get<bool>(config::AO, false),
                m_config.shadows.ao.factor  = static_cast<float>(_config.get<double>(config::AO_FACTOR, 1.));
            }

            //Colour bleeding
            {
                const double colorBleedingFactor = _config.get<double>(config::COLOR_BLEEDING_FACTOR, 1.);

                m_config.shadows.colour_bleeding.enabled = _config.get<bool>(config::COLOR_BLEEDING, false);
                m_config.shadows.colour_bleeding.r       = static_cast<float>(colorBleedingFactor);
                m_config.shadows.colour_bleeding.g       = static_cast<float>(colorBleedingFactor);
                m_config.shadows.colour_bleeding.b       = static_cast<float>(colorBleedingFactor);
            }
        }
    }

    this->setTransformId(
        _config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );
}

//-----------------------------------------------------------------------------

void volume_render::starting()
{
    this->initialize();

    auto renderService = this->getRenderService();
    renderService->makeCurrent();

    //Scene (node, manager)
    {
        m_sceneManager = this->getSceneManager();

        Ogre::SceneNode* const rootSceneNode = m_sceneManager->getRootSceneNode();
        Ogre::SceneNode* const transformNode = this->getOrCreateTransformNode(rootSceneNode);
        m_volumeSceneNode = transformNode->createChildSceneNode(this->get_id() + "_transform_origin");
    }

    //Renderer
    {
        sight::viz::scene3d::Layer::sptr layer = renderService->getLayer(m_layerID);

        const auto image = m_image.lock();
        const auto mask  = m_mask.lock();
        const auto tf    = m_tf.lock();
        m_volumeRenderer = std::make_unique<sight::viz::scene3d::vr::ray_tracing_volume_renderer>(
            this->get_id(),
            layer,
            m_volumeSceneNode,
            image.get_shared(),
            mask.get_shared(),
            tf.get_shared(),
            m_config.samples,
            m_config.dynamic,
            m_config.preintegration,
            m_config.shadows,
            m_config.sat
        );
        m_volumeRenderer->update(tf.get_shared());
    }

    m_volumeSceneNode->setVisible(m_isVisible);

    // Initially focus on the image center.
    this->setFocalDistance(50);

    bool isValid = false;
    //image
    {
        const auto image = m_image.lock();
        isValid = data::helper::MedicalImage::checkImageValidity(image.get_shared());
    }

    if(isValid)
    {
        this->newImage();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::updating()
{
}

//-----------------------------------------------------------------------------

void volume_render::stopping()
{
    this->getRenderService()->makeCurrent();

    // First wait on all pending buffering tasks and destroy the worker.
    m_bufferingWorker.reset();
    m_volumeRenderer.reset();

    this->getSceneManager()->destroySceneNode(m_volumeSceneNode);

    auto* const transformNode = this->getTransformNode();

    if(transformNode != nullptr)
    {
        m_sceneManager->getRootSceneNode()->removeChild(transformNode);
        this->getSceneManager()->destroySceneNode(static_cast<Ogre::SceneNode*>(transformNode));
    }

    this->destroyWidget();
}

//-----------------------------------------------------------------------------

void volume_render::updateVolumeTF()
{
    this->getRenderService()->makeCurrent();
    std::lock_guard swapLock(m_mutex);

    {
        const auto tf = m_tf.lock();
        m_volumeRenderer->updateVolumeTF(tf.get_shared());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::newImage()
{
    auto renderService = this->getRenderService();

    {
        const auto image = m_image.lock();
        const auto mask  = m_mask.lock();

        // Ignore this update to avoid flickering when loading a new image
        // We will be signaled later when either the image or the mask will be updated
        if(image->size() != mask->size())
        {
            return;
        }
    }
    {
        if(m_config.dynamic)
        {
            // Destroy the worker to wait for all pending buffering tasks to be cleared.
            m_bufferingWorker.reset();

            auto* newWorker = renderService->getInteractorManager()->createGraphicsWorker();
            m_bufferingWorker = std::unique_ptr<sight::viz::scene3d::IGraphicsWorker>(newWorker);
        }

        renderService->makeCurrent();
        {
            const auto image = m_image.lock();
            m_volumeRenderer->loadImage();
        }
        this->updateMask();
        this->updateVolumeTF();
    }

    this->updateImage();
}

//-----------------------------------------------------------------------------

void volume_render::bufferImage()
{
    if(m_config.dynamic)
    {
        auto bufferingFn =
            [this]()
            {
                const auto image = m_image.lock();

                m_volumeRenderer->loadImage();

                // Switch back to the main thread to compute the proxy geometry.
                // Ogre can't handle parallel rendering.
                this->slot(UPDATE_IMAGE_SLOT)->async_run();
            };

        m_bufferingWorker->pushTask(bufferingFn);
    }
    else
    {
        this->getRenderService()->makeCurrent();
        {
            const auto image = m_image.lock();
            m_volumeRenderer->loadImage();
        }
        this->updateImage();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateImage()
{
    const auto image = m_image.lock();

    this->getRenderService()->makeCurrent();

    {
        const auto volumeTF = m_tf.lock();
        m_volumeRenderer->updateImage(image.get_shared(), volumeTF.get_shared());
    }

    // Create widgets on image update to take the image's size into account.
    this->createWidget();

    if(m_config.camera_autoreset)
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

void volume_render::updateMask()
{
    const auto mask = m_mask.lock();

    this->getRenderService()->makeCurrent();

    m_volumeRenderer->updateMask(mask.get_shared());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::updateSampling(unsigned _nbSamples)
{
    this->getRenderService()->makeCurrent();

    SIGHT_ASSERT("Sampling rate must fit in a 16 bit uint.", _nbSamples < 65536);

    const auto tf = m_tf.lock();
    m_volumeRenderer->setSampling(static_cast<std::uint16_t>(_nbSamples), tf.get_shared());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::updateOpacityCorrection(unsigned _opacityCorrection)
{
    m_volumeRenderer->setOpacityCorrection(int(_opacityCorrection));
    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::updateAOFactor(float _aoFactor)
{
    if(m_volumeRenderer->setAOFactor(_aoFactor))
    {
        this->requestRender(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateColorBleedingFactor(float _colorBleedingFactor)
{
    if(m_volumeRenderer->setColorBleedingFactor(_colorBleedingFactor))
    {
        this->requestRender(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatSizeRatio(unsigned _sizeRatio)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATSizeRatio(_sizeRatio);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatShellsNumber(unsigned _shellsNumber)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATShellsNumber(_shellsNumber);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatShellRadius(unsigned _shellRadius)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATShellRadius(_shellRadius);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatConeAngle(float _coneAngle)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATConeAngle(_coneAngle);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatConeSamples(unsigned _nbConeSamples)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATConeSamples(_nbConeSamples);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::togglePreintegration(bool _preintegration)
{
    this->getRenderService()->makeCurrent();

    m_volumeRenderer->setPreIntegratedRendering(_preintegration);

    if(_preintegration)
    {
        const auto image    = m_image.lock();
        const auto volumeTF = m_tf.lock();

        m_volumeRenderer->updateImage(image.get_shared(), volumeTF.get_shared());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::toggleAmbientOcclusion(bool _ambientOcclusion)
{
    this->toggleVREffect(VREffectType::VR_AMBIENT_OCCLUSION, _ambientOcclusion);
}

//-----------------------------------------------------------------------------

void volume_render::toggleColorBleeding(bool _colorBleeding)
{
    this->toggleVREffect(VREffectType::VR_COLOR_BLEEDING, _colorBleeding);
}

//-----------------------------------------------------------------------------

void volume_render::toggleShadows(bool _shadows)
{
    this->toggleVREffect(VREffectType::VR_SHADOWS, _shadows);
}

//-----------------------------------------------------------------------------

void volume_render::toggleWidgets(bool _visible)
{
    m_config.visible = _visible;

    if(m_widget)
    {
        m_widget->setBoxVisibility(m_config.visible && m_volumeRenderer->isVisible());

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::setFocalDistance(int _focalDistance)
{
    if(this->getRenderService()->getLayer(m_layerID)->getStereoMode()
       != sight::viz::scene3d::compositor::core::StereoModeType::NONE)
    {
        m_volumeRenderer->setFocalLength(static_cast<float>(_focalDistance) / 100);
    }
}

//-----------------------------------------------------------------------------

void volume_render::setBoolParameter(bool _val, std::string _key)
{
    SIGHT_ASSERT(
        "Invalid slot key " + _key,
        _key == "preIntegration"
        || _key == "ambientOcclusion"
        || _key == "colorBleeding"
        || _key == "shadows"
        || _key == "widgets"
    );

    this->getRenderService()->makeCurrent();
    std::lock_guard swapLock(m_mutex);

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

void volume_render::setIntParameter(int _val, std::string _key)
{
    SIGHT_ASSERT(
        "Invalid slot key " + _key,
        _key == "sampling"
        || _key == "opacityCorrection"
        || _key == "satSizeRatio"
        || _key == "satShellsNumber"
        || _key == "satShellRadius"
        || _key == "satConeSamples"
    );

    SIGHT_ASSERT("Int parameter cannot be negative in this context.", _val >= 0);

    this->getRenderService()->makeCurrent();
    std::lock_guard<std::mutex> swapLock(m_mutex);

    const auto param = static_cast<unsigned>(_val);

    if(_key == "sampling")
    {
        this->updateSampling(param);
    }
    else if(_key == "opacityCorrection")
    {
        this->updateOpacityCorrection(param);
    }
    else if(_key == "satSizeRatio")
    {
        this->updateSatSizeRatio(param);
    }
    else if(_key == "satShellsNumber")
    {
        this->updateSatShellsNumber(param);
    }
    else if(_key == "satShellRadius")
    {
        this->updateSatShellRadius(param);
    }
    else if(_key == "satConeSamples")
    {
        this->updateSatConeSamples(param);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::setDoubleParameter(double _val, std::string _key)
{
    SIGHT_ASSERT(
        "Invalid slot key " + _key,
        _key == "colorBleedingFactor"
        || _key == "aoFactor"
        || _key == "satConeAngle"
    );

    this->getRenderService()->makeCurrent();
    std::lock_guard swapLock(m_mutex);

    const auto param = static_cast<float>(_val);

    if(_key == "colorBleedingFactor")
    {
        this->updateColorBleedingFactor(param);
    }
    else if(_key == "aoFactor")
    {
        this->updateAOFactor(param);
    }
    else if(_key == "satConeAngle")
    {
        this->updateSatConeAngle(param);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::createWidget()
{
    auto clippingMxUpdate = [this]{updateClippingTM3D();};

    Ogre::Matrix4 ogreClippingMx = Ogre::Matrix4::IDENTITY;

    const auto clippingMatrix = m_clippingMatrix.lock();
    if(clippingMatrix)
    {
        ogreClippingMx = sight::viz::scene3d::Utils::convertTM3DToOgreMx(clippingMatrix.get_shared());
    }

    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    this->destroyWidget(); // Destroys the old widgets if they were created.
    m_widget = std::make_shared<sight::viz::scene3d::interactor::clipping_box_interactor>(
        layer,
        m_config.order_dependent,
        this->get_id(),
        m_volumeSceneNode,
        ogreClippingMx,
        clippingMxUpdate,
        "BasicAmbient",
        "BasicPhong"
    );

    layer->addInteractor(m_widget, m_config.priority);

    m_volumeRenderer->clipImage(m_widget->get_clipping_box());

    m_widget->setBoxVisibility(m_config.visible && m_volumeRenderer->isVisible());
}

//-----------------------------------------------------------------------------

void volume_render::destroyWidget()
{
    if(m_widget)
    {
        sight::viz::scene3d::Layer::sptr layer = this->getLayer();
        layer->removeInteractor(m_widget);
        m_widget.reset();
    }
}

//-----------------------------------------------------------------------------

void volume_render::toggleVREffect(VREffectType _vrEffect, bool _enable)
{
    this->getRenderService()->makeCurrent();

    //First, check the image is valid (requires locking locally thus the lambda)
    bool isValid = false;
    {
        const auto image = m_image.lock();
        isValid = data::helper::MedicalImage::checkImageValidity(image.get_shared());
    }

    // Volume illumination is only implemented for raycasting rendering
    if(isValid)
    {
        //Renderer update
        {
            switch(_vrEffect)
            {
                case VREffectType::VR_AMBIENT_OCCLUSION:
                {
                    m_volumeRenderer->toggleAmbientOcclusion(_enable);
                    break;
                }

                case VREffectType::VR_COLOR_BLEEDING:
                {
                    m_volumeRenderer->toggleColorBleeding(_enable);
                    break;
                }

                case VREffectType::VR_SHADOWS:
                {
                    m_volumeRenderer->toggleShadows(_enable);
                    break;
                }
            }
        }

        if(m_volumeRenderer->preintegration())
        {
            const auto image    = m_image.lock();
            const auto volumeTF = m_tf.lock();

            m_volumeRenderer->updateImage(image.get_shared(), volumeTF.get_shared());
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateClippingBox()
{
    if(m_widget)
    {
        bool matrixSet = false;
        Ogre::Matrix4 clippingMx;
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

void volume_render::updateClippingTM3D()
{
    auto clippingMatrix = m_clippingMatrix.lock();
    if(clippingMatrix)
    {
        sight::viz::scene3d::Utils::copyOgreMxToTM3D(m_widget->get_clipping_transform(), clippingMatrix.get_shared());

        const auto sig =
            clippingMatrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);

        core::com::connection::blocker blocker(sig->get_connection(this->slot(UPDATE_CLIPPING_BOX_SLOT)));

        sig->async_emit();
    }

    std::lock_guard<std::mutex> swapLock(m_mutex);
    m_volumeRenderer->clipImage(m_widget->get_clipping_box());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::setVisible(bool _visible)
{
    if(m_volumeSceneNode != nullptr)
    {
        m_volumeSceneNode->setVisible(_visible);

        if(m_widget)
        {
            m_widget->setBoxVisibility(_visible && m_config.visible);
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

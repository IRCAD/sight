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

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/helper/shading.hpp>
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
                const double color_bleeding_factor = _config.get<double>(config::COLOR_BLEEDING_FACTOR, 1.);

                m_config.shadows.colour_bleeding.enabled = _config.get<bool>(config::COLOR_BLEEDING, false);
                m_config.shadows.colour_bleeding.r       = static_cast<float>(color_bleeding_factor);
                m_config.shadows.colour_bleeding.g       = static_cast<float>(color_bleeding_factor);
                m_config.shadows.colour_bleeding.b       = static_cast<float>(color_bleeding_factor);
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

    auto render_service = this->getRenderService();
    render_service->makeCurrent();

    //scene (node, manager)
    {
        m_sceneManager = this->getSceneManager();

        Ogre::SceneNode* const root_scene_node = m_sceneManager->getRootSceneNode();
        Ogre::SceneNode* const transform_node  = this->getOrCreateTransformNode(root_scene_node);
        m_volumeSceneNode = transform_node->createChildSceneNode(this->get_id() + "_transform_origin");
    }

    //Renderer
    {
        sight::viz::scene3d::layer::sptr layer = render_service->getLayer(m_layerID);

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

    bool is_valid = false;
    //image
    {
        const auto image = m_image.lock();
        is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
    }

    if(is_valid)
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

    auto* const transform_node = this->getTransformNode();

    if(transform_node != nullptr)
    {
        m_sceneManager->getRootSceneNode()->removeChild(transform_node);
        this->getSceneManager()->destroySceneNode(static_cast<Ogre::SceneNode*>(transform_node));
    }

    this->destroyWidget();
}

//-----------------------------------------------------------------------------

void volume_render::updateVolumeTF()
{
    this->getRenderService()->makeCurrent();
    std::lock_guard swap_lock(m_mutex);

    {
        const auto tf = m_tf.lock();
        m_volumeRenderer->updateVolumeTF(tf.get_shared());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::newImage()
{
    auto render_service = this->getRenderService();

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

            auto* new_worker = render_service->getInteractorManager()->createGraphicsWorker();
            m_bufferingWorker = std::unique_ptr<sight::viz::scene3d::graphics_worker>(new_worker);
        }

        render_service->makeCurrent();
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
        auto buffering_fn =
            [this]()
            {
                const auto image = m_image.lock();

                m_volumeRenderer->loadImage();

                // Switch back to the main thread to compute the proxy geometry.
                // Ogre can't handle parallel rendering.
                this->slot(UPDATE_IMAGE_SLOT)->async_run();
            };

        m_bufferingWorker->pushTask(buffering_fn);
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
        const auto volume_tf = m_tf.lock();
        m_volumeRenderer->updateImage(image.get_shared(), volume_tf.get_shared());
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

void volume_render::updateSampling(unsigned _nb_samples)
{
    this->getRenderService()->makeCurrent();

    SIGHT_ASSERT("Sampling rate must fit in a 16 bit uint.", _nb_samples < 65536);

    const auto tf = m_tf.lock();
    m_volumeRenderer->setSampling(static_cast<std::uint16_t>(_nb_samples), tf.get_shared());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::updateOpacityCorrection(unsigned _opacity_correction)
{
    m_volumeRenderer->setOpacityCorrection(int(_opacity_correction));
    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::updateAOFactor(float _ao_factor)
{
    if(m_volumeRenderer->setAOFactor(_ao_factor))
    {
        this->requestRender(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateColorBleedingFactor(float _color_bleeding_factor)
{
    if(m_volumeRenderer->setColorBleedingFactor(_color_bleeding_factor))
    {
        this->requestRender(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatSizeRatio(unsigned _size_ratio)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATSizeRatio(_size_ratio);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatShellsNumber(unsigned _shells_number)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATShellsNumber(_shells_number);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatShellRadius(unsigned _shell_radius)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATShellRadius(_shell_radius);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatConeAngle(float _cone_angle)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATConeAngle(_cone_angle);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateSatConeSamples(unsigned _nb_cone_samples)
{
    if(m_volumeRenderer->shadows().parameters.enabled())
    {
        this->getRenderService()->makeCurrent();

        m_volumeRenderer->updateSATConeSamples(_nb_cone_samples);

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
        const auto image     = m_image.lock();
        const auto volume_tf = m_tf.lock();

        m_volumeRenderer->updateImage(image.get_shared(), volume_tf.get_shared());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void volume_render::toggleAmbientOcclusion(bool _ambient_occlusion)
{
    this->toggleVREffect(VREffectType::VR_AMBIENT_OCCLUSION, _ambient_occlusion);
}

//-----------------------------------------------------------------------------

void volume_render::toggleColorBleeding(bool _color_bleeding)
{
    this->toggleVREffect(VREffectType::VR_COLOR_BLEEDING, _color_bleeding);
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

void volume_render::setFocalDistance(int _focal_distance)
{
    if(this->getRenderService()->getLayer(m_layerID)->getStereoMode()
       != sight::viz::scene3d::compositor::core::stereo_mode_t::NONE)
    {
        m_volumeRenderer->setFocalLength(static_cast<float>(_focal_distance) / 100);
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
    std::lock_guard swap_lock(m_mutex);

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
    std::lock_guard<std::mutex> swap_lock(m_mutex);

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
    std::lock_guard swap_lock(m_mutex);

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
    auto clipping_mx_update = [this]{updateClippingTM3D();};

    Ogre::Matrix4 ogre_clipping_mx = Ogre::Matrix4::IDENTITY;

    const auto clipping_matrix = m_clippingMatrix.lock();
    if(clipping_matrix)
    {
        ogre_clipping_mx = sight::viz::scene3d::utils::convertTM3DToOgreMx(clipping_matrix.get_shared());
    }

    const sight::viz::scene3d::layer::sptr layer = this->getLayer();

    this->destroyWidget(); // Destroys the old widgets if they were created.
    m_widget = std::make_shared<sight::viz::scene3d::interactor::clipping_box_interactor>(
        layer,
        m_config.order_dependent,
        this->get_id(),
        m_volumeSceneNode,
        ogre_clipping_mx,
        clipping_mx_update,
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
        sight::viz::scene3d::layer::sptr layer = this->getLayer();
        layer->removeInteractor(m_widget);
        m_widget.reset();
    }
}

//-----------------------------------------------------------------------------

void volume_render::toggleVREffect(VREffectType _vr_effect, bool _enable)
{
    this->getRenderService()->makeCurrent();

    //First, check the image is valid (requires locking locally thus the lambda)
    bool is_valid = false;
    {
        const auto image = m_image.lock();
        is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
    }

    // Volume illumination is only implemented for raycasting rendering
    if(is_valid)
    {
        //Renderer update
        {
            switch(_vr_effect)
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
            const auto image     = m_image.lock();
            const auto volume_tf = m_tf.lock();

            m_volumeRenderer->updateImage(image.get_shared(), volume_tf.get_shared());
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateClippingBox()
{
    if(m_widget)
    {
        bool matrix_set = false;
        Ogre::Matrix4 clipping_mx;
        {
            const auto clipping_matrix = m_clippingMatrix.lock();
            if(clipping_matrix)
            {
                clipping_mx = sight::viz::scene3d::utils::convertTM3DToOgreMx(clipping_matrix.get_shared());
                matrix_set  = true;
            }
        }

        if(matrix_set)
        {
            this->getRenderService()->makeCurrent();

            // updateFromTransform is called outside of the lock of the InOut data to prevent a deadlock
            m_widget->updateFromTransform(clipping_mx);
        }
    }
}

//-----------------------------------------------------------------------------

void volume_render::updateClippingTM3D()
{
    auto clipping_matrix = m_clippingMatrix.lock();
    if(clipping_matrix)
    {
        sight::viz::scene3d::utils::copyOgreMxToTM3D(m_widget->get_clipping_transform(), clipping_matrix.get_shared());

        const auto sig =
            clipping_matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);

        core::com::connection::blocker blocker(sig->get_connection(this->slot(UPDATE_CLIPPING_BOX_SLOT)));

        sig->async_emit();
    }

    std::lock_guard<std::mutex> swap_lock(m_mutex);
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

/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/scene3d/layer.hpp"

#include "helper/scene.hpp"

#include "viz/scene3d/adaptor.hpp"
#include "viz/scene3d/compositor/core.hpp"
#include "viz/scene3d/helper/camera.hpp"
#include "viz/scene3d/light_adaptor.hpp"
#include "viz/scene3d/ogre.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>

#include <data/tools/color.hpp>

#include <service/registry.hpp>

#include <boost/tokenizer.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRectangle2D.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>

#include <cmath>
#include <memory>
#include <stack>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

const core::com::signals::key_t layer::INIT_LAYER_SIG           = "layerInitialized";
const core::com::signals::key_t layer::RESIZE_LAYER_SIG         = "layerResized";
const core::com::signals::key_t layer::CAMERA_RANGE_UPDATED_SIG = "CameraRangeUpdated";

const core::com::slots::key_t layer::INTERACTION_SLOT  = "interaction";
const core::com::slots::key_t layer::RESET_CAMERA_SLOT = "resetCamera";

//-----------------------------------------------------------------------------

const std::string layer::s_DEFAULT_CAMERA_NAME      = "DefaultCam";
const std::string layer::s_DEFAULT_LIGHT_NAME       = "DefaultLight";
const std::string layer::s_DEFAULT_CAMERA_NODE_NAME = "CameraNode";

//-----------------------------------------------------------------------------

struct layer::LayerCameraListener : public Ogre::Camera::Listener
{
    layer* m_layer {nullptr};
    int m_frameId {0};

    //------------------------------------------------------------------------------

    explicit LayerCameraListener(layer* _renderer) :
        m_layer(_renderer)
    {
    }

    //------------------------------------------------------------------------------

    void cameraPreRenderScene(Ogre::Camera* /*cam*/) final
    {
        SIGHT_ASSERT("layer is not set", m_layer);

        if(m_layer->getStereoMode() != viz::scene3d::compositor::core::stereo_mode_t::NONE)
        {
            const int frame_id = m_layer->getRenderService()->getInteractorManager()->getFrameId();
            if(frame_id != m_frameId)
            {
                auto& gpu_program_mgr = Ogre::GpuProgramManager::getSingleton();

                for(std::uint8_t i = 0 ; i < m_layer->numCameras() ; ++i)
                {
                    Ogre::Matrix4 proj_mat = m_layer->getCameraProjMat(i);

                    proj_mat[1][0] = -proj_mat[1][0];
                    proj_mat[1][1] = -proj_mat[1][1];
                    proj_mat[1][2] = -proj_mat[1][2];
                    proj_mat[1][3] = -proj_mat[1][3];

                    const auto& shared_parameter_map = gpu_program_mgr.getAvailableSharedParameters();
                    {
                        const std::string proj_param_name = "ProjectionMatrixParam/" + std::to_string(i);
                        auto it                           = shared_parameter_map.find(proj_param_name);
                        if(it != shared_parameter_map.end())
                        {
                            it->second->setNamedConstant("u_proj", proj_mat);
                        }
                    }
                    {
                        const std::string proj_param_name = "InverseProjectionMatrixParam/" + std::to_string(i);
                        auto it                           = shared_parameter_map.find(proj_param_name);
                        if(it != shared_parameter_map.end())
                        {
                            it->second->setNamedConstant("u_invProj", proj_mat.inverse());
                        }
                    }
                }

                m_frameId = frame_id;
            }
        }
    }

    //------------------------------------------------------------------------------
};

layer::layer()
{
    new_signal<init_layer_signal_t>(INIT_LAYER_SIG);
    new_signal<resize_layer_signal_t>(RESIZE_LAYER_SIG);
    new_signal<camera_updated_signal_t>(CAMERA_RANGE_UPDATED_SIG);

    new_slot(INTERACTION_SLOT, &layer::interaction, this);
    new_slot(RESET_CAMERA_SLOT, &layer::resetCameraCoordinates, this);
}

//-----------------------------------------------------------------------------

Ogre::RenderTarget* layer::getRenderTarget() const
{
    return m_renderTarget;
}

//-----------------------------------------------------------------------------

void layer::setRenderTarget(Ogre::RenderTarget* _render_target)
{
    m_renderTarget = _render_target;
}

//-----------------------------------------------------------------------------

void layer::set_id(const std::string& _id)
{
    auto render_service = m_renderService.lock();
    SIGHT_ASSERT("Render service must be set before calling set_id().", render_service);
    m_id = _id;
}

//-----------------------------------------------------------------------------

std::string layer::getName() const
{
    return m_sceneManager->getName();
}

//-----------------------------------------------------------------------------

const std::string& layer::getLayerID() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

Ogre::SceneManager* layer::getSceneManager() const
{
    return m_sceneManager;
}

//-----------------------------------------------------------------------------

void layer::createScene()
{
    if(initialized())
    {
        return;
    }

    namespace fwc = viz::scene3d::compositor;
    {
        auto render_service = m_renderService.lock();
        auto* root          = viz::scene3d::utils::getOgreRoot();
        m_sceneManager = root->createSceneManager("DefaultSceneManager", render_service->get_id() + "_" + m_id);
    }

    SIGHT_ASSERT("scene manager must be initialized", m_sceneManager);
    SIGHT_ASSERT("Render window must be initialized", m_renderTarget);

    m_sceneManager->setAmbientLight(Ogre::ColourValue(0.8F, 0.8F, 0.8F));

    // Create the camera
    m_camera = m_sceneManager->createCamera(layer::s_DEFAULT_CAMERA_NAME);
    m_camera->setNearClipDistance(1);

    const auto& [left, top, width, height] = m_viewportCfg;
    auto* viewport = m_renderTarget->addViewport(m_camera, m_order, left, top, width, height);
    SIGHT_ASSERT("Could not create a viewport", viewport);

    m_compositorChainManager = std::make_unique<fwc::chain_manager>(this->get_sptr());

    if(m_order != 0)
    {
        viewport->setClearEveryFrame(true, Ogre::FBT_DEPTH);
    } // Set the background material
    else
    {
        // FIXME : background isn't shown when using compositor with a clear pass
        // We must blend the input previous in each compositor
        Ogre::MaterialPtr default_material = Ogre::MaterialManager::getSingleton().getByName(
            m_backgroundMaterial.empty() ? "Background" : m_backgroundMaterial,
            RESOURCE_GROUP
        );
        SIGHT_ASSERT("Could not find material ", default_material);
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
            this->getName() + "backgroundMat",
            RESOURCE_GROUP
        );
        default_material.get()->copyDetailsTo(material);

        if(m_backgroundMaterial.empty())
        {
            std::array<std::uint8_t, 4> color {};
            Ogre::Pass* pass                           = material->getTechnique(0)->getPass(0);
            Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

            data::tools::color::hexaStringToRGBA(m_topColor, color);
            Ogre::ColourValue ogre_top_color(static_cast<float>(color[0]) / 255.F,
                                             static_cast<float>(color[1]) / 255.F,
                                             static_cast<float>(color[2]) / 255.F,
                                             1.F);
            params->setNamedConstant("topColour", ogre_top_color);

            data::tools::color::hexaStringToRGBA(m_bottomColor, color);
            Ogre::ColourValue ogre_bot_color(static_cast<float>(color[0]) / 255.F,
                                             static_cast<float>(color[1]) / 255.F,
                                             static_cast<float>(color[2]) / 255.F,
                                             1.F);
            params->setNamedConstant("bottomColour", ogre_bot_color);

            params->setNamedConstant("topScale", m_topScale);
            params->setNamedConstant("bottomScale", m_bottomScale);
        }

        // Create background rectangle covering the whole screen
        auto* rect = new Ogre::Rectangle2D();
        rect->setCorners(-1.0, 1.0, 1.0, -1.0);
        rect->setMaterial(material);

        // Render the background before everything else
        rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

        // Use infinite AAB to always stay visible
        Ogre::AxisAlignedBox aab_inf;
        aab_inf.setInfinite();
        rect->setBoundingBox(aab_inf);

        // Attach background to the scene
        Ogre::SceneNode* node = m_sceneManager->getRootSceneNode()->createChildSceneNode("Background");
        node->attachObject(rect);
    }

    // Alter the camera aspect ratio to match the viewport
    SIGHT_ASSERT(
        "Width and height should be strictly positive",
        viewport->getActualWidth() > 0 && viewport->getActualHeight() > 0
    );
    m_camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    // Creating Camera scene Node
    Ogre::SceneNode* camera_node = m_sceneManager->getRootSceneNode()->createChildSceneNode(
        layer::s_DEFAULT_CAMERA_NODE_NAME
    );
    camera_node->setPosition(Ogre::Vector3(0, 0, 5));
    camera_node->lookAt(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_WORLD);

    camera_node->attachObject(m_camera);

    auto render_service = m_renderService.lock();
    if(m_hasDefaultLight)
    {
        m_defaultLightDiffuseColor  = std::make_shared<data::color>();
        m_defaultLightSpecularColor = std::make_shared<data::color>();

        m_lightAdaptor = viz::scene3d::light_adaptor::createLightAdaptor(
            m_defaultLightDiffuseColor,
            m_defaultLightSpecularColor
        );
        m_lightAdaptor->setName(layer::s_DEFAULT_LIGHT_NAME);
        m_lightAdaptor->setType(Ogre::Light::LT_DIRECTIONAL);
        m_lightAdaptor->setTransformId(camera_node->getName());
        m_lightAdaptor->setLayerID(this->getLayerID());
        m_lightAdaptor->setRenderService(render_service);
        m_lightAdaptor->start();
    }

    m_cameraListener = new LayerCameraListener(this);
    m_camera->addListener(m_cameraListener);

    // Setup transparency compositors
    if(m_hasCoreCompositor)
    {
        this->setupCore();
    }

    // Setup custom compositors and autostereo
    {
        boost::char_separator<char> sep(";");
        boost::tokenizer<boost::char_separator<char> > tok(m_rawCompositorChain, sep);
        std::vector<fwc::chain_manager::compositor_id_t> compositor_chain;

        for(const auto& it : tok)
        {
            compositor_chain.push_back(it);
        }

        if(m_stereoMode != compositor::core::stereo_mode_t::NONE)
        {
            m_autostereoListener = new compositor::listener::auto_stereo_compositor_listener(this->numCameras());
            Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
        }

        m_compositorChainManager->setCompositorChain(compositor_chain);
    }

    m_sceneCreated = true;

    this->signal<init_layer_signal_t>(INIT_LAYER_SIG)->async_emit(this->get_sptr());
}

// ----------------------------------------------------------------------------

void layer::destroyScene()
{
    if(m_order == 0)
    {
        // Remove the background material for the first layer
        Ogre::MaterialManager::getSingleton().remove(
            this->getName() + "backgroundMat",
            viz::scene3d::RESOURCE_GROUP
        );
    }

    if(m_lightAdaptor)
    {
        viz::scene3d::light_adaptor::destroyLightAdaptor(m_lightAdaptor);
        m_lightAdaptor.reset();
    }

    if((m_camera != nullptr) && (m_cameraListener != nullptr))
    {
        m_camera->removeListener(m_cameraListener);
        delete m_cameraListener;
        m_cameraListener = nullptr;
    }

    if(m_autostereoListener != nullptr)
    {
        Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
        m_autostereoListener = nullptr;
    }

    if(m_sceneManager != nullptr)
    {
        viz::scene3d::utils::getOgreRoot()->destroySceneManager(m_sceneManager);
        m_sceneManager = nullptr;
    }

    m_compositorChainManager = nullptr;

    m_camera = nullptr;
}

// ----------------------------------------------------------------------------

void layer::addAvailableCompositor(std::string _compositor_name)
{
    m_compositorChainManager->addAvailableCompositor(_compositor_name);
}

// ----------------------------------------------------------------------------

void layer::updateCompositorState(std::string _compositor_name, bool _is_enabled)
{
    auto render_service = m_renderService.lock();

    render_service->makeCurrent();
    m_compositorChainManager->updateCompositorState(_compositor_name, _is_enabled);
    render_service->requestRender();
}

// ----------------------------------------------------------------------------

void layer::forAllInteractors(const std::function<void(const interactor::base::sptr&)>&& _f)
{
    const auto interactors_begin = m_interactors.begin();
    const auto interactors_end   = m_interactors.end();

    for(auto it = interactors_begin ; it != interactors_end && !m_cancelFurtherInteraction ; ++it)
    {
        const interactor::base::sptr interactor = it->second.lock();
        if(interactor)
        {
            _f(interactor);
        }
        else // remove the expired interactor.
        {
            it = m_interactors.erase(it);
        }
    }
}

// ----------------------------------------------------------------------------

void layer::interaction(viz::scene3d::window_interactor::InteractionInfo _info)
{
    this->getRenderService()->makeCurrent();

    switch(_info.interactionType)
    {
        case viz::scene3d::window_interactor::InteractionInfo::MOUSEMOVE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->mouseMoveEvent(_info.button, _info.modifiers, _info.x, _info.y, _info.dx, _info.dy);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::WHEELMOVE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->wheelEvent(_info.modifiers, _info.delta, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::RESIZE:
        {
            auto sig = this->signal<resize_layer_signal_t>(RESIZE_LAYER_SIG);
            sig->async_emit(_info.x, _info.y);

            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
                {
                    _i->resizeEvent(_info.x, _info.y);
                });
            break;
        }

        case viz::scene3d::window_interactor::InteractionInfo::KEYPRESS:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->keyPressEvent(_info.key, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::KEYRELEASE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->keyReleaseEvent(_info.key, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::BUTTONRELEASE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->buttonReleaseEvent(_info.button, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::BUTTONPRESS:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->buttonPressEvent(_info.button, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::BUTTONDOUBLEPRESS:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->buttonDoublePressEvent(_info.button, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::PINCH_GESTURE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->pinchGestureEvent(_info.delta, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::PAN_GESTURE_MOVE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->panGestureMoveEvent(_info.x, _info.y, _info.dx, _info.dy);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::PAN_GESTURE_RELEASE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->panGestureReleaseEvent(_info.x, _info.y, _info.dx, _info.dy);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::LONG_TAP_GESTURE:
            this->forAllInteractors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->longTapGestureEvent(_info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::LEAVE:
            this->forAllInteractors(
                [](const interactor::base::sptr& _i)
            {
                _i->leaveEvent();
            });
            break;

        case viz::scene3d::window_interactor::InteractionInfo::ENTER:
            this->forAllInteractors(
                [](const interactor::base::sptr& _i)
            {
                _i->enterEvent();
            });
            break;
    }

    m_cancelFurtherInteraction = false;
}

// ----------------------------------------------------------------------------

int layer::getOrder() const
{
    return m_order;
}

// ----------------------------------------------------------------------------

void layer::setOrder(int _order)
{
    m_order = _order;
}

// ----------------------------------------------------------------------------

void layer::set_worker(const core::thread::worker::sptr& _worker)
{
    core::com::has_slots::m_slots.set_worker(_worker);
}

// ----------------------------------------------------------------------------

viz::scene3d::render::sptr layer::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

void layer::setRenderService(const viz::scene3d::render::sptr& _service)
{
    SIGHT_ASSERT("service not instanced", _service);

    m_renderService = _service;
}

// ----------------------------------------------------------------------------

void layer::addInteractor(const viz::scene3d::interactor::base::sptr& _interactor, int _priority)
{
    using pair_t = typename decltype(m_interactors)::value_type;
    const pair_t pair = std::make_pair(_priority, _interactor);

    const auto is_pair = [&pair](const pair_t& _p)
                         {
                             return pair.second.lock() == _p.second.lock();
                         };

    // Add the interactor if it doesn't already exit.
    if(std::none_of(m_interactors.begin(), m_interactors.end(), is_pair))
    {
        m_interactors.insert(pair);
    }
}

// ----------------------------------------------------------------------------

void layer::removeInteractor(const viz::scene3d::interactor::base::sptr& _interactor)
{
    const auto interactor_equal = [&_interactor](typename decltype(m_interactors)::value_type _i)
                                  {
                                      return _i.second.lock() == _interactor;
                                  };

    const auto it = std::find_if(m_interactors.cbegin(), m_interactors.cend(), interactor_equal);

    if(it != m_interactors.end())
    {
        m_interactors.erase(it);
    }
}

//-----------------------------------------------------------------------------

Ogre::AxisAlignedBox layer::computeWorldBoundingBox() const
{
    // Getting this render service scene manager
    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();

    // Needed to recompute world bounding boxes
    root_scene_node->_update(true, false);

    return helper::scene::computeBoundingBox(root_scene_node);
}

//------------------------------------------------------------------------------

compositor::transparencyTechnique layer::getTransparencyTechnique()
{
    if(m_coreCompositor)
    {
        return m_coreCompositor->getTransparencyTechnique();
    }

    return compositor::DEFAULT;
}

//------------------------------------------------------------------------------

int layer::getTransparencyDepth()
{
    if(m_coreCompositor)
    {
        return m_coreCompositor->getTransparencyDepth();
    }

    return 0;
}

//------------------------------------------------------------------------------

bool layer::setTransparencyTechnique(compositor::transparencyTechnique _technique)
{
    bool success = false;
    if(m_coreCompositor)
    {
        // Playing with the transparency may create new compositors, thus new resources in OpenGL, thus we
        // need to explicitly switch to our OpenGL context
        this->getRenderService()->makeCurrent();
        success = m_coreCompositor->setTransparencyTechnique(_technique);
        m_coreCompositor->update();
        this->requestRender();
    }

    return success;
}

//------------------------------------------------------------------------------

void layer::setTransparencyDepth(int _depth)
{
    if(m_coreCompositor)
    {
        // Playing with the transparency may create new compositors, thus new resources in OpenGL, thus we
        // need to explicitly switch to our OpenGL context
        this->getRenderService()->makeCurrent();
        m_coreCompositor->setTransparencyDepth(_depth);
        m_coreCompositor->update();
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void layer::setCameraCalibrations(const camera_calibrations_t& _calibrations)
{
    m_stereoCameraCalibration = _calibrations;
}

//-----------------------------------------------------------------------------

bool layer::isDefaultLight(const viz::scene3d::light_adaptor::csptr& _light) const
{
    return m_lightAdaptor == _light;
}

//-----------------------------------------------------------------------------

void layer::removeDefaultLight()
{
    SIGHT_ASSERT("m_lightAdaptor must not be null", m_lightAdaptor != nullptr);
    viz::scene3d::light_adaptor::destroyLightAdaptor(m_lightAdaptor);
    m_lightAdaptor.reset();
}

//------------------------------------------------------------------------------

float layer::computeSceneLength(const Ogre::AxisAlignedBox& _world_bounding_box)
{
    // Arbitrary coefficient
    const Ogre::Real bounding_box_length = _world_bounding_box.getSize().length() > 0
                                           ? _world_bounding_box.getSize().length() : 0;

    SIGHT_DEBUG("Zoom coefficient : " << bounding_box_length);

    // Update interactor's mouse scale
    this->forAllInteractors(
        [bounding_box_length](const interactor::base::sptr& _i)
        {
            _i->setSceneLength(bounding_box_length);
        });

    return bounding_box_length;
}

//-----------------------------------------------------------------------------

void layer::resetCameraCoordinates()
{
    const Ogre::AxisAlignedBox world_bounding_box = this->computeWorldBoundingBox();

    if((m_camera != nullptr))
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if(world_bounding_box == Ogre::AxisAlignedBox::EXTENT_NULL
           || world_bounding_box == Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            Ogre::SceneNode* cam_node = m_camera->getParentSceneNode();
            cam_node->setPosition(0.F, 0.F, 0.F);
        }
        else
        {
            // Arbitrary coefficient
            const float bounding_box_length = computeSceneLength(world_bounding_box);

            // Set the direction of the camera
            Ogre::SceneNode* cam_node     = m_camera->getParentSceneNode();
            const Ogre::Quaternion quat   = cam_node->getOrientation();
            const Ogre::Vector3 direction = quat.zAxis();

            // Set the position of the camera
            cam_node->setPosition((world_bounding_box.getCenter() + bounding_box_length * direction));
        }
    }
}

//-----------------------------------------------------------------------------

void layer::computeCameraParameters()
{
    const Ogre::AxisAlignedBox world_bounding_box = this->computeWorldBoundingBox();

    if((m_camera != nullptr))
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if(world_bounding_box != Ogre::AxisAlignedBox::EXTENT_NULL
           && world_bounding_box != Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            computeSceneLength(world_bounding_box);
        }
    }
}

//-----------------------------------------------------------------------------

void layer::resetCameraClippingRange() const
{
    auto world_bounding_box = this->computeWorldBoundingBox();

    if((m_camera != nullptr) and world_bounding_box.isFinite())
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if(world_bounding_box == Ogre::AxisAlignedBox::EXTENT_NULL
           || world_bounding_box == Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            return;
        }

        Ogre::SceneNode* cam_node = m_camera->getParentSceneNode();

        // Set the direction of the camera
        Ogre::Quaternion quat   = cam_node->getOrientation();
        Ogre::Vector3 direction = quat.zAxis();
        Ogre::Vector3 position  = cam_node->getPosition();

        // Set near and far plan
        Ogre::Vector3 minimum = world_bounding_box.getMinimum();
        Ogre::Vector3 maximum = world_bounding_box.getMaximum();

        const float a = -direction.x;
        const float b = -direction.y;
        const float c = -direction.z;
        const float d = -(a * position.x + b * position.y + c * position.z);

        // Max near and min far
        float max_near = a * minimum.x + b * minimum.y + c * minimum.z + d;
        float min_far  = 1e-18F;

        std::array corners {
            minimum.x,
            maximum.x,
            minimum.y,
            maximum.y,
            minimum.z,
            maximum.z
        };
        // Find the closest / farthest bounding box vertex
        for(std::size_t k = 0 ; k < 2 ; k++)
        {
            for(std::size_t j = 0 ; j < 2 ; j++)
            {
                for(std::size_t i = 0 ; i < 2 ; i++)
                {
                    const float dist = a * corners[i] + b * corners[2 + j] + c * corners[4 + k] + d;
                    max_near = std::min(dist, max_near);
                    min_far  = std::max(dist, min_far);
                }
            }
        }

        // Make sure near is not bigger than far
        auto near = std::min(min_far, max_near);
        auto far  = std::max(min_far, max_near);

        // Give ourselves a little breathing room
        auto range = std::max((far - near) * 0.25F, 2.F);
        far  = far + range;
        near = near - range;

        // Do not let the range behind the camera throw off the calculation.
        near = std::max(near, 0.1F);

        const auto& chain            = this->getCompositorChain();
        const auto sao_compositor_it = std::find_if(
            chain.begin(),
            chain.end(),
            [](const auto& _compositor)
            {
                return _compositor.first == "SAO";
            });

        const auto prev_far = m_camera->getFarClipDistance();
        if(sao_compositor_it != chain.end() && sao_compositor_it->second)
        {
            // Near and far for SAO
            far = 10000;
        }

        if(m_cameraOrthographic && m_camera->getProjectionType() != Ogre::PT_PERSPECTIVE)
        {
            // Use height as the difference on y coordinates.
            const auto y1     = world_bounding_box.getMinimum().y;
            const auto y2     = world_bounding_box.getMaximum().y;
            Ogre::Real h      = y2 - y1;
            Ogre::Real margin = 0.1F;
            m_camera->setOrthoWindowHeight(h + h * margin);
        }

        // Keep the near plane close all the time. We need this for instance to display the shape extruder tool
        m_camera->setNearClipDistance(near);
        m_camera->setFarClipDistance(far);

        if(far != prev_far)
        {
            this->signal<camera_updated_signal_t>(CAMERA_RANGE_UPDATED_SIG)->async_emit();
        }
    }
}

//-----------------------------------------------------------------------------

void layer::requestRender()
{
    m_renderService.lock()->requestRender();
}

//-----------------------------------------------------------------------------

void layer::setStereoMode(compositor::core::stereo_mode_t _mode)
{
    // Disable the old compositor
    if(m_stereoMode != compositor::core::stereo_mode_t::NONE)
    {
        Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
        m_autostereoListener = nullptr;
    }

    // Enable the new one
    m_stereoMode = _mode;

    this->getRenderService()->makeCurrent();
    m_coreCompositor->setStereoMode(_mode);
    m_coreCompositor->update();

    if(m_stereoMode != compositor::core::stereo_mode_t::NONE)
    {
        m_autostereoListener = new compositor::listener::auto_stereo_compositor_listener(this->numCameras());
        Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
    }

    this->restartAdaptors();
}

//-----------------------------------------------------------------------------

void layer::setBackgroundColor(std::string _top_color, std::string _bot_color)
{
    m_topColor    = _top_color;
    m_bottomColor = _bot_color;
}

//-----------------------------------------------------------------------------

void layer::setBackgroundScale(float _top_scale, float _bot_scale)
{
    m_topScale    = _top_scale;
    m_bottomScale = _bot_scale;
}

//-----------------------------------------------------------------------------

void layer::setBackgroundMaterial(const std::string& _background)
{
    m_backgroundMaterial = _background;
}

//-------------------------------------------------------------------------------------

void layer::setCoreCompositorEnabled(
    bool _enabled,
    std::string _transparency_technique,
    std::string _num_peels,
    compositor::core::stereo_mode_t _stereo_mode
)
{
    m_hasCoreCompositor = _enabled;
    m_stereoMode        = _stereo_mode;
    if(!_transparency_technique.empty())
    {
        if(_transparency_technique == "DepthPeeling")
        {
            m_transparencyTechnique = viz::scene3d::compositor::DEPTHPEELING;
        }
        else if(_transparency_technique == "CelShadingDepthPeeling")
        {
            m_transparencyTechnique = viz::scene3d::compositor::CELLSHADING_DEPTHPEELING;
        }
        else if(_transparency_technique == "DualDepthPeeling")
        {
            m_transparencyTechnique = viz::scene3d::compositor::DUALDEPTHPEELING;
        }
        else if(_transparency_technique == "WeightedBlended")
        {
            m_transparencyTechnique = viz::scene3d::compositor::WEIGHTEDBLENDEDOIT;
        }
        else if(_transparency_technique == "HybridTransparency" || _transparency_technique == "Default")
        {
            m_transparencyTechnique = viz::scene3d::compositor::HYBRIDTRANSPARENCY;
        }
        else
        {
            SIGHT_ERROR("Unknown transparency technique : " << _transparency_technique);
        }
    }

    if(!_num_peels.empty())
    {
        m_numPeels = std::stoi(_num_peels);
    }
}

//-------------------------------------------------------------------------------------

void layer::setCompositorChainEnabled(const std::string& _compositor_chain)
{
    m_hasCompositorChain = !_compositor_chain.empty();

    if(m_hasCompositorChain)
    {
        m_rawCompositorChain = _compositor_chain;
    }
}

//-------------------------------------------------------------------------------------

void layer::setViewportConfig(const viewport_config_t& _vp_cfg)
{
    m_viewportCfg = _vp_cfg;
}

//-------------------------------------------------------------------------------------

bool layer::isCoreCompositorEnabled() const
{
    return m_hasCoreCompositor;
}

//-------------------------------------------------------------------------------------

bool layer::isCompositorChainEnabled() const
{
    return m_hasCompositorChain;
}

//-------------------------------------------------------------------------------------

bool layer::initialized() const
{
    return m_sceneManager != nullptr;
}

//-------------------------------------------------------------------------------------

compositor::core::stereo_mode_t layer::getStereoMode() const
{
    return m_stereoMode;
}

//-------------------------------------------------------------------------------------

void layer::setupCore()
{
    // Needed to setup compositors in GL3Plus, Ogre creates render targets
    m_renderService.lock()->makeCurrent();

    m_coreCompositor = std::make_shared<viz::scene3d::compositor::core>(this->getViewport());
    m_coreCompositor->setStereoMode(m_stereoMode);
    m_coreCompositor->setTransparencyTechnique(m_transparencyTechnique);
    m_coreCompositor->setTransparencyDepth(m_numPeels);
    m_coreCompositor->update();
}

//-------------------------------------------------------------------------------------

void layer::restartAdaptors()
{
    auto adaptors = this->getRenderService()->getAdaptors<adaptor>();

    auto not_in_layer = [this](const adaptor::sptr& _adapt)
                        {
                            return _adapt->getLayerID() != this->m_id || _adapt == nullptr;
                        };

    // Only keep adaptors belonging to this layer.
    std::erase_if(adaptors, not_in_layer);

    // Search for all adaptors created as subservices by other adaptors.
    std::vector<service::base::wptr> sub_adaptors;
    for(auto& adapt : adaptors)
    {
        SPTR(service::has_services) has_services = std::dynamic_pointer_cast<service::has_services>(adapt);
        if(has_services != nullptr)
        {
            const auto& sub_services = has_services->getRegisteredServices();
            std::copy(sub_services.begin(), sub_services.end(), std::back_inserter(sub_adaptors));
        }
    }

    // Subadaptors should be started/stopped by their parent. Remove them from the list.
    for(const auto& sub_srv : sub_adaptors)
    {
        auto it = std::find(adaptors.begin(), adaptors.end(), sub_srv.lock());

        if(it != adaptors.end())
        {
            adaptors.erase(it);
        }
    }

    // Restart all adaptors.
    for(auto& adapt : adaptors)
    {
        if(adapt->started())
        {
            adapt->stop().wait();
            adapt->start().wait();
        }
    }
}

//-------------------------------------------------------------------------------------

viz::scene3d::compositor::chain_manager::compositor_chain_t layer::getCompositorChain() const
{
    return m_compositorChainManager->getCompositorChain();
}

//-------------------------------------------------------------------------------------

service::has_services::ServiceVector layer::getRegisteredAdaptors() const
{
    if(m_compositorChainManager)
    {
        return m_compositorChainManager->getRegisteredServices();
    }

    return {};
}

//-------------------------------------------------------------------------------------

Ogre::Viewport* layer::getViewport() const
{
    return m_renderTarget->getViewport(m_renderTarget->getNumViewports() - 1);
}

//-------------------------------------------------------------------------------------

bool layer::isSceneCreated() const
{
    return m_sceneCreated;
}

//-------------------------------------------------------------------------------------

Ogre::Camera* layer::getDefaultCamera() const
{
    return m_camera;
}

//-------------------------------------------------------------------------------------

Ogre::Matrix4 layer::getCameraProjMat(const uint8_t _camera_idx) const
{
    SIGHT_ASSERT(
        "Index exceeds the number of cameras used for this stereo mode",
        _camera_idx < this->numCameras()
    );
    Ogre::Matrix4 extrinsic_transform(Ogre::Matrix4::IDENTITY);

    if(m_stereoMode == viz::scene3d::compositor::core::stereo_mode_t::AUTOSTEREO_5)
    {
        const float eye_angle = 0.02321F;
        const float angle     = eye_angle * (-2.F + float(_camera_idx));

        extrinsic_transform = viz::scene3d::helper::camera::computeFrustumShearTransform(*m_camera, angle);
    }
    else if(m_stereoMode == viz::scene3d::compositor::core::stereo_mode_t::AUTOSTEREO_8)
    {
        const float eye_angle = 0.01625F;
        const float angle     = eye_angle * (-3.5F + float(_camera_idx));

        extrinsic_transform = viz::scene3d::helper::camera::computeFrustumShearTransform(*m_camera, angle);
    }
    else if(m_stereoMode == viz::scene3d::compositor::core::stereo_mode_t::STEREO)
    {
        if(m_stereoCameraCalibration.size() < 2 && _camera_idx == 1)
        {
            SIGHT_WARN("Only a single calibration was set but stereo rendering is set.");
            // Kept for compatibility purposes.
            extrinsic_transform = Ogre::Matrix4(
                1,
                0,
                0,
                5,
                0,
                1,
                0,
                0,
                0,
                0,
                1,
                0,
                0,
                0,
                0,
                1
            );
        }
        else if(!m_stereoCameraCalibration.empty())
        {
            return m_stereoCameraCalibration[_camera_idx];
        }
    }

    const Ogre::Matrix4 proj_mat = m_camera->getProjectionMatrixWithRSDepth();

    return proj_mat * extrinsic_transform;
}

//-------------------------------------------------------------------------------------

uint8_t layer::numCameras() const
{
    return m_stereoMode == viz::scene3d::compositor::core::stereo_mode_t::AUTOSTEREO_8 ? 8
                                                                                       : m_stereoMode
           == viz::scene3d::compositor::core::stereo_mode_t::AUTOSTEREO_5 ? 5
                                                                          :
           m_stereoMode == viz::scene3d::compositor::core::stereo_mode_t::STEREO ? 2 : 1;
}

//-------------------------------------------------------------------------------------

void layer::setHasDefaultLight(bool _has_default_light)
{
    m_hasDefaultLight = _has_default_light;
}

//-------------------------------------------------------------------------------------

int layer::getLightsNumber() const
{
    auto light_adaptors = this->getRenderService()->getAdaptors<viz::scene3d::light_adaptor>();
    int lights_number(0);

    std::ranges::for_each(
        light_adaptors,
        [&](viz::scene3d::light_adaptor::sptr _adaptor)
        {
            if(_adaptor->getLayerID() == this->getLayerID())
            {
                ++lights_number;
            }
        });

    return lights_number;
}

//-------------------------------------------------------------------------------------

std::vector<viz::scene3d::light_adaptor::sptr> layer::getLightAdaptors() const
{
    auto light_adaptors = this->getRenderService()->getAdaptors<viz::scene3d::light_adaptor>();
    std::vector<viz::scene3d::light_adaptor::sptr> layer_light_adaptors;

    std::ranges::for_each(
        light_adaptors,
        [&](viz::scene3d::light_adaptor::sptr _light_adaptor)
        {
            if(_light_adaptor->getLayerID() == this->getLayerID())
            {
                layer_light_adaptors.push_back(_light_adaptor);
            }
        });

    return layer_light_adaptors;
}

//-----------------------------------------------------------------------------

void layer::cancelFurtherInteraction()
{
    m_cancelFurtherInteraction = true;
}

//-----------------------------------------------------------------------------

void layer::setOrthographicCamera(bool _ortho)
{
    m_cameraOrthographic = _ortho;
}

//-----------------------------------------------------------------------------

bool layer::isOrthographicCameraForce() const
{
    return m_cameraOrthographic;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d.

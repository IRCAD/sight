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

#include "viz/scene3d/Layer.hpp"

#include "viz/scene3d/compositor/Core.hpp"
#include "viz/scene3d/helper/Camera.hpp"
#include "viz/scene3d/IAdaptor.hpp"
#include "viz/scene3d/ILight.hpp"
#include "viz/scene3d/ogre.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/thread/Worker.hpp>

#include <data/tools/Color.hpp>

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

const core::com::Signals::SignalKeyType Layer::s_INIT_LAYER_SIG           = "layerInitialized";
const core::com::Signals::SignalKeyType Layer::s_RESIZE_LAYER_SIG         = "layerResized";
const core::com::Signals::SignalKeyType Layer::s_CAMERA_RANGE_UPDATED_SIG = "CameraRangeUpdated";

const core::com::Slots::SlotKeyType Layer::s_INTERACTION_SLOT  = "interaction";
const core::com::Slots::SlotKeyType Layer::s_RESET_CAMERA_SLOT = "resetCamera";

//-----------------------------------------------------------------------------

const std::string Layer::s_DEFAULT_CAMERA_NAME      = "DefaultCam";
const std::string Layer::s_DEFAULT_LIGHT_NAME       = "DefaultLight";
const std::string Layer::s_DEFAULT_CAMERA_NODE_NAME = "CameraNode";

//-----------------------------------------------------------------------------

struct Layer::LayerCameraListener : public Ogre::Camera::Listener
{
    Layer* m_layer {nullptr};
    int m_frameId {0};

    //------------------------------------------------------------------------------

    explicit LayerCameraListener(Layer* renderer) :
        m_layer(renderer)
    {
    }

    //------------------------------------------------------------------------------

    void cameraPreRenderScene(Ogre::Camera* /*cam*/) final
    {
        SIGHT_ASSERT("Layer is not set", m_layer);

        if(m_layer->getStereoMode() != viz::scene3d::compositor::Core::StereoModeType::NONE)
        {
            const int frameId = m_layer->getRenderService()->getInteractorManager()->getFrameId();
            if(frameId != m_frameId)
            {
                auto& gpuProgramMgr = Ogre::GpuProgramManager::getSingleton();

                for(std::uint8_t i = 0 ; i < m_layer->numCameras() ; ++i)
                {
                    Ogre::Matrix4 projMat = m_layer->getCameraProjMat(i);

                    projMat[1][0] = -projMat[1][0];
                    projMat[1][1] = -projMat[1][1];
                    projMat[1][2] = -projMat[1][2];
                    projMat[1][3] = -projMat[1][3];

                    const auto& sharedParameterMap = gpuProgramMgr.getAvailableSharedParameters();
                    {
                        const std::string projParamName = "ProjectionMatrixParam/" + std::to_string(i);
                        auto it                         = sharedParameterMap.find(projParamName);
                        if(it != sharedParameterMap.end())
                        {
                            it->second->setNamedConstant("u_proj", projMat);
                        }
                    }
                    {
                        const std::string projParamName = "InverseProjectionMatrixParam/" + std::to_string(i);
                        auto it                         = sharedParameterMap.find(projParamName);
                        if(it != sharedParameterMap.end())
                        {
                            it->second->setNamedConstant("u_invProj", projMat.inverse());
                        }
                    }
                }

                m_frameId = frameId;
            }
        }
    }

    //------------------------------------------------------------------------------
};

Layer::Layer()
{
    newSignal<InitLayerSignalType>(s_INIT_LAYER_SIG);
    newSignal<ResizeLayerSignalType>(s_RESIZE_LAYER_SIG);
    newSignal<CameraUpdatedSignalType>(s_CAMERA_RANGE_UPDATED_SIG);

    newSlot(s_INTERACTION_SLOT, &Layer::interaction, this);
    newSlot(s_RESET_CAMERA_SLOT, &Layer::resetCameraCoordinates, this);
}

//-----------------------------------------------------------------------------

Ogre::RenderTarget* Layer::getRenderTarget() const
{
    return m_renderTarget;
}

//-----------------------------------------------------------------------------

void Layer::setRenderTarget(Ogre::RenderTarget* _renderTarget)
{
    m_renderTarget = _renderTarget;
}

//-----------------------------------------------------------------------------

void Layer::setID(const std::string& id)
{
    auto renderService = m_renderService.lock();
    SIGHT_ASSERT("Render service must be set before calling setID().", renderService);
    m_id = id;
}

//-----------------------------------------------------------------------------

std::string Layer::getName() const
{
    return m_sceneManager->getName();
}

//-----------------------------------------------------------------------------

const std::string& Layer::getLayerID() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

Ogre::SceneManager* Layer::getSceneManager() const
{
    return m_sceneManager;
}

//-----------------------------------------------------------------------------

void Layer::createScene()
{
    if(initialized())
    {
        return;
    }

    namespace fwc = viz::scene3d::compositor;
    {
        auto renderService = m_renderService.lock();
        auto* root         = viz::scene3d::Utils::getOgreRoot();
        m_sceneManager = root->createSceneManager("DefaultSceneManager", renderService->getID() + "_" + m_id);
    }

    SIGHT_ASSERT("Scene manager must be initialized", m_sceneManager);
    SIGHT_ASSERT("Render window must be initialized", m_renderTarget);

    m_sceneManager->setAmbientLight(Ogre::ColourValue(0.8F, 0.8F, 0.8F));

    // Create the camera
    m_camera = m_sceneManager->createCamera(Layer::s_DEFAULT_CAMERA_NAME);
    m_camera->setNearClipDistance(1);

    const auto& [left, top, width, height] = m_viewportCfg;
    auto* viewport = m_renderTarget->addViewport(m_camera, m_order, left, top, width, height);
    SIGHT_ASSERT("Could not create a viewport", viewport);

    m_compositorChainManager = std::make_unique<fwc::ChainManager>(this->getSptr());

    if(m_order != 0)
    {
        viewport->setClearEveryFrame(true, Ogre::FBT_DEPTH);
    } // Set the background material
    else
    {
        // FIXME : background isn't shown when using compositor with a clear pass
        // We must blend the input previous in each compositor
        Ogre::MaterialPtr defaultMaterial = Ogre::MaterialManager::getSingleton().getByName(
            "Background",
            RESOURCE_GROUP
        );
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
            this->getName() + "backgroundMat",
            RESOURCE_GROUP
        );
        defaultMaterial.get()->copyDetailsTo(material);

        std::array<std::uint8_t, 4> color {};
        Ogre::Pass* pass                           = material->getTechnique(0)->getPass(0);
        Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

        data::tools::Color::hexaStringToRGBA(m_topColor, color);
        Ogre::ColourValue ogreTopColor(static_cast<float>(color[0]) / 255.F,
                                       static_cast<float>(color[1]) / 255.F,
                                       static_cast<float>(color[2]) / 255.F,
                                       1.F);
        params->setNamedConstant("topColour", ogreTopColor);

        data::tools::Color::hexaStringToRGBA(m_bottomColor, color);
        Ogre::ColourValue ogreBotColor(static_cast<float>(color[0]) / 255.F,
                                       static_cast<float>(color[1]) / 255.F,
                                       static_cast<float>(color[2]) / 255.F,
                                       1.F);
        params->setNamedConstant("bottomColour", ogreBotColor);

        params->setNamedConstant("topScale", m_topScale);
        params->setNamedConstant("bottomScale", m_bottomScale);

        // Create background rectangle covering the whole screen
        auto* rect = new Ogre::Rectangle2D();
        rect->setCorners(-1.0, 1.0, 1.0, -1.0);
        rect->setMaterial(material);

        // Render the background before everything else
        rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

        // Use infinite AAB to always stay visible
        Ogre::AxisAlignedBox aabInf;
        aabInf.setInfinite();
        rect->setBoundingBox(aabInf);

        // Attach background to the scene
        Ogre::SceneNode* node = m_sceneManager->getRootSceneNode()->createChildSceneNode("Background");
        node->attachObject(rect);
    }

    // Alter the camera aspect ratio to match the viewport
    m_camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    // Creating Camera Scene Node
    Ogre::SceneNode* cameraNode = m_sceneManager->getRootSceneNode()->createChildSceneNode(
        Layer::s_DEFAULT_CAMERA_NODE_NAME
    );
    cameraNode->setPosition(Ogre::Vector3(0, 0, 5));
    cameraNode->lookAt(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_WORLD);

    cameraNode->attachObject(m_camera);

    auto renderService = m_renderService.lock();
    if(m_hasDefaultLight)
    {
        m_defaultLightDiffuseColor  = data::Color::New();
        m_defaultLightSpecularColor = data::Color::New();

        m_lightAdaptor = viz::scene3d::ILight::createLightAdaptor(
            m_defaultLightDiffuseColor,
            m_defaultLightSpecularColor
        );
        m_lightAdaptor->setName(Layer::s_DEFAULT_LIGHT_NAME);
        m_lightAdaptor->setType(Ogre::Light::LT_DIRECTIONAL);
        m_lightAdaptor->setTransformId(cameraNode->getName());
        m_lightAdaptor->setLayerID(this->getLayerID());
        m_lightAdaptor->setRenderService(renderService);
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
        std::vector<fwc::ChainManager::CompositorIdType> compositorChain;

        for(const auto& it : tok)
        {
            compositorChain.push_back(it);
        }

        if(m_stereoMode != compositor::Core::StereoModeType::NONE)
        {
            m_autostereoListener = new compositor::listener::AutoStereoCompositorListener(this->numCameras());
            Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
        }

        m_compositorChainManager->setCompositorChain(compositorChain);
    }

    m_sceneCreated = true;

    this->signal<InitLayerSignalType>(s_INIT_LAYER_SIG)->asyncEmit(this->getSptr());
}

// ----------------------------------------------------------------------------

void Layer::destroyScene()
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
        viz::scene3d::ILight::destroyLightAdaptor(m_lightAdaptor);
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
        viz::scene3d::Utils::getOgreRoot()->destroySceneManager(m_sceneManager);
        m_sceneManager = nullptr;
    }

    m_compositorChainManager = nullptr;

    m_camera = nullptr;
}

// ----------------------------------------------------------------------------

void Layer::addAvailableCompositor(std::string compositorName)
{
    m_compositorChainManager->addAvailableCompositor(compositorName);
}

// ----------------------------------------------------------------------------

void Layer::updateCompositorState(std::string compositorName, bool isEnabled)
{
    auto renderService = m_renderService.lock();

    renderService->makeCurrent();
    m_compositorChainManager->updateCompositorState(compositorName, isEnabled);
    renderService->requestRender();
}

// ----------------------------------------------------------------------------

void Layer::forAllInteractors(const std::function<void(const interactor::IInteractor::sptr&)>&& _f)
{
    const auto interactorsBegin = m_interactors.begin();
    const auto interactorsEnd   = m_interactors.end();

    for(auto it = interactorsBegin ; it != interactorsEnd && !m_cancelFurtherInteraction ; ++it)
    {
        const interactor::IInteractor::sptr interactor = it->second.lock();
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

void Layer::interaction(viz::scene3d::IWindowInteractor::InteractionInfo info)
{
    this->getRenderService()->makeCurrent();

    switch(info.interactionType)
    {
        case viz::scene3d::IWindowInteractor::InteractionInfo::MOUSEMOVE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->mouseMoveEvent(info.button, info.modifiers, info.x, info.y, info.dx, info.dy);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::WHEELMOVE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->wheelEvent(info.modifiers, info.delta, info.x, info.y);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::RESIZE:
        {
            auto sig = this->signal<ResizeLayerSignalType>(s_RESIZE_LAYER_SIG);
            sig->asyncEmit(info.x, info.y);

            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
                {
                    _i->resizeEvent(info.x, info.y);
                });
            break;
        }

        case viz::scene3d::IWindowInteractor::InteractionInfo::KEYPRESS:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->keyPressEvent(info.key, info.modifiers, info.x, info.y);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::KEYRELEASE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->keyReleaseEvent(info.key, info.modifiers, info.x, info.y);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::BUTTONRELEASE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->buttonReleaseEvent(info.button, info.modifiers, info.x, info.y);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::BUTTONPRESS:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->buttonPressEvent(info.button, info.modifiers, info.x, info.y);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::BUTTONDOUBLEPRESS:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->buttonDoublePressEvent(info.button, info.modifiers, info.x, info.y);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::PINCH_GESTURE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->pinchGestureEvent(info.delta, info.x, info.y);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::PAN_GESTURE_MOVE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->panGestureMoveEvent(info.x, info.y, info.dx, info.dy);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::PAN_GESTURE_RELEASE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->panGestureReleaseEvent(info.x, info.y, info.dx, info.dy);
            });
            break;

        case viz::scene3d::IWindowInteractor::InteractionInfo::LONG_TAP_GESTURE:
            this->forAllInteractors(
                [&info](const interactor::IInteractor::sptr& _i)
            {
                _i->longTapGestureEvent(info.x, info.y);
            });
            break;
    }

    m_cancelFurtherInteraction = false;
}

// ----------------------------------------------------------------------------

int Layer::getOrder() const
{
    return m_order;
}

// ----------------------------------------------------------------------------

void Layer::setOrder(int _order)
{
    m_order = _order;
}

// ----------------------------------------------------------------------------

void Layer::setWorker(const core::thread::Worker::sptr& _worker)
{
    core::com::HasSlots::m_slots.setWorker(_worker);
}

// ----------------------------------------------------------------------------

viz::scene3d::SRender::sptr Layer::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

void Layer::setRenderService(const viz::scene3d::SRender::sptr& _service)
{
    SIGHT_ASSERT("service not instanced", _service);

    m_renderService = _service;
}

// ----------------------------------------------------------------------------

void Layer::addInteractor(const viz::scene3d::interactor::IInteractor::sptr& _interactor, int _priority)
{
    using PairType = typename decltype(m_interactors)::value_type;
    const PairType pair = std::make_pair(_priority, _interactor);

    const auto isPair = [&pair](const PairType& _p)
                        {
                            return pair.second.lock() == _p.second.lock();
                        };

    // Add the interactor if it doesn't already exit.
    if(std::none_of(m_interactors.begin(), m_interactors.end(), isPair))
    {
        m_interactors.insert(pair);
    }
}

// ----------------------------------------------------------------------------

void Layer::removeInteractor(const viz::scene3d::interactor::IInteractor::sptr& _interactor)
{
    const auto interactorEqual = [&_interactor](typename decltype(m_interactors)::value_type _i)
                                 {
                                     return _i.second.lock() == _interactor;
                                 };

    const auto it = std::find_if(m_interactors.cbegin(), m_interactors.cend(), interactorEqual);

    if(it != m_interactors.end())
    {
        m_interactors.erase(it);
    }
}

//-----------------------------------------------------------------------------

Ogre::AxisAlignedBox Layer::computeWorldBoundingBox() const
{
    // The bounding box in which all the object's bounding boxes will be merged
    Ogre::AxisAlignedBox worldCoordBoundingBox;

    // Getting this render service scene manager
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();

    // Needed to recompute world bounding boxes
    rootSceneNode->_update(true, false);

    // This stack will be used for iterate through the scene nodes graph
    std::stack<const Ogre::SceneNode*> childrenStack;
    childrenStack.push(rootSceneNode);

    while(!childrenStack.empty())
    {
        const Ogre::SceneNode* tempSceneNode = childrenStack.top();
        childrenStack.pop();

        // Retrieves an iterator pointing to the attached movable objects of the current scene node
        const Ogre::SceneNode::ObjectMap& entities = tempSceneNode->getAttachedObjects();
        for(auto* const movable : entities)
        {
            const Ogre::Entity* entity = dynamic_cast<Ogre::Entity*>(movable);

            if(entity != nullptr)
            {
                if(entity->isVisible())
                {
                    worldCoordBoundingBox.merge(entity->getWorldBoundingBox());
                }
            }
            else
            {
                // Then try to cast into a ManualObject*
                const Ogre::ManualObject* manualObject = dynamic_cast<Ogre::ManualObject*>(movable);

                if(manualObject != nullptr)
                {
                    if(manualObject->isVisible())
                    {
                        worldCoordBoundingBox.merge(manualObject->getWorldBoundingBox());
                    }
                }
                else
                {
                    // Last try to cast into a Camera*
                    const Ogre::Camera* cameraObject = dynamic_cast<Ogre::Camera*>(movable);

                    if((cameraObject != nullptr) && cameraObject != this->getDefaultCamera())
                    {
                        if(cameraObject->isDebugDisplayEnabled())
                        {
                            worldCoordBoundingBox.merge(cameraObject->getWorldBoundingBox());
                        }
                    }
                }
            }
        }

        for(auto* const childNode : tempSceneNode->getChildren())
        {
            // First, we must cast the Node* into a SceneNode*
            const Ogre::SceneNode* childSceneNode = dynamic_cast<Ogre::SceneNode*>(childNode);
            if(childSceneNode != nullptr)
            {
                // Push the current node into the stack in order to continue iteration
                childrenStack.push(childSceneNode);
            }
        }
    }

    return worldCoordBoundingBox;
}

//------------------------------------------------------------------------------

compositor::transparencyTechnique Layer::getTransparencyTechnique()
{
    if(m_coreCompositor)
    {
        return m_coreCompositor->getTransparencyTechnique();
    }

    return compositor::DEFAULT;
}

//------------------------------------------------------------------------------

int Layer::getTransparencyDepth()
{
    if(m_coreCompositor)
    {
        return m_coreCompositor->getTransparencyDepth();
    }

    return 0;
}

//------------------------------------------------------------------------------

bool Layer::setTransparencyTechnique(compositor::transparencyTechnique _technique)
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

void Layer::setTransparencyDepth(int _depth)
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

void Layer::setCameraCalibrations(const CameraCalibrationsType& calibrations)
{
    m_stereoCameraCalibration = calibrations;
}

//-----------------------------------------------------------------------------

bool Layer::isDefaultLight(const viz::scene3d::ILight::csptr& _light) const
{
    return m_lightAdaptor == _light;
}

//-----------------------------------------------------------------------------

void Layer::removeDefaultLight()
{
    SIGHT_ASSERT("m_lightAdaptor must not be null", m_lightAdaptor != nullptr);
    viz::scene3d::ILight::destroyLightAdaptor(m_lightAdaptor);
    m_lightAdaptor.reset();
}

//-----------------------------------------------------------------------------

void Layer::resetCameraCoordinates()
{
    const Ogre::AxisAlignedBox worldCoordBoundingBox = this->computeWorldBoundingBox();

    if((m_camera != nullptr))
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if(worldCoordBoundingBox == Ogre::AxisAlignedBox::EXTENT_NULL
           || worldCoordBoundingBox == Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            Ogre::SceneNode* camNode = m_camera->getParentSceneNode();
            camNode->setPosition(0.F, 0.F, 0.F);
        }
        else
        {
            // Arbitrary coefficient
            const Ogre::Real boundingBoxLength = worldCoordBoundingBox.getSize().length() > 0
                                                 ? worldCoordBoundingBox.getSize().length() : 0;

            auto coeffZoom = static_cast<float>(boundingBoxLength);
            SIGHT_DEBUG("Zoom coefficient : " << coeffZoom);

            // Set the direction of the camera
            Ogre::SceneNode* camNode      = m_camera->getParentSceneNode();
            const Ogre::Quaternion quat   = camNode->getOrientation();
            const Ogre::Vector3 direction = quat.zAxis();

            // Set the position of the camera
            camNode->setPosition((worldCoordBoundingBox.getCenter() + coeffZoom * direction));

            // Update interactor's mouse scale
            this->forAllInteractors(
                [coeffZoom](const interactor::IInteractor::sptr& _i)
                {
                    _i->setSceneLength(coeffZoom);
                });

            this->resetCameraClippingRange(worldCoordBoundingBox);
        }

        m_renderService.lock()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void Layer::computeCameraParameters()
{
    const Ogre::AxisAlignedBox worldCoordBoundingBox = this->computeWorldBoundingBox();

    if((m_camera != nullptr))
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if(worldCoordBoundingBox != Ogre::AxisAlignedBox::EXTENT_NULL
           && worldCoordBoundingBox != Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            Ogre::SceneNode* camNode      = m_camera->getParentSceneNode();
            const Ogre::Quaternion quat   = camNode->getOrientation();
            const Ogre::Vector3 direction = quat.zAxis();
            const Ogre::Vector3 position  = camNode->getPosition();

            const Ogre::Vector3 div = (position - worldCoordBoundingBox.getCenter()) / direction;
            const float distance    = div.z;

            // Update interactor's mouse scale
            this->forAllInteractors(
                [distance](const interactor::IInteractor::sptr& _i)
                {
                    _i->setSceneLength(distance);
                });

            this->resetCameraClippingRange(worldCoordBoundingBox);
        }
    }
}

//-----------------------------------------------------------------------------

void Layer::resetCameraClippingRange() const
{
    this->resetCameraClippingRange(this->computeWorldBoundingBox());
}

//-----------------------------------------------------------------------------

void Layer::resetCameraClippingRange(const Ogre::AxisAlignedBox& worldCoordBoundingBox) const
{
    if((m_camera != nullptr))
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if(worldCoordBoundingBox == Ogre::AxisAlignedBox::EXTENT_NULL
           || worldCoordBoundingBox == Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            return;
        }

        Ogre::SceneNode* camNode = m_camera->getParentSceneNode();

        // Set the direction of the camera
        Ogre::Quaternion quat   = camNode->getOrientation();
        Ogre::Vector3 direction = quat.zAxis();
        Ogre::Vector3 position  = camNode->getPosition();

        // Set near and far plan
        Ogre::Vector3 minimum = worldCoordBoundingBox.getMinimum();
        Ogre::Vector3 maximum = worldCoordBoundingBox.getMaximum();

        const float a = -direction.x;
        const float b = -direction.y;
        const float c = -direction.z;
        const float d = -(a * position.x + b * position.y + c * position.z);

        // Max near and min far
        float maxNear = a * minimum.x + b * minimum.y + c * minimum.z + d;
        float minFar  = 1e-18F;

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
                    maxNear = std::min(dist, maxNear);
                    minFar  = std::max(dist, minFar);
                }
            }
        }

        // Make sure near is not bigger than far
        auto near = std::min(minFar, maxNear);
        auto far  = std::max(minFar, maxNear);

        // Give ourselves a little breathing room
        auto range = std::max((far - near) * 0.25F, 2.F);
        far  = far + range;
        near = near - range;

        // Do not let the range behind the camera throw off the calculation.
        near = std::max(near, 0.1F);

        const auto& chain          = this->getCompositorChain();
        const auto saoCompositorIt = std::find_if(
            chain.begin(),
            chain.end(),
            [](const auto& _compositor)
            {
                return _compositor.first == "SAO";
            });

        const auto prevFar = m_camera->getFarClipDistance();
        if(saoCompositorIt != chain.end() && saoCompositorIt->second)
        {
            // Near and far for SAO
            far = 10000;
        }

        if(m_cameraOrthographic && m_camera->getProjectionType() != Ogre::PT_PERSPECTIVE)
        {
            // Use height as the difference on y coordinates.
            const auto y1     = worldCoordBoundingBox.getMinimum().y;
            const auto y2     = worldCoordBoundingBox.getMaximum().y;
            Ogre::Real h      = y2 - y1;
            Ogre::Real margin = 0.1F;
            m_camera->setOrthoWindowHeight(h + h * margin);
        }

        // Keep the near plane close all the time. We need this for instance to display the shape extruder tool
        m_camera->setNearClipDistance(near);
        m_camera->setFarClipDistance(far);

        if(far != prevFar)
        {
            this->signal<CameraUpdatedSignalType>(s_CAMERA_RANGE_UPDATED_SIG)->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

void Layer::requestRender()
{
    m_renderService.lock()->requestRender();
}

//-----------------------------------------------------------------------------

void Layer::setStereoMode(compositor::Core::StereoModeType mode)
{
    // Disable the old compositor
    if(m_stereoMode != compositor::Core::StereoModeType::NONE)
    {
        Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
        m_autostereoListener = nullptr;
    }

    // Enable the new one
    m_stereoMode = mode;

    this->getRenderService()->makeCurrent();
    m_coreCompositor->setStereoMode(mode);
    m_coreCompositor->update();

    if(m_stereoMode != compositor::Core::StereoModeType::NONE)
    {
        m_autostereoListener = new compositor::listener::AutoStereoCompositorListener(this->numCameras());
        Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
    }

    this->restartAdaptors();
}

//-----------------------------------------------------------------------------

void Layer::setBackgroundColor(std::string topColor, std::string botColor)
{
    m_topColor    = topColor;
    m_bottomColor = botColor;
}

//-----------------------------------------------------------------------------

void Layer::setBackgroundScale(float topScale, float botScale)
{
    m_topScale    = topScale;
    m_bottomScale = botScale;
}

//-------------------------------------------------------------------------------------

void Layer::setCoreCompositorEnabled(
    bool enabled,
    std::string transparencyTechnique,
    std::string numPeels,
    compositor::Core::StereoModeType stereoMode
)
{
    m_hasCoreCompositor = enabled;
    m_stereoMode        = stereoMode;
    if(!transparencyTechnique.empty())
    {
        if(transparencyTechnique == "DepthPeeling")
        {
            m_transparencyTechnique = viz::scene3d::compositor::DEPTHPEELING;
        }
        else if(transparencyTechnique == "CelShadingDepthPeeling")
        {
            m_transparencyTechnique = viz::scene3d::compositor::CELLSHADING_DEPTHPEELING;
        }
        else if(transparencyTechnique == "DualDepthPeeling")
        {
            m_transparencyTechnique = viz::scene3d::compositor::DUALDEPTHPEELING;
        }
        else if(transparencyTechnique == "WeightedBlended")
        {
            m_transparencyTechnique = viz::scene3d::compositor::WEIGHTEDBLENDEDOIT;
        }
        else if(transparencyTechnique == "HybridTransparency" || transparencyTechnique == "Default")
        {
            m_transparencyTechnique = viz::scene3d::compositor::HYBRIDTRANSPARENCY;
        }
        else
        {
            SIGHT_ERROR("Unknown transparency technique : " << transparencyTechnique);
        }
    }

    if(!numPeels.empty())
    {
        m_numPeels = std::stoi(numPeels);
    }
}

//-------------------------------------------------------------------------------------

void Layer::setCompositorChainEnabled(const std::string& compositorChain)
{
    m_hasCompositorChain = !compositorChain.empty();

    if(m_hasCompositorChain)
    {
        m_rawCompositorChain = compositorChain;
    }
}

//-------------------------------------------------------------------------------------

void Layer::setViewportConfig(const ViewportConfigType& _vpCfg)
{
    m_viewportCfg = _vpCfg;
}

//-------------------------------------------------------------------------------------

bool Layer::isCoreCompositorEnabled() const
{
    return m_hasCoreCompositor;
}

//-------------------------------------------------------------------------------------

bool Layer::isCompositorChainEnabled() const
{
    return m_hasCompositorChain;
}

//-------------------------------------------------------------------------------------

bool Layer::initialized() const
{
    return m_sceneManager != nullptr;
}

//-------------------------------------------------------------------------------------

compositor::Core::StereoModeType Layer::getStereoMode() const
{
    return m_stereoMode;
}

//-------------------------------------------------------------------------------------

void Layer::setupCore()
{
    // Needed to setup compositors in GL3Plus, Ogre creates render targets
    m_renderService.lock()->makeCurrent();

    m_coreCompositor = std::make_shared<viz::scene3d::compositor::Core>(this->getViewport());
    m_coreCompositor->setStereoMode(m_stereoMode);
    m_coreCompositor->setTransparencyTechnique(m_transparencyTechnique);
    m_coreCompositor->setTransparencyDepth(m_numPeels);
    m_coreCompositor->update();
}

//-------------------------------------------------------------------------------------

void Layer::restartAdaptors()
{
    auto adaptors = this->getRenderService()->getAdaptors<IAdaptor>();

    auto notInLayer = [this](const IAdaptor::sptr& _adapt)
                      {
                          return _adapt->getLayerID() != this->m_id || _adapt == nullptr;
                      };

    // Only keep adaptors belonging to this layer.
    std::erase_if(adaptors, notInLayer);

    // Search for all adaptors created as subservices by other adaptors.
    std::vector<service::IService::wptr> subAdaptors;
    for(auto& adapt : adaptors)
    {
        SPTR(service::IHasServices) hasServices = std::dynamic_pointer_cast<service::IHasServices>(adapt);
        if(hasServices != nullptr)
        {
            const auto& subServices = hasServices->getRegisteredServices();
            std::copy(subServices.begin(), subServices.end(), std::back_inserter(subAdaptors));
        }
    }

    // Subadaptors should be started/stopped by their parent. Remove them from the list.
    for(const auto& subSrv : subAdaptors)
    {
        auto it = std::find(adaptors.begin(), adaptors.end(), subSrv.lock());

        if(it != adaptors.end())
        {
            adaptors.erase(it);
        }
    }

    // Restart all adaptors.
    for(auto& adapt : adaptors)
    {
        if(adapt->isStarted())
        {
            adapt->stop().wait();
            adapt->start().wait();
        }
    }
}

//-------------------------------------------------------------------------------------

viz::scene3d::compositor::ChainManager::CompositorChainType Layer::getCompositorChain() const
{
    return m_compositorChainManager->getCompositorChain();
}

//-------------------------------------------------------------------------------------

service::IHasServices::ServiceVector Layer::getRegisteredAdaptors() const
{
    if(m_compositorChainManager)
    {
        return m_compositorChainManager->getRegisteredServices();
    }

    return {};
}

//-------------------------------------------------------------------------------------

Ogre::Viewport* Layer::getViewport() const
{
    return m_renderTarget->getViewport(m_renderTarget->getNumViewports() - 1);
}

//-------------------------------------------------------------------------------------

bool Layer::isSceneCreated() const
{
    return m_sceneCreated;
}

//-------------------------------------------------------------------------------------

Ogre::Camera* Layer::getDefaultCamera() const
{
    return m_camera;
}

//-------------------------------------------------------------------------------------

Ogre::Matrix4 Layer::getCameraProjMat(const uint8_t cameraIdx) const
{
    SIGHT_ASSERT(
        "Index exceeds the number of cameras used for this stereo mode",
        cameraIdx < this->numCameras()
    );
    Ogre::Matrix4 extrinsicTransform(Ogre::Matrix4::IDENTITY);

    if(m_stereoMode == viz::scene3d::compositor::Core::StereoModeType::AUTOSTEREO_5)
    {
        const float eyeAngle = 0.02321F;
        const float angle    = eyeAngle * (-2.F + float(cameraIdx));

        extrinsicTransform = viz::scene3d::helper::Camera::computeFrustumShearTransform(*m_camera, angle);
    }
    else if(m_stereoMode == viz::scene3d::compositor::Core::StereoModeType::AUTOSTEREO_8)
    {
        const float eyeAngle = 0.01625F;
        const float angle    = eyeAngle * (-3.5F + float(cameraIdx));

        extrinsicTransform = viz::scene3d::helper::Camera::computeFrustumShearTransform(*m_camera, angle);
    }
    else if(m_stereoMode == viz::scene3d::compositor::Core::StereoModeType::STEREO)
    {
        if(m_stereoCameraCalibration.size() < 2 && cameraIdx == 1)
        {
            SIGHT_WARN("Only a single calibration was set but stereo rendering is set.");
            // Kept for compatibility purposes.
            extrinsicTransform = Ogre::Matrix4(
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
            return m_stereoCameraCalibration[cameraIdx];
        }
    }

    const Ogre::Matrix4 projMat = m_camera->getProjectionMatrixWithRSDepth();

    return projMat * extrinsicTransform;
}

//-------------------------------------------------------------------------------------

uint8_t Layer::numCameras() const
{
    return m_stereoMode == viz::scene3d::compositor::Core::StereoModeType::AUTOSTEREO_8 ? 8
                                                                                        : m_stereoMode
           == viz::scene3d::compositor::Core::StereoModeType::AUTOSTEREO_5 ? 5
                                                                           :
           m_stereoMode == viz::scene3d::compositor::Core::StereoModeType::STEREO ? 2 : 1;
}

//-------------------------------------------------------------------------------------

void Layer::setHasDefaultLight(bool hasDefaultLight)
{
    m_hasDefaultLight = hasDefaultLight;
}

//-------------------------------------------------------------------------------------

int Layer::getLightsNumber() const
{
    auto lightAdaptors = this->getRenderService()->getAdaptors<viz::scene3d::ILight>();
    int lightsNumber(0);

    std::ranges::for_each(
        lightAdaptors,
        [&](viz::scene3d::ILight::sptr adaptor)
        {
            if(adaptor->getLayerID() == this->getLayerID())
            {
                ++lightsNumber;
            }
        });

    return lightsNumber;
}

//-------------------------------------------------------------------------------------

std::vector<viz::scene3d::ILight::sptr> Layer::getLightAdaptors() const
{
    auto lightAdaptors = this->getRenderService()->getAdaptors<viz::scene3d::ILight>();
    std::vector<viz::scene3d::ILight::sptr> layerLightAdaptors;

    std::ranges::for_each(
        lightAdaptors,
        [&](viz::scene3d::ILight::sptr lightAdaptor)
        {
            if(lightAdaptor->getLayerID() == this->getLayerID())
            {
                layerLightAdaptors.push_back(lightAdaptor);
            }
        });

    return layerLightAdaptors;
}

//-----------------------------------------------------------------------------

void Layer::cancelFurtherInteraction()
{
    m_cancelFurtherInteraction = true;
}

//-----------------------------------------------------------------------------

void Layer::setOrthographicCamera(bool _ortho)
{
    m_cameraOrthographic = _ortho;
}

//-----------------------------------------------------------------------------

bool Layer::isOrthographicCameraForce() const
{
    return m_cameraOrthographic;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d.

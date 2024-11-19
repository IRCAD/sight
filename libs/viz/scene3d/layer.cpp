/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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
const core::com::slots::key_t layer::RESET_CAMERA_SLOT = "reset_camera";

//-----------------------------------------------------------------------------

const std::string layer::DEFAULT_CAMERA_NAME      = "DefaultCam";
const std::string layer::DEFAULT_LIGHT_NAME       = "DefaultLight";
const std::string layer::DEFAULT_CAMERA_NODE_NAME = "CameraNode";

//-----------------------------------------------------------------------------

struct layer::LayerCameraListener : public Ogre::Camera::Listener
{
    layer* m_layer {nullptr};
    int m_frame_id {0};

    //------------------------------------------------------------------------------

    explicit LayerCameraListener(layer* _renderer) :
        m_layer(_renderer)
    {
    }

    //------------------------------------------------------------------------------

    void cameraPreRenderScene(Ogre::Camera* /*cam*/) final
    {
        SIGHT_ASSERT("layer is not set", m_layer);

        if(m_layer->get_stereo_mode() != viz::scene3d::compositor::core::stereo_mode_t::none)
        {
            const int frame_id = m_layer->render_service()->get_interactor_manager()->get_frame_id();
            if(frame_id != m_frame_id)
            {
                auto& gpu_program_mgr = Ogre::GpuProgramManager::getSingleton();

                for(std::uint8_t i = 0 ; i < m_layer->num_cameras() ; ++i)
                {
                    Ogre::Matrix4 proj_mat = m_layer->get_camera_proj_mat(i);

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

                m_frame_id = frame_id;
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
    new_slot(RESET_CAMERA_SLOT, &layer::reset_camera_coordinates, this);
}

//-----------------------------------------------------------------------------

Ogre::RenderTarget* layer::get_render_target() const
{
    return m_render_target;
}

//-----------------------------------------------------------------------------

void layer::set_render_target(Ogre::RenderTarget* _render_target)
{
    m_render_target = _render_target;
}

//-----------------------------------------------------------------------------

void layer::set_id(const std::string& _id)
{
    auto render_service = m_render_service.lock();
    SIGHT_ASSERT("Render service must be set before calling set_id().", render_service);
    m_id = _id;
}

//-----------------------------------------------------------------------------

std::string layer::get_name() const
{
    return m_scene_manager->getName();
}

//-----------------------------------------------------------------------------

const std::string& layer::layer_id() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

Ogre::SceneManager* layer::get_scene_manager() const
{
    return m_scene_manager;
}

//-----------------------------------------------------------------------------

void layer::create_scene()
{
    if(initialized())
    {
        return;
    }

    namespace fwc = viz::scene3d::compositor;
    {
        auto render_service = m_render_service.lock();
        auto* root          = viz::scene3d::utils::get_ogre_root();
        m_scene_manager = root->createSceneManager("DefaultSceneManager", render_service->get_id() + "_" + m_id);
    }

    SIGHT_ASSERT("scene manager must be initialized", m_scene_manager);
    SIGHT_ASSERT("Render window must be initialized", m_render_target);

    m_scene_manager->setAmbientLight(Ogre::ColourValue(0.8F, 0.8F, 0.8F));

    // Create the camera
    m_camera = m_scene_manager->createCamera(layer::DEFAULT_CAMERA_NAME);
    m_camera->setNearClipDistance(1);

    const auto& [left, top, width, height] = m_viewport_cfg;
    auto* viewport = m_render_target->addViewport(m_camera, m_order, left, top, width, height);
    SIGHT_ASSERT("Could not create a viewport", viewport);

    m_compositor_chain_manager = std::make_unique<fwc::chain_manager>(this->get_sptr());

    if(m_order != 0)
    {
        viewport->setClearEveryFrame(true, Ogre::FBT_DEPTH);
    } // Set the background material
    else
    {
        // FIXME : background isn't shown when using compositor with a clear pass
        // We must blend the input previous in each compositor
        Ogre::MaterialPtr default_material = Ogre::MaterialManager::getSingleton().getByName(
            m_background_material.empty() ? "Background" : m_background_material,
            RESOURCE_GROUP
        );
        SIGHT_ASSERT("Could not find material ", default_material);
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
            this->get_name() + "backgroundMat",
            RESOURCE_GROUP
        );
        default_material.get()->copyDetailsTo(material);

        if(m_background_material.empty())
        {
            std::array<std::uint8_t, 4> color {};
            Ogre::Pass* pass                           = material->getTechnique(0)->getPass(0);
            Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

            data::tools::color::hexa_string_to_rgba(m_top_color, color);
            Ogre::ColourValue ogre_top_color(static_cast<float>(color[0]) / 255.F,
                                             static_cast<float>(color[1]) / 255.F,
                                             static_cast<float>(color[2]) / 255.F,
                                             1.F);
            params->setNamedConstant("topColour", ogre_top_color);

            data::tools::color::hexa_string_to_rgba(m_bottom_color, color);
            Ogre::ColourValue ogre_bot_color(static_cast<float>(color[0]) / 255.F,
                                             static_cast<float>(color[1]) / 255.F,
                                             static_cast<float>(color[2]) / 255.F,
                                             1.F);
            params->setNamedConstant("bottomColour", ogre_bot_color);

            params->setNamedConstant("topScale", m_top_scale);
            params->setNamedConstant("bottomScale", m_bottom_scale);
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
        Ogre::SceneNode* node = m_scene_manager->getRootSceneNode()->createChildSceneNode("Background");
        node->attachObject(rect);
    }

    // Alter the camera aspect ratio to match the viewport
    SIGHT_ASSERT(
        "Width and height should be strictly positive",
        viewport->getActualWidth() > 0 && viewport->getActualHeight() > 0
    );
    m_camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    // Creating Camera scene Node
    Ogre::SceneNode* camera_node = m_scene_manager->getRootSceneNode()->createChildSceneNode(
        layer::DEFAULT_CAMERA_NODE_NAME
    );
    camera_node->setPosition(Ogre::Vector3(0, 0, 5));
    camera_node->lookAt(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_WORLD);

    camera_node->attachObject(m_camera);

    auto render_service = m_render_service.lock();
    if(m_has_default_light)
    {
        m_default_light_diffuse_color  = std::make_shared<data::color>();
        m_default_light_specular_color = std::make_shared<data::color>();

        m_light_adaptor = viz::scene3d::light_adaptor::create_light_adaptor(
            m_default_light_diffuse_color,
            m_default_light_specular_color
        );
        service::config_t config;
        config.put("config.<xmlattr>.name", "default");
        m_light_adaptor->configure(config);
        m_light_adaptor->set_name(layer::DEFAULT_LIGHT_NAME);
        m_light_adaptor->set_type(Ogre::Light::LT_DIRECTIONAL);
        m_light_adaptor->set_transform_id(camera_node->getName());
        m_light_adaptor->set_layer_id(this->layer_id());
        m_light_adaptor->set_render_service(render_service);
        m_light_adaptor->start();
    }

    m_camera_listener = new LayerCameraListener(this);
    m_camera->addListener(m_camera_listener);

    // Setup transparency compositors
    if(m_has_core_compositor)
    {
        this->setup_core();
    }

    // Setup custom compositors and autostereo
    {
        boost::char_separator<char> sep(";");
        boost::tokenizer<boost::char_separator<char> > tok(m_raw_compositor_chain, sep);
        std::vector<fwc::chain_manager::compositor_id_t> compositor_chain;

        for(const auto& it : tok)
        {
            compositor_chain.push_back(it);
        }

        if(m_stereo_mode != compositor::core::stereo_mode_t::none)
        {
            m_autostereo_listener = new compositor::listener::auto_stereo_compositor_listener(this->num_cameras());
            Ogre::MaterialManager::getSingleton().addListener(m_autostereo_listener);
        }

        m_compositor_chain_manager->set_compositor_chain(compositor_chain);
    }

    m_scene_created = true;

    this->signal<init_layer_signal_t>(INIT_LAYER_SIG)->async_emit(this->get_sptr());
}

// ----------------------------------------------------------------------------

void layer::destroy_scene()
{
    if(m_order == 0)
    {
        // Remove the background material for the first layer
        Ogre::MaterialManager::getSingleton().remove(
            this->get_name() + "backgroundMat",
            viz::scene3d::RESOURCE_GROUP
        );
    }

    if(m_light_adaptor)
    {
        viz::scene3d::light_adaptor::destroy_light_adaptor(m_light_adaptor);
        m_light_adaptor.reset();
    }

    if((m_camera != nullptr) && (m_camera_listener != nullptr))
    {
        m_camera->removeListener(m_camera_listener);
        delete m_camera_listener;
        m_camera_listener = nullptr;
    }

    if(m_autostereo_listener != nullptr)
    {
        Ogre::MaterialManager::getSingleton().removeListener(m_autostereo_listener);
        delete m_autostereo_listener;
        m_autostereo_listener = nullptr;
    }

    if(m_scene_manager != nullptr)
    {
        viz::scene3d::utils::get_ogre_root()->destroySceneManager(m_scene_manager);
        m_scene_manager = nullptr;
    }

    m_compositor_chain_manager = nullptr;

    m_camera = nullptr;
}

// ----------------------------------------------------------------------------

void layer::add_available_compositor(std::string _compositor_name)
{
    m_compositor_chain_manager->add_available_compositor(_compositor_name);
}

// ----------------------------------------------------------------------------

void layer::update_compositor_state(std::string _compositor_name, bool _is_enabled)
{
    auto render_service = m_render_service.lock();

    render_service->make_current();
    m_compositor_chain_manager->update_compositor_state(_compositor_name, _is_enabled);
    render_service->request_render();
}

// ----------------------------------------------------------------------------

void layer::for_all_interactors(const std::function<void(const interactor::base::sptr&)>&& _f)
{
    const auto interactors_begin = m_interactors.begin();
    const auto interactors_end   = m_interactors.end();

    for(auto it = interactors_begin ; it != interactors_end && !m_cancel_further_interaction ; ++it)
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

void layer::interaction(viz::scene3d::window_interactor::interaction_info _info)
{
    this->render_service()->make_current();

    switch(_info.interaction_type)
    {
        case viz::scene3d::window_interactor::interaction_info::mousemove:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->mouse_move_event(_info.button, _info.modifiers, _info.x, _info.y, _info.dx, _info.dy);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::wheelmove:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->wheel_event(_info.modifiers, _info.delta, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::resize:
        {
            auto sig = this->signal<resize_layer_signal_t>(RESIZE_LAYER_SIG);
            sig->async_emit(_info.x, _info.y);

            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
                {
                    _i->resize_event(_info.x, _info.y);
                });
            break;
        }

        case viz::scene3d::window_interactor::interaction_info::keypress:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->key_press_event(_info.key, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::keyrelease:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->key_release_event(_info.key, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::buttonrelease:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->button_release_event(_info.button, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::buttonpress:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->button_press_event(_info.button, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::buttondoublepress:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->button_double_press_event(_info.button, _info.modifiers, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::pinch_gesture:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->pinch_gesture_event(_info.delta, _info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::pan_gesture_move:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->pan_gesture_move_event(_info.x, _info.y, _info.dx, _info.dy);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::pan_gesture_release:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->pan_gesture_release_event(_info.x, _info.y, _info.dx, _info.dy);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::long_tap_gesture:
            this->for_all_interactors(
                [&_info](const interactor::base::sptr& _i)
            {
                _i->long_tap_gesture_event(_info.x, _info.y);
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::leave:
            this->for_all_interactors(
                [](const interactor::base::sptr& _i)
            {
                _i->leave_event();
            });
            break;

        case viz::scene3d::window_interactor::interaction_info::enter:
            this->for_all_interactors(
                [](const interactor::base::sptr& _i)
            {
                _i->enter_event();
            });
            break;
    }

    m_cancel_further_interaction = false;
}

// ----------------------------------------------------------------------------

int layer::get_order() const
{
    return m_order;
}

// ----------------------------------------------------------------------------

void layer::set_order(int _order)
{
    m_order = _order;
}

// ----------------------------------------------------------------------------

void layer::set_worker(const core::thread::worker::sptr& _worker)
{
    core::com::has_slots::m_slots.set_worker(_worker);
}

// ----------------------------------------------------------------------------

viz::scene3d::render::sptr layer::render_service() const
{
    return m_render_service.lock();
}

//------------------------------------------------------------------------------

void layer::set_render_service(const viz::scene3d::render::sptr& _service)
{
    SIGHT_ASSERT("service not instanced", _service);

    m_render_service = _service;
}

// ----------------------------------------------------------------------------

void layer::add_interactor(const viz::scene3d::interactor::base::sptr& _interactor, int _priority)
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

void layer::remove_interactor(const viz::scene3d::interactor::base::sptr& _interactor)
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

Ogre::AxisAlignedBox layer::compute_world_bounding_box(bool _exclude_static) const
{
    // Getting this render service scene manager
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();

    // Needed to recompute world bounding boxes
    root_scene_node->_update(true, false);

    return helper::scene::compute_bounding_box(root_scene_node, _exclude_static);
}

//------------------------------------------------------------------------------

compositor::transparency_technique layer::get_transparency_technique()
{
    if(m_core_compositor)
    {
        return m_core_compositor->get_transparency_technique();
    }

    return compositor::DEFAULT;
}

//------------------------------------------------------------------------------

int layer::get_transparency_depth()
{
    if(m_core_compositor)
    {
        return m_core_compositor->get_transparency_depth();
    }

    return 0;
}

//------------------------------------------------------------------------------

bool layer::set_transparency_technique(compositor::transparency_technique _technique)
{
    bool success = false;
    if(m_core_compositor)
    {
        // Playing with the transparency may create new compositors, thus new resources in OpenGL, thus we
        // need to explicitly switch to our OpenGL context
        this->render_service()->make_current();
        success = m_core_compositor->set_transparency_technique(_technique);
        m_core_compositor->update();
        this->request_render();
    }

    return success;
}

//------------------------------------------------------------------------------

void layer::set_transparency_depth(int _depth)
{
    if(m_core_compositor)
    {
        // Playing with the transparency may create new compositors, thus new resources in OpenGL, thus we
        // need to explicitly switch to our OpenGL context
        this->render_service()->make_current();
        m_core_compositor->set_transparency_depth(_depth);
        m_core_compositor->update();
        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void layer::set_camera_calibrations(const camera_calibrations_t& _calibrations)
{
    m_stereo_camera_calibration = _calibrations;
}

//-----------------------------------------------------------------------------

bool layer::is_default_light(const viz::scene3d::light_adaptor::csptr& _light) const
{
    return m_light_adaptor == _light;
}

//-----------------------------------------------------------------------------

void layer::remove_default_light()
{
    SIGHT_ASSERT("m_lightAdaptor must not be null", m_light_adaptor != nullptr);
    viz::scene3d::light_adaptor::destroy_light_adaptor(m_light_adaptor);
    m_light_adaptor.reset();
}

//------------------------------------------------------------------------------

float layer::compute_scene_length(const Ogre::AxisAlignedBox& _world_bounding_box)
{
    // Arbitrary coefficient
    const Ogre::Real bounding_box_length = _world_bounding_box.getSize().length() > 0
                                           ? _world_bounding_box.getSize().length() : 0;

    SIGHT_DEBUG("Zoom coefficient : " << bounding_box_length);

    // Update interactor's mouse scale
    this->for_all_interactors(
        [bounding_box_length](const interactor::base::sptr& _i)
        {
            _i->set_scene_length(bounding_box_length);
        });

    return bounding_box_length;
}

//-----------------------------------------------------------------------------

void layer::reset_camera_coordinates()
{
    const Ogre::AxisAlignedBox world_bounding_box = this->compute_world_bounding_box(true);

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
            const float bounding_box_length = compute_scene_length(world_bounding_box);

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

void layer::compute_camera_parameters()
{
    const Ogre::AxisAlignedBox world_bounding_box = this->compute_world_bounding_box(true);

    if((m_camera != nullptr))
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if(world_bounding_box != Ogre::AxisAlignedBox::EXTENT_NULL
           && world_bounding_box != Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            compute_scene_length(world_bounding_box);
        }
    }
}

//-----------------------------------------------------------------------------

void layer::reset_camera_clipping_range() const
{
    auto world_bounding_box = this->compute_world_bounding_box();

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

        const auto& chain            = this->get_compositor_chain();
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

        if(m_camera_orthographic && m_camera->getProjectionType() != Ogre::PT_PERSPECTIVE)
        {
            // To compute the needed orthographic height,
            // one needs to apply the camera transformation on the world bounding box.
            auto transformed_bb = world_bounding_box;
            Ogre::Matrix3 camera_transform;

            // We inverse the orientation because it's being applied to an object in the world coordinate system.
            m_camera->getRealOrientation().Inverse().ToRotationMatrix(camera_transform);

            transformed_bb.transform(Ogre::Matrix4(camera_transform));

            // Use height as the difference on y coordinates.
            const auto y1     = transformed_bb.getMinimum().y;
            const auto y2     = transformed_bb.getMaximum().y;
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

void layer::request_render()
{
    m_render_service.lock()->request_render();
}

//-----------------------------------------------------------------------------

void layer::set_stereo_mode(compositor::core::stereo_mode_t _mode)
{
    // Disable the old compositor
    if(m_stereo_mode != compositor::core::stereo_mode_t::none)
    {
        Ogre::MaterialManager::getSingleton().removeListener(m_autostereo_listener);
        delete m_autostereo_listener;
        m_autostereo_listener = nullptr;
    }

    // Enable the new one
    m_stereo_mode = _mode;

    this->render_service()->make_current();
    m_core_compositor->set_stereo_mode(_mode);
    m_core_compositor->update();

    if(m_stereo_mode != compositor::core::stereo_mode_t::none)
    {
        m_autostereo_listener = new compositor::listener::auto_stereo_compositor_listener(this->num_cameras());
        Ogre::MaterialManager::getSingleton().addListener(m_autostereo_listener);
    }

    this->restart_adaptors();
}

//-----------------------------------------------------------------------------

void layer::set_background_color(std::string _top_color, std::string _bot_color)
{
    m_top_color    = _top_color;
    m_bottom_color = _bot_color;
}

//-----------------------------------------------------------------------------

void layer::set_background_scale(float _top_scale, float _bot_scale)
{
    m_top_scale    = _top_scale;
    m_bottom_scale = _bot_scale;
}

//-----------------------------------------------------------------------------

void layer::set_background_material(const std::string& _background)
{
    m_background_material = _background;
}

//-------------------------------------------------------------------------------------

void layer::set_core_compositor_enabled(
    bool _enabled,
    std::string _transparency_technique,
    std::string _num_peels,
    compositor::core::stereo_mode_t _stereo_mode
)
{
    m_has_core_compositor = _enabled;
    m_stereo_mode         = _stereo_mode;
    if(!_transparency_technique.empty())
    {
        if(_transparency_technique == "DepthPeeling")
        {
            m_transparency_technique = viz::scene3d::compositor::depthpeeling;
        }
        else if(_transparency_technique == "CelShadingDepthPeeling")
        {
            m_transparency_technique = viz::scene3d::compositor::cellshading_depthpeeling;
        }
        else if(_transparency_technique == "DualDepthPeeling")
        {
            m_transparency_technique = viz::scene3d::compositor::dualdepthpeeling;
        }
        else if(_transparency_technique == "WeightedBlended")
        {
            m_transparency_technique = viz::scene3d::compositor::weightedblendedoit;
        }
        else if(_transparency_technique == "HybridTransparency" || _transparency_technique == "Default")
        {
            m_transparency_technique = viz::scene3d::compositor::hybridtransparency;
        }
        else
        {
            SIGHT_ERROR("Unknown transparency technique : " << _transparency_technique);
        }
    }

    if(!_num_peels.empty())
    {
        m_num_peels = std::stoi(_num_peels);
    }
}

//-------------------------------------------------------------------------------------

void layer::set_compositor_chain_enabled(const std::string& _compositor_chain)
{
    m_has_compositor_chain = !_compositor_chain.empty();

    if(m_has_compositor_chain)
    {
        m_raw_compositor_chain = _compositor_chain;
    }
}

//-------------------------------------------------------------------------------------

void layer::set_viewport_config(const viewport_config_t& _vp_cfg)
{
    m_viewport_cfg = _vp_cfg;
}

//-------------------------------------------------------------------------------------

bool layer::is_core_compositor_enabled() const
{
    return m_has_core_compositor;
}

//-------------------------------------------------------------------------------------

bool layer::is_compositor_chain_enabled() const
{
    return m_has_compositor_chain;
}

//-------------------------------------------------------------------------------------

bool layer::initialized() const
{
    return m_scene_manager != nullptr;
}

//-------------------------------------------------------------------------------------

compositor::core::stereo_mode_t layer::get_stereo_mode() const
{
    return m_stereo_mode;
}

//-------------------------------------------------------------------------------------

void layer::setup_core()
{
    // Needed to setup compositors in GL3Plus, Ogre creates render targets
    m_render_service.lock()->make_current();

    m_core_compositor = std::make_shared<viz::scene3d::compositor::core>(this->get_viewport());
    m_core_compositor->set_stereo_mode(m_stereo_mode);
    m_core_compositor->set_transparency_technique(m_transparency_technique);
    m_core_compositor->set_transparency_depth(m_num_peels);
    m_core_compositor->update();
}

//-------------------------------------------------------------------------------------

void layer::restart_adaptors()
{
    auto adaptors = this->render_service()->get_adaptors<adaptor>();

    auto not_in_layer = [this](const adaptor::sptr& _adapt)
                        {
                            return _adapt->layer_id() != this->m_id || _adapt == nullptr;
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
            const auto& sub_services = has_services->get_registered_services();
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

viz::scene3d::compositor::chain_manager::compositor_chain_t layer::get_compositor_chain() const
{
    return m_compositor_chain_manager->get_compositor_chain();
}

//-------------------------------------------------------------------------------------

service::has_services::service_vector_t layer::get_registered_adaptors() const
{
    if(m_compositor_chain_manager)
    {
        return m_compositor_chain_manager->get_registered_services();
    }

    return {};
}

//-------------------------------------------------------------------------------------

Ogre::Viewport* layer::get_viewport() const
{
    return m_render_target->getViewport(m_render_target->getNumViewports() - 1);
}

//-------------------------------------------------------------------------------------

bool layer::is_scene_created() const
{
    return m_scene_created;
}

//-------------------------------------------------------------------------------------

Ogre::Camera* layer::get_default_camera() const
{
    return m_camera;
}

//-------------------------------------------------------------------------------------

Ogre::Matrix4 layer::get_camera_proj_mat(const uint8_t _camera_idx) const
{
    SIGHT_ASSERT(
        "Index exceeds the number of cameras used for this stereo mode",
        _camera_idx < this->num_cameras()
    );
    Ogre::Matrix4 extrinsic_transform(Ogre::Matrix4::IDENTITY);

    if(m_stereo_mode == viz::scene3d::compositor::core::stereo_mode_t::autostereo_5)
    {
        const float eye_angle = 0.02321F;
        const float angle     = eye_angle * (-2.F + float(_camera_idx));

        extrinsic_transform = viz::scene3d::helper::camera::compute_frustum_shear_transform(*m_camera, angle);
    }
    else if(m_stereo_mode == viz::scene3d::compositor::core::stereo_mode_t::autostereo_8)
    {
        const float eye_angle = 0.01625F;
        const float angle     = eye_angle * (-3.5F + float(_camera_idx));

        extrinsic_transform = viz::scene3d::helper::camera::compute_frustum_shear_transform(*m_camera, angle);
    }
    else if(m_stereo_mode == viz::scene3d::compositor::core::stereo_mode_t::stereo)
    {
        if(m_stereo_camera_calibration.size() < 2 && _camera_idx == 1)
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
        else if(!m_stereo_camera_calibration.empty())
        {
            return m_stereo_camera_calibration[_camera_idx];
        }
    }

    const Ogre::Matrix4 proj_mat = m_camera->getProjectionMatrixWithRSDepth();

    return proj_mat * extrinsic_transform;
}

//-------------------------------------------------------------------------------------

uint8_t layer::num_cameras() const
{
    return m_stereo_mode == viz::scene3d::compositor::core::stereo_mode_t::autostereo_8 ? 8
                                                                                        : m_stereo_mode
           == viz::scene3d::compositor::core::stereo_mode_t::autostereo_5 ? 5
                                                                          :
           m_stereo_mode == viz::scene3d::compositor::core::stereo_mode_t::stereo ? 2 : 1;
}

//-------------------------------------------------------------------------------------

void layer::set_has_default_light(bool _has_default_light)
{
    m_has_default_light = _has_default_light;
}

//-------------------------------------------------------------------------------------

int layer::num_lights() const
{
    auto light_adaptors = this->render_service()->get_adaptors<viz::scene3d::light_adaptor>();
    int lights_number(0);

    std::ranges::for_each(
        light_adaptors,
        [&](viz::scene3d::light_adaptor::sptr _adaptor)
        {
            if(_adaptor->layer_id() == this->layer_id())
            {
                ++lights_number;
            }
        });

    return lights_number;
}

//-------------------------------------------------------------------------------------

std::vector<viz::scene3d::light_adaptor::sptr> layer::get_light_adaptors() const
{
    auto light_adaptors = this->render_service()->get_adaptors<viz::scene3d::light_adaptor>();
    std::vector<viz::scene3d::light_adaptor::sptr> layer_light_adaptors;

    std::ranges::for_each(
        light_adaptors,
        [&](viz::scene3d::light_adaptor::sptr _light_adaptor)
        {
            if(_light_adaptor->layer_id() == this->layer_id())
            {
                layer_light_adaptors.push_back(_light_adaptor);
            }
        });

    return layer_light_adaptors;
}

//-----------------------------------------------------------------------------

void layer::cancel_further_interaction()
{
    m_cancel_further_interaction = true;
}

//-----------------------------------------------------------------------------

void layer::set_orthographic_camera(bool _ortho)
{
    m_camera_orthographic = _ortho;
}

//-----------------------------------------------------------------------------

bool layer::is_orthographic_camera_force() const
{
    return m_camera_orthographic;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d.

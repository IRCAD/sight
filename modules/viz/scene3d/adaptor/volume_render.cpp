/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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
    new_slot(NEW_IMAGE_SLOT, &volume_render::new_image, this);
    new_slot(BUFFER_IMAGE_SLOT, &volume_render::buffer_image, this);
    new_slot(UPDATE_IMAGE_SLOT, &volume_render::update_image, this);
    new_slot(TOGGLE_WIDGETS_SLOT, &volume_render::toggle_widgets, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &volume_render::set_bool_parameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &volume_render::set_int_parameter, this);
    new_slot(SET_DOUBLE_PARAMETER_SLOT, &volume_render::set_double_parameter, this);
    new_slot(UPDATE_CLIPPING_BOX_SLOT, &volume_render::update_clipping_box, this);
    new_slot(UPDATE_MASK_SLOT, &volume_render::update_mask, this);
    new_slot(UPDATE_TF_SLOT, &volume_render::update_volume_tf, this);
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
    this->configure_params();

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

    this->set_transform_id(
        _config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );
}

//-----------------------------------------------------------------------------

void volume_render::starting()
{
    this->initialize();

    auto render_service = this->render_service();
    render_service->make_current();

    //scene (node, manager)
    {
        m_scene_manager = this->get_scene_manager();

        Ogre::SceneNode* const root_scene_node = m_scene_manager->getRootSceneNode();
        Ogre::SceneNode* const transform_node  = this->get_or_create_transform_node(root_scene_node);
        m_volume_scene_node = transform_node->createChildSceneNode(this->get_id() + "_transform_origin");
    }

    //Renderer
    {
        sight::viz::scene3d::layer::sptr layer = render_service->layer(m_layer_id);

        const auto image = m_image.lock();
        const auto mask  = m_mask.lock();
        const auto tf    = m_tf.lock();
        m_volume_renderer = std::make_unique<sight::viz::scene3d::vr::ray_tracing_volume_renderer>(
            this->get_id(),
            layer,
            m_volume_scene_node,
            image.get_shared(),
            mask.get_shared(),
            tf.get_shared(),
            m_config.samples,
            m_config.dynamic,
            m_config.preintegration,
            m_config.shadows,
            m_config.sat
        );
        m_volume_renderer->update(tf.get_shared());
    }

    m_volume_scene_node->setVisible(m_visible);

    // Initially focus on the image center.
    this->set_focal_distance(50);

    bool is_valid = false;
    //image
    {
        const auto image = m_image.lock();
        is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
    }

    if(is_valid)
    {
        this->new_image();
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::updating()
{
}

//-----------------------------------------------------------------------------

void volume_render::stopping()
{
    this->render_service()->make_current();

    // First wait on all pending buffering tasks and destroy the worker.
    m_buffering_worker.reset();
    m_volume_renderer.reset();

    this->get_scene_manager()->destroySceneNode(m_volume_scene_node);

    auto* const transform_node = this->get_transform_node();

    if(transform_node != nullptr)
    {
        m_scene_manager->getRootSceneNode()->removeChild(transform_node);
        this->get_scene_manager()->destroySceneNode(static_cast<Ogre::SceneNode*>(transform_node));
    }

    this->destroy_widget();
}

//-----------------------------------------------------------------------------

void volume_render::update_volume_tf()
{
    this->render_service()->make_current();
    std::lock_guard swap_lock(m_mutex);

    {
        const auto tf = m_tf.lock();
        m_volume_renderer->update_volume_tf(tf.get_shared());
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::new_image()
{
    auto render_service = this->render_service();

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
            m_buffering_worker.reset();

            auto* new_worker = render_service->get_interactor_manager()->create_graphics_worker();
            m_buffering_worker = std::unique_ptr<sight::viz::scene3d::graphics_worker>(new_worker);
        }

        render_service->make_current();
        {
            const auto image = m_image.lock();
            m_volume_renderer->load_image();
        }
        this->update_mask();
        this->update_volume_tf();
    }

    this->update_image();
}

//-----------------------------------------------------------------------------

void volume_render::buffer_image()
{
    if(m_config.dynamic)
    {
        auto buffering_fn =
            [this]()
            {
                const auto image = m_image.lock();

                m_volume_renderer->load_image();

                // Switch back to the main thread to compute the proxy geometry.
                // Ogre can't handle parallel rendering.
                this->slot(UPDATE_IMAGE_SLOT)->async_run();
            };

        m_buffering_worker->push_task(buffering_fn);
    }
    else
    {
        this->render_service()->make_current();
        {
            const auto image = m_image.lock();
            m_volume_renderer->load_image();
        }
        this->update_image();
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_image()
{
    const auto image = m_image.lock();

    this->render_service()->make_current();

    {
        const auto volume_tf = m_tf.lock();
        m_volume_renderer->update_image(image.get_shared(), volume_tf.get_shared());
    }

    // Create widgets on image update to take the image's size into account.
    this->create_widget();

    if(m_config.camera_autoreset)
    {
        this->render_service()->reset_camera_coordinates(m_layer_id);
    }
    else
    {
        this->layer()->compute_camera_parameters();
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::update_mask()
{
    this->render_service()->make_current();

    {
        const auto mask = m_mask.lock();

        if(!data::helper::medical_image::check_image_validity(*mask))
        {
            return;
        }

        m_volume_renderer->update_clipping_box(mask.get_shared());
    }

    /// Load updated mask to the gpu
    /// @warning This locks the mask again, do it outside the scope
    m_volume_renderer->load_mask();

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::update_sampling(unsigned _nb_samples)
{
    this->render_service()->make_current();

    SIGHT_ASSERT("Sampling rate must fit in a 16 bit uint.", _nb_samples < 65536);

    const auto tf = m_tf.lock();
    m_volume_renderer->set_sampling(static_cast<std::uint16_t>(_nb_samples), tf.get_shared());

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::update_opacity_correction(unsigned _opacity_correction)
{
    m_volume_renderer->set_opacity_correction(int(_opacity_correction));
    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::update_ao_factor(float _ao_factor)
{
    if(m_volume_renderer->set_ao_factor(_ao_factor))
    {
        this->request_render(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_color_bleeding_factor(float _color_bleeding_factor)
{
    if(m_volume_renderer->set_color_bleeding_factor(_color_bleeding_factor))
    {
        this->request_render(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_sat_size_ratio(unsigned _size_ratio)
{
    if(m_volume_renderer->shadows().parameters.enabled())
    {
        this->render_service()->make_current();

        m_volume_renderer->update_sat_size_ratio(_size_ratio);

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_sat_shells_number(unsigned _shells_number)
{
    if(m_volume_renderer->shadows().parameters.enabled())
    {
        this->render_service()->make_current();

        m_volume_renderer->update_sat_shells_number(_shells_number);

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_sat_shell_radius(unsigned _shell_radius)
{
    if(m_volume_renderer->shadows().parameters.enabled())
    {
        this->render_service()->make_current();

        m_volume_renderer->update_sat_shell_radius(_shell_radius);

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_sat_cone_angle(float _cone_angle)
{
    if(m_volume_renderer->shadows().parameters.enabled())
    {
        this->render_service()->make_current();

        m_volume_renderer->update_sat_cone_angle(_cone_angle);

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_sat_cone_samples(unsigned _nb_cone_samples)
{
    if(m_volume_renderer->shadows().parameters.enabled())
    {
        this->render_service()->make_current();

        m_volume_renderer->update_sat_cone_samples(_nb_cone_samples);

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::toggle_preintegration(bool _preintegration)
{
    this->render_service()->make_current();

    m_volume_renderer->set_pre_integrated_rendering(_preintegration);

    if(_preintegration)
    {
        const auto image     = m_image.lock();
        const auto volume_tf = m_tf.lock();

        m_volume_renderer->update_image(image.get_shared(), volume_tf.get_shared());
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::toggle_ambient_occlusion(bool _ambient_occlusion)
{
    this->toggle_vr_effect(vr_effect_type::vr_ambient_occlusion, _ambient_occlusion);
}

//-----------------------------------------------------------------------------

void volume_render::toggle_color_bleeding(bool _color_bleeding)
{
    this->toggle_vr_effect(vr_effect_type::vr_color_bleeding, _color_bleeding);
}

//-----------------------------------------------------------------------------

void volume_render::toggle_shadows(bool _shadows)
{
    this->toggle_vr_effect(vr_effect_type::vr_shadows, _shadows);
}

//-----------------------------------------------------------------------------

void volume_render::toggle_widgets(bool _visible)
{
    m_config.visible = _visible;

    if(m_widget)
    {
        m_widget->set_box_visibility(m_config.visible && m_volume_renderer->is_visible());

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::set_focal_distance(int _focal_distance)
{
    if(this->render_service()->layer(m_layer_id)->get_stereo_mode()
       != sight::viz::scene3d::compositor::core::stereo_mode_t::none)
    {
        m_volume_renderer->set_focal_length(static_cast<float>(_focal_distance) / 100);
    }
}

//-----------------------------------------------------------------------------

void volume_render::set_bool_parameter(bool _val, std::string _key)
{
    SIGHT_ASSERT(
        "Invalid slot key " + _key,
        _key == "preIntegration"
        || _key == "ambientOcclusion"
        || _key == "colorBleeding"
        || _key == "shadows"
        || _key == "widgets"
    );

    this->render_service()->make_current();
    std::lock_guard swap_lock(m_mutex);

    if(_key == "preIntegration")
    {
        this->toggle_preintegration(_val);
    }
    else if(_key == "ambientOcclusion")
    {
        this->toggle_ambient_occlusion(_val);
    }
    else if(_key == "colorBleeding")
    {
        this->toggle_color_bleeding(_val);
    }
    else if(_key == "shadows")
    {
        this->toggle_shadows(_val);
    }
    else if(_key == "widgets")
    {
        this->toggle_widgets(_val);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::set_int_parameter(int _val, std::string _key)
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

    this->render_service()->make_current();
    std::lock_guard<std::mutex> swap_lock(m_mutex);

    const auto param = static_cast<unsigned>(_val);

    if(_key == "sampling")
    {
        this->update_sampling(param);
    }
    else if(_key == "opacityCorrection")
    {
        this->update_opacity_correction(param);
    }
    else if(_key == "satSizeRatio")
    {
        this->update_sat_size_ratio(param);
    }
    else if(_key == "satShellsNumber")
    {
        this->update_sat_shells_number(param);
    }
    else if(_key == "satShellRadius")
    {
        this->update_sat_shell_radius(param);
    }
    else if(_key == "satConeSamples")
    {
        this->update_sat_cone_samples(param);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::set_double_parameter(double _val, std::string _key)
{
    SIGHT_ASSERT(
        "Invalid slot key " + _key,
        _key == "colorBleedingFactor"
        || _key == "aoFactor"
        || _key == "satConeAngle"
    );

    this->render_service()->make_current();
    std::lock_guard swap_lock(m_mutex);

    const auto param = static_cast<float>(_val);

    if(_key == "colorBleedingFactor")
    {
        this->update_color_bleeding_factor(param);
    }
    else if(_key == "aoFactor")
    {
        this->update_ao_factor(param);
    }
    else if(_key == "satConeAngle")
    {
        this->update_sat_cone_angle(param);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::create_widget()
{
    auto clipping_mx_update = [this]{update_clipping_matrix();};

    Ogre::Matrix4 ogre_clipping_mx = Ogre::Matrix4::IDENTITY;

    const auto clipping_matrix = m_clipping_matrix.lock();
    if(clipping_matrix)
    {
        ogre_clipping_mx = sight::viz::scene3d::utils::to_ogre_matrix(clipping_matrix.get_shared());
    }

    const sight::viz::scene3d::layer::sptr layer = this->layer();

    this->destroy_widget(); // Destroys the old widgets if they were created.
    m_widget = std::make_shared<sight::viz::scene3d::interactor::clipping_box_interactor>(
        layer,
        m_config.order_dependent,
        this->get_id(),
        m_volume_scene_node,
        ogre_clipping_mx,
        clipping_mx_update,
        "BasicAmbient",
        "BasicPhong"
    );

    layer->add_interactor(m_widget, m_config.priority);

    m_volume_renderer->clip_image(m_widget->get_clipping_box());

    m_widget->set_box_visibility(m_config.visible && m_volume_renderer->is_visible());
}

//-----------------------------------------------------------------------------

void volume_render::destroy_widget()
{
    if(m_widget)
    {
        sight::viz::scene3d::layer::sptr layer = this->layer();
        layer->remove_interactor(m_widget);
        m_widget.reset();
    }
}

//-----------------------------------------------------------------------------

void volume_render::toggle_vr_effect(vr_effect_type _vr_effect, bool _enable)
{
    this->render_service()->make_current();

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
                case vr_effect_type::vr_ambient_occlusion:
                {
                    m_volume_renderer->toggle_ambient_occlusion(_enable);
                    break;
                }

                case vr_effect_type::vr_color_bleeding:
                {
                    m_volume_renderer->toggle_color_bleeding(_enable);
                    break;
                }

                case vr_effect_type::vr_shadows:
                {
                    m_volume_renderer->toggle_shadows(_enable);
                    break;
                }
            }
        }

        if(m_volume_renderer->preintegration())
        {
            const auto image     = m_image.lock();
            const auto volume_tf = m_tf.lock();

            m_volume_renderer->update_image(image.get_shared(), volume_tf.get_shared());
        }

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_clipping_box()
{
    if(m_widget)
    {
        bool matrix_set = false;
        Ogre::Matrix4 clipping_mx;
        {
            const auto clipping_matrix = m_clipping_matrix.lock();
            if(clipping_matrix)
            {
                clipping_mx = sight::viz::scene3d::utils::to_ogre_matrix(clipping_matrix.get_shared());
                matrix_set  = true;
            }
        }

        if(matrix_set)
        {
            this->render_service()->make_current();

            // updateFromTransform is called outside of the lock of the InOut data to prevent a deadlock
            m_widget->update_from_transform(clipping_mx);

            std::lock_guard<std::mutex> swap_lock(m_mutex);
            m_volume_renderer->clip_image(m_widget->get_clipping_box());
            this->request_render();
        }
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_clipping_matrix()
{
    auto clipping_matrix = m_clipping_matrix.lock();
    if(clipping_matrix)
    {
        sight::viz::scene3d::utils::from_ogre_matrix(m_widget->get_clipping_transform(), clipping_matrix.get_shared());

        const auto sig =
            clipping_matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);

        core::com::connection::blocker blocker(sig->get_connection(this->slot(UPDATE_CLIPPING_BOX_SLOT)));

        sig->async_emit();
    }

    std::lock_guard<std::mutex> swap_lock(m_mutex);
    m_volume_renderer->clip_image(m_widget->get_clipping_box());

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::set_visible(bool _visible)
{
    if(m_volume_scene_node != nullptr)
    {
        m_volume_scene_node->setVisible(_visible);

        if(m_widget)
        {
            m_widget->set_box_visibility(_visible && m_config.visible);
        }

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

/************************************************************************
 *
 * Copyright (C) 2016-2026 IRCAD France
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

#include "module/viz/scene3d/adaptor/volume_render.hpp"

#include <core/spy_log.hpp>
#include <data/helper/medical_image.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>

#include <memory>

//-----------------------------------------------------------------------------

namespace sight::module::viz::scene3d::adaptor
{

namespace
{

//-----------------------------------------------------------------------------

} // namespace

volume_render::volume_render() noexcept
{
    // Auto-connected slots
    new_slot(slots::NEW_IMAGE, [this](){lazy_update(update_flags::image);});
    new_slot(slots::BUFFER_IMAGE, [this](){lazy_update(update_flags::image_buffer);});
    new_slot(slots::UPDATE_MASK, [this](){lazy_update(update_flags::mask_buffer);});
    new_slot(slots::UPDATE_TF, [this](){lazy_update(update_flags::tf);});
    new_slot(slots::UPDATE_CLIPPING_BOX, [this](){lazy_update(update_flags::clipping_box);});

    new_slot(slots::TOGGLE_PREINTEGRATION, [this](){toggle_preintegration(*m_preintegration);});
    new_slot(slots::TOGGLE_AMBIENT_OCCLUSION, [this](){toggle_ambient_occlusion(*m_ambient_occlusion);});
    new_slot(slots::TOGGLE_COLOR_BLEEDING, [this](){toggle_color_bleeding(*m_color_bleeding);});
    new_slot(slots::TOGGLE_SHADOWS, [this](){toggle_shadows(*m_shadows);});
    new_slot(slots::TOGGLE_WIDGETS, [this](){toggle_widgets(*m_widgets);});
    new_slot(slots::UPDATE_SAMPLING, [this](){update_sampling(static_cast<unsigned>(*m_sampling));});
    new_slot(
        slots::UPDATE_OPACITY_CORRECTION,
        [this]()
        {
            update_opacity_correction(static_cast<unsigned>(*m_opacity_correction));
        });
    new_slot(
        slots::UPDATE_SAT_SHELLS_NUMBER,
        [this]()
        {
            update_sat_shells_number(static_cast<unsigned>(*m_sat_shells_number));
        });
    new_slot(
        slots::UPDATE_SAT_SHELL_RADIUS,
        [this]()
        {
            update_sat_shell_radius(static_cast<unsigned>(*m_sat_shell_radius));
        });
    new_slot(
        slots::UPDATE_SAT_CONE_SAMPLES,
        [this]()
        {
            update_sat_cone_samples(static_cast<unsigned>(*m_sat_cone_samples));
        });
    new_slot(
        slots::UPDATE_COLOR_BLEEDING_FACTOR,
        [this]()
        {
            update_color_bleeding_factor(static_cast<float>(*m_color_bleeding_factor));
        });
    new_slot(slots::UPDATE_AO_FACTOR, [this](){update_ao_factor(static_cast<float>(*m_ao_factor));});
    new_slot(slots::UPDATE_SAT_CONE_ANGLE, [this](){update_sat_cone_angle(static_cast<float>(*m_sat_cone_angle));});
    new_slot(
        slots::UPDATE_SAT_SIZE_RATIO,
        [this]()
        {
            update_sat_size_ratio(static_cast<unsigned int>(*m_sat_size_ratio));
        });

    // Slot for async update
    new_slot(slots::UPDATE_IMAGE, &volume_render::update_image, this);
}

//-----------------------------------------------------------------------------

service::connections_t volume_render::auto_connections() const
{
    service::connections_t connections = {
        {objects::IMAGE_IN, data::signals::MODIFIED, slots::NEW_IMAGE},
        {objects::IMAGE_IN, data::image::signals::BUFFER_MODIFIED, slots::BUFFER_IMAGE},
        {objects::MASK_IN, data::signals::MODIFIED, slots::NEW_IMAGE},
        {objects::MASK_IN, data::image::signals::BUFFER_MODIFIED, slots::UPDATE_MASK},
        {objects::CLIPPING_MATRIX_INOUT, data::signals::MODIFIED, slots::UPDATE_CLIPPING_BOX},
        {objects::VOLUME_TF_IN, data::signals::MODIFIED, slots::UPDATE_TF},
        {objects::VOLUME_TF_IN, data::transfer_function::signals::POINTS_MODIFIED, slots::UPDATE_TF},
        {objects::VOLUME_TF_IN, data::transfer_function::signals::WINDOWING_MODIFIED, slots::UPDATE_TF},
        {m_preintegration, data::signals::MODIFIED, slots::TOGGLE_PREINTEGRATION},
        {m_ambient_occlusion, data::signals::MODIFIED, slots::TOGGLE_AMBIENT_OCCLUSION},
        {m_color_bleeding, data::signals::MODIFIED, slots::TOGGLE_COLOR_BLEEDING},
        {m_shadows, data::signals::MODIFIED, slots::TOGGLE_SHADOWS},
        {m_widgets, data::signals::MODIFIED, slots::TOGGLE_WIDGETS},
        {m_sampling, data::signals::MODIFIED, slots::UPDATE_SAMPLING},
        {m_opacity_correction, data::signals::MODIFIED, slots::UPDATE_OPACITY_CORRECTION},
        {m_sat_shells_number, data::signals::MODIFIED, slots::UPDATE_SAT_SHELLS_NUMBER},
        {m_sat_shell_radius, data::signals::MODIFIED, slots::UPDATE_SAT_SHELL_RADIUS},
        {m_sat_cone_samples, data::signals::MODIFIED, slots::UPDATE_SAT_CONE_SAMPLES},
        {m_color_bleeding_factor, data::signals::MODIFIED, slots::UPDATE_COLOR_BLEEDING_FACTOR},
        {m_ao_factor, data::signals::MODIFIED, slots::UPDATE_AO_FACTOR},
        {m_sat_cone_angle, data::signals::MODIFIED, slots::UPDATE_SAT_CONE_ANGLE},
        {m_sat_size_ratio, data::signals::MODIFIED, slots::UPDATE_SAT_SIZE_RATIO},
    };

    return connections + adaptor::auto_connections();
}

//-----------------------------------------------------------------------------

void volume_render::configuring(const config_t& _config)
{
    this->configure_params();

    m_config.camera_autoreset = _config.get<bool>(config::AUTORESET_CAMERA, true);
    m_config.dynamic          = _config.get<bool>(config::DYNAMIC, false);
    m_config.priority         = _config.get<int>(config::PRIORITY, 2);

    this->set_transform_id(
        _config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );
}

//-----------------------------------------------------------------------------

void volume_render::starting()
{
    adaptor::init();

    auto render_service = this->render_service();
    render_service->make_current();

    config_data_t::sat_parameters_t sat {};
    sat.size_ratio = static_cast<unsigned int>(*m_sat_size_ratio);
    sat.shells     = static_cast<unsigned>(*m_sat_shells_number);
    sat.radius     = static_cast<unsigned>(*m_sat_shell_radius);
    sat.angle      = static_cast<float>(*m_sat_cone_angle);
    sat.samples    = static_cast<unsigned>(*m_sat_cone_samples);

    config_data_t::shadows_parameters_t shadows {};
    shadows.soft_shadows = *m_shadows;
    shadows.ao.enabled   = *m_ambient_occlusion;
    shadows.ao.factor    = static_cast<float>(*m_ao_factor);

    const auto color_bleeding_factor = static_cast<float>(*m_color_bleeding_factor);
    shadows.colour_bleeding.enabled = *m_color_bleeding;
    shadows.colour_bleeding.r       = color_bleeding_factor;
    shadows.colour_bleeding.g       = color_bleeding_factor;
    shadows.colour_bleeding.b       = color_bleeding_factor;

    //scene (node, manager)
    {
        m_scene_manager = this->get_scene_manager();

        Ogre::SceneNode* const root_scene_node = m_scene_manager->getRootSceneNode();
        Ogre::SceneNode* const transform_node  = this->get_or_create_transform_node(root_scene_node);
        m_volume_scene_node = transform_node->createChildSceneNode(gen_id("transform_origin"));
    }

    //Renderer
    {
        sight::viz::scene3d::layer::sptr layer = render_service->layer(layer_id());

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
            static_cast<std::uint16_t>(*m_sampling),
            m_config.dynamic,
            *m_preintegration,
            shadows,
            sat
        );
    }

    m_volume_scene_node->setVisible(visible());

    // Initially focus on the image center.
    this->set_focal_distance(50);

    bool is_valid = false;
    //image
    {
        const auto image = m_image.lock();
        is_valid = data::helper::medical_image::check_image_validity(image.get_shared());
    }

    m_volume_renderer->update_ray_tracing_material();

    if(is_valid)
    {
        this->new_image();
    }

    {
        const auto tf = m_tf.lock();
        // Will also update the SAT
        m_volume_renderer->set_sampling(static_cast<std::uint16_t>(*m_sampling), tf.get_shared());
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::updating()
{
    if(update_needed(update_flags::image))
    {
        new_image();
    }
    else // new_image() already calls all the functions below
    {
        if(update_needed(update_flags::image_buffer))
        {
            buffer_image();
        }

        if(update_needed(update_flags::mask_buffer))
        {
            update_mask();
        }

        if(update_needed(update_flags::tf))
        {
            update_volume_tf();
        }

        if(update_needed(update_flags::clipping_box))
        {
            update_clipping_box();
        }
    }

    update_done();
    this->request_render();
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

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void volume_render::update_volume_tf()
{
    this->render_service()->make_current();
    std::scoped_lock swap_lock(m_mutex);

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

        if(not data::helper::medical_image::check_image_validity(image.get_shared()))
        {
            return;
        }

        render_service->make_current();
        // Lazy update, necessary to ensure the image is loaded and its size known for the SAT computation
        m_volume_renderer->load_image();

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
                this->slot(slots::UPDATE_IMAGE)->async_run();
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
        this->render_service()->reset_camera_coordinates(layer_id());
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
    std::scoped_lock swap_lock(m_mutex);
    this->render_service()->make_current();

    SIGHT_ASSERT("Sampling rate must fit in a 16 bit uint.", _nb_samples < 65536);

    const auto tf = m_tf.lock();
    m_volume_renderer->set_sampling(static_cast<std::uint16_t>(_nb_samples), tf.get_shared());

    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::update_opacity_correction(unsigned _opacity_correction)
{
    std::scoped_lock swap_lock(m_mutex);
    this->render_service()->make_current();
    m_volume_renderer->set_opacity_correction(static_cast<int>(_opacity_correction));
    this->request_render();
}

//-----------------------------------------------------------------------------

void volume_render::update_ao_factor(float _ao_factor)
{
    std::scoped_lock swap_lock(m_mutex);
    this->render_service()->make_current();

    if(m_volume_renderer->set_ao_factor(_ao_factor))
    {
        this->request_render(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_color_bleeding_factor(float _color_bleeding_factor)
{
    std::scoped_lock swap_lock(m_mutex);
    this->render_service()->make_current();

    if(m_volume_renderer->set_color_bleeding_factor(_color_bleeding_factor))
    {
        this->request_render(); //Only request new render when AO was enabled, i.e. the call had an effect
    }
}

//-----------------------------------------------------------------------------

void volume_render::update_sat_size_ratio(unsigned int _size_ratio)
{
    std::scoped_lock swap_lock(m_mutex);
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
    std::scoped_lock swap_lock(m_mutex);
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
    std::scoped_lock swap_lock(m_mutex);
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
    std::scoped_lock swap_lock(m_mutex);
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
    std::scoped_lock swap_lock(m_mutex);
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
    std::scoped_lock swap_lock(m_mutex);
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
    std::scoped_lock swap_lock(m_mutex);
    this->toggle_vr_effect(vr_effect_type::vr_ambient_occlusion, _ambient_occlusion);
}

//-----------------------------------------------------------------------------

void volume_render::toggle_color_bleeding(bool _color_bleeding)
{
    std::scoped_lock swap_lock(m_mutex);
    this->toggle_vr_effect(vr_effect_type::vr_color_bleeding, _color_bleeding);
}

//-----------------------------------------------------------------------------

void volume_render::toggle_shadows(bool _shadows)
{
    std::scoped_lock swap_lock(m_mutex);
    this->toggle_vr_effect(vr_effect_type::vr_shadows, _shadows);
}

//-----------------------------------------------------------------------------

void volume_render::toggle_widgets(bool _visible)
{
    std::scoped_lock swap_lock(m_mutex);

    if(m_widget)
    {
        m_widget->set_box_visibility(_visible && m_volume_renderer->is_visible());

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void volume_render::set_focal_distance(int _focal_distance)
{
    if(this->render_service()->layer(layer_id())->get_stereo_mode()
       != sight::viz::scene3d::compositor::core::stereo_mode_t::none)
    {
        m_volume_renderer->set_focal_length(static_cast<float>(_focal_distance) / 100);
    }
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
        this->get_id(),
        m_volume_scene_node,
        ogre_clipping_mx,
        clipping_mx_update,
        "BasicAmbient",
        "BasicPhong"
    );

    layer->add_interactor(m_widget, m_config.priority);

    m_volume_renderer->clip_image(m_widget->get_clipping_box());

    m_widget->set_box_visibility(*m_widgets && m_volume_renderer->is_visible());
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

            std::scoped_lock swap_lock(m_mutex);
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
        clipping_matrix->async_emit(this, data::signals::MODIFIED);
    }

    std::scoped_lock swap_lock(m_mutex);
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
            m_widget->set_box_visibility(_visible && *m_widgets);
        }

        if(m_config.camera_autoreset)
        {
            this->render_service()->reset_camera_coordinates(layer_id());
        }

        this->request_render();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

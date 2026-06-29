/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "module/viz/scene3d/adaptor/negato.hpp"

#include <core/ptree.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <viz/scene3d/utils.hpp>

#include <OgreSceneNode.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

service::connections_t negato::auto_connections() const
{
    service::connections_t connections = {
        {m_image, data::signals::MODIFIED, slots::UPDATE_IMAGE},
        {m_image, data::image::signals::BUFFER_MODIFIED, slots::UPDATE_IMAGE_BUFFER},
        {m_image, data::image::signals::SLICE_TYPE_MODIFIED, slots::SLICE_TYPE},
        {m_image, data::image::signals::SLICE_INDEX_MODIFIED, slots::SLICE_INDEX},
        {m_tf, data::signals::MODIFIED, slots::UPDATE_TF},
        {m_tf, data::transfer_function::signals::POINTS_MODIFIED, slots::UPDATE_TF},
        {m_tf, data::transfer_function::signals::WINDOWING_MODIFIED, slots::UPDATE_TF},
        {m_mask, data::signals::MODIFIED, slots::UPDATE_MASK},
        {m_mask, data::image::signals::BUFFER_MODIFIED, slots::UPDATE_MASK}
    };
    return connections + adaptor::auto_connections();
}

//------------------------------------------------------------------------------

negato::negato() noexcept
{
    // Auto-connected slots
    new_slot(slots::UPDATE_IMAGE, [this](){lazy_update(update_flags::image);});
    new_slot(slots::UPDATE_IMAGE_BUFFER, [this](){lazy_update(update_flags::image_buffer);});
    new_slot(slots::UPDATE_TF, [this](){lazy_update(update_flags::tf);});
    new_slot(slots::UPDATE_MASK, [this](){lazy_update(update_flags::mask);});

    // Interaction slots
    new_slot(slots::SLICE_TYPE, &negato::change_slice_type, this);
    new_slot(slots::SLICE_INDEX, &negato::change_slice_index, this);
    new_slot(slots::UPDATE_SLICES_FROM_WORLD, &negato::update_slices_from_world, this);

    new_signal<signals::slice_index_changed_t>(signals::SLICE_INDEX_CHANGED);
    new_signal<signals::picked_voxel_t>(signals::PICKED_VOXEL);
}

//------------------------------------------------------------------------------

void negato::configuring(const config_t& _config)
{
    this->configure_params();

    static const std::string s_FILTERING_CONFIG   = CONFIG + "filtering";
    static const std::string s_TF_ALPHA_CONFIG    = CONFIG + "tf_alpha";
    static const std::string s_BORDER_CONFIG      = CONFIG + "border";
    static const std::string s_INTERACTIVE_CONFIG = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG    = CONFIG + "priority";

    if(const auto filtering_cfg = _config.get_optional<std::string>(s_FILTERING_CONFIG); filtering_cfg.has_value())
    {
        sight::viz::scene3d::plane::filter_t filtering(sight::viz::scene3d::plane::filter_t::linear);

        if(filtering_cfg.value() == "none")
        {
            filtering = sight::viz::scene3d::plane::filter_t::none;
        }
        else if(filtering_cfg.value() == "anisotropic")
        {
            filtering = sight::viz::scene3d::plane::filter_t::anisotropic;
        }

        this->set_filtering(filtering);
    }

    const auto hexa_mask = core::ptree::get_and_deprecate<std::string>(
        _config,
        CONFIG + "query_flags",
        CONFIG + "queryFlags",
        "26.0"
    );
    if(not hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_flags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    m_enable_alpha = _config.get<bool>(s_TF_ALPHA_CONFIG, m_enable_alpha);
    m_border       = _config.get<bool>(s_BORDER_CONFIG, m_border);
    m_interactive  = _config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);

    const std::string transform_id =
        _config.get<std::string>(sight::viz::scene3d::transformable::TRANSFORM_CONFIG, gen_id("transform"));
    this->set_transform_id(transform_id);
}

//------------------------------------------------------------------------------

void negato::starting()
{
    adaptor::init();

    this->render_service()->make_current();
    {
        // 3D source texture instantiation
        const auto image = m_image.lock();
        m_3d_ogre_texture = std::make_shared<sight::viz::scene3d::texture>(image.get_shared());

        // TF texture initialization
        const auto tf = m_tf.lock();
        if(tf)
        {
            m_gpu_tf = std::make_unique<sight::viz::scene3d::transfer_function>(tf.get_shared());
        }
    }

    // scene node's instantiation
    Ogre::SceneNode* const root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* const transform_node  = this->get_or_create_transform_node(root_scene_node);
    m_origin_scene_node = transform_node->createChildSceneNode();
    m_negato_scene_node = m_origin_scene_node->createChildSceneNode();

    this->update_mask();
    this->update_image(true);

    if(m_auto_reset_camera)
    {
        this->render_service()->reset_camera_coordinates(layer_id());
    }

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->add_interactor(interactor, static_cast<int>(*m_priority));

        m_picking_cross = std::make_unique<sight::viz::scene3d::picking_cross>(
            this->get_id(),
            *this->get_scene_manager(),
            *m_negato_scene_node
        );
    }
}

//------------------------------------------------------------------------------

void negato::stopping()
{
    this->render_service()->make_current();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->remove_interactor(interactor);
    }

    m_picked_plane.reset();
    std::ranges::for_each(m_planes, [](auto& _p){_p.first.reset();});

    auto* const scene_manager = this->get_scene_manager();
    m_negato_scene_node->removeAndDestroyAllChildren();
    scene_manager->destroySceneNode(m_negato_scene_node);
    m_negato_scene_node = nullptr;

    m_origin_scene_node->removeAndDestroyAllChildren();
    scene_manager->destroySceneNode(m_origin_scene_node);
    m_origin_scene_node = nullptr;

    m_picking_cross.reset();
    m_mask_texture.reset();
    m_3d_ogre_texture.reset();
    m_gpu_tf.reset();

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void negato::updating()
{
    if(update_needed(update_flags::image))
    {
        this->update_image(true);
    }
    else if(update_needed(update_flags::image_buffer))
    {
        this->update_image(false);
    }
    else if(update_needed(update_flags::tf))
    {
        this->update_tf();
    }
    else if(update_needed(update_flags::mask))
    {
        this->update_mask();
    }

    this->update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void negato::update_mask()
{
    const auto mask = m_mask.lock();
    if(mask)
    {
        if(!m_mask_texture)
        {
            m_mask_texture = std::make_shared<sight::viz::scene3d::texture>(mask.get_shared());
        }

        if(mask->num_elements() > 0)
        {
            m_mask_texture->update();
        }
    }
}

//------------------------------------------------------------------------------

void negato::update_image(bool _new)
{
    if(!m_3d_ogre_texture)
    {
        // The adaptor hasn't start yet (the window is maybe not visible)
        return;
    }

    this->render_service()->make_current();

    int axial_idx    = 0;
    int frontal_idx  = 0;
    int sagittal_idx = 0;
    {
        {
            const auto image = m_image.lock();

            if(!data::helper::medical_image::check_image_validity(image.get_shared()))
            {
                std::ranges::for_each(m_planes, [](auto& _p){_p.first.reset();});
                return;
            }
        }

        // Update the texture
        m_3d_ogre_texture->update();

        const auto image = m_image.lock();
        if(_new)
        {
            const auto mask = m_mask.lock();
            std::string material_name;
            const auto tf = m_tf.lock();
            if(tf == nullptr)
            {
                material_name = mask != nullptr ? "Negato_rgb_mask" : "Negato_rgb";
            }
            else
            {
                // Default case with gray scale image
                material_name = (mask != nullptr) ? "Negato_mask" : (*m_classification == std::string("post"))
                                ? "Negato" : "Negato_pre";
            }

            // Instantiation of the planes
            for(auto& plane : m_planes)
            {
                plane.first = std::make_shared<sight::viz::scene3d::plane>(
                    this->get_id(),
                    m_negato_scene_node,
                    this->get_scene_manager(),
                    m_3d_ogre_texture,
                    m_mask_texture,
                    m_filtering,
                    material_name,
                    m_border,
                    m_slices_cross,
                    1.0F,
                    static_cast<float>(*m_depth_bias)
                );
            }

            const auto spacing = sight::viz::scene3d::utils::get_ogre_spacing(*image);
            m_origin_scene_node->setPosition(sight::viz::scene3d::utils::get_ogre_origin(*image));
            m_origin_scene_node->setOrientation(sight::viz::scene3d::utils::get_ogre_orientation(*image));

            // Fits the planes to the new texture
            for(const auto& plane : m_planes)
            {
                plane.first->update(plane.second, spacing, this->priority());
                plane.first->set_query_flags(m_query_flags);
                SIGHT_ERROR_IF(
                    "Priority is too high, should be less than 65535",
                    *m_priority >= 0xFFFF
                );
            }

            // Update Slice
            namespace medical_image = data::helper::medical_image;

            axial_idx = std::max(
                0,
                static_cast<int>(medical_image::get_slice_index(*image, axis_t::axial).value_or(0))
            );
            frontal_idx = std::max(
                0,
                static_cast<int>(medical_image::get_slice_index(*image, axis_t::frontal).value_or(0))
            );
            sagittal_idx = std::max(
                0,
                static_cast<int>(medical_image::get_slice_index(*image, axis_t::sagittal).value_or(0))
            );
        }
    }

    if(_new)
    {
        this->change_slice_index(axial_idx, frontal_idx, sagittal_idx);
        this->set_visible(visible());
        // Update transfer function in Gpu programs
        this->update_tf();
    }
}

//------------------------------------------------------------------------------

void negato::change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index)
{
    {
        const auto image = m_image.lock();

        if(!data::helper::medical_image::check_image_validity(image.get_shared()))
        {
            return;
        }

        this->render_service()->make_current();

        auto img_size = image->size();

        // Sometimes, the image can contain only one slice,
        // it results into a division by 0 when the range is transformed between [0-1].
        // So we increase the image size to 2 to divide by 1.
        img_size[0] = img_size[0] == 1 ? 2 : img_size[0];
        img_size[1] = img_size[1] == 1 ? 2 : img_size[1];
        img_size[2] = img_size[2] == 1 ? 2 : img_size[2];

        m_current_slice_index = {
            static_cast<float>(_sagittal_index) / (static_cast<float>(img_size[0] - 1)),
            static_cast<float>(_frontal_index) / (static_cast<float>(img_size[1] - 1)),
            static_cast<float>(_axial_index) / (static_cast<float>(img_size[2] - 1))
        };

        for(auto& plane : m_planes)
        {
            if(plane.first)
            {
                plane.first->change_slice(m_current_slice_index, this->priority());
            }
        }
    }

    // Here we cheat a bit by sending synchronously to ensure that the new slice index is taken into account by
    // potential other negatos, before rendering the new slice. Otherwise we would suffer from a flickering effect.
    this->emit(this, signals::SLICE_INDEX_CHANGED, _axial_index, _frontal_index, _sagittal_index);

    this->request_render();
}

//------------------------------------------------------------------------------

void negato::update_slices_from_world(double _x, double _y, double _z)
{
    const auto image = m_image.lock();

    Ogre::Vector3 point = {static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_z)};
    Ogre::Vector3i slice_idx;
    try
    {
        slice_idx = sight::viz::scene3d::utils::world_to_slices(*image, point);
    }
    catch(core::exception& e)
    {
        SIGHT_WARN("Cannot update slice index: " << e.what());
        return;
    }

    image->async_emit(data::image::signals::SLICE_INDEX_MODIFIED, slice_idx[2], slice_idx[1], slice_idx[0]);
}

//------------------------------------------------------------------------------

void negato::update_tf()
{
    this->render_service()->make_current();
    if(m_gpu_tf)
    {
        m_gpu_tf->update();

        // Sends the TF texture to the negato-related passes
        std::ranges::for_each(
            m_planes,
            [this](auto& _p)
            {
                if(_p.first)
                {
                    _p.first->set_tf_data(*m_gpu_tf.get(), m_enable_alpha);
                }
            });
    }
}

//------------------------------------------------------------------------------

void negato::update_windowing(double _dw, double _dl)
{
    const double new_window = m_initial_window + _dw;
    const double new_level  = m_initial_level - _dl;

    {
        const auto tf = m_tf.lock();
        if(tf)
        {
            const auto image = m_image.const_lock();
            tf->set_window(std::copysign(std::max(1.0, std::abs(new_window)), new_window));
            tf->set_level(new_level);
        }

        tf->async_emit(data::transfer_function::signals::WINDOWING_MODIFIED, new_window, new_level);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

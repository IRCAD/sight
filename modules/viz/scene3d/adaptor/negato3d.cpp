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

// cspell:ignore NOLINTNEXTLINE NOLINT

#include "modules/viz/scene3d/adaptor/negato3d.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/tools/color.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

negato3d::negato3d() noexcept
{
    // Auto-connected slots
    new_slot(slots::UPDATE_IMAGE, [this](){lazy_update(update_flags::IMAGE);});
    new_slot(slots::UPDATE_TF, [this](){lazy_update(update_flags::TF);});

    // Interaction slots
    new_slot(slots::SLICE_TYPE, &negato3d::change_slice_type, this);
    new_slot(slots::SLICE_INDEX, &negato3d::change_slice_index, this);
    new_slot(slots::UPDATE_SLICES_FROM_WORLD, &negato3d::update_slices_from_world, this);
    new_slot(slots::SET_TRANSPARENCY, &negato3d::set_transparency, this);

    new_signal<signals::picked_voxel_t>(signals::PICKED_VOXEL);
}

//------------------------------------------------------------------------------

void negato3d::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_AUTORESET_CAMERA_CONFIG = CONFIG + "autoresetcamera";
    static const std::string s_FILTERING_CONFIG        = CONFIG + "filtering";
    static const std::string s_TF_ALPHA_CONFIG         = CONFIG + "tfAlpha";
    static const std::string s_INTERACTIVE_CONFIG      = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG         = CONFIG + "priority";
    static const std::string s_QUERY_CONFIG            = CONFIG + "queryFlags";
    static const std::string s_BORDER_CONFIG           = CONFIG + "border";

    m_auto_reset_camera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    if(config.count(s_FILTERING_CONFIG) != 0U)
    {
        const auto filtering_value = config.get<std::string>(s_FILTERING_CONFIG);
        sight::viz::scene3d::plane::filter_t filtering(sight::viz::scene3d::plane::filter_t::linear);

        if(filtering_value == "none")
        {
            filtering = sight::viz::scene3d::plane::filter_t::none;
        }
        else if(filtering_value == "anisotropic")
        {
            filtering = sight::viz::scene3d::plane::filter_t::anisotropic;
        }

        m_filtering = filtering;
    }

    const std::string hexa_mask = config.get<std::string>(s_QUERY_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_flags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    m_enable_alpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enable_alpha);
    m_interactive  = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority     = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_border       = config.get<bool>(s_BORDER_CONFIG, m_border);

    const std::string transform_id =
        config.get<std::string>(sight::viz::scene3d::transformable::TRANSFORM_CONFIG, this->get_id() + "_transform");
    this->set_transform_id(transform_id);
}

//------------------------------------------------------------------------------

void negato3d::starting()
{
    adaptor::init();

    this->render_service()->make_current();
    {
        // 3D source texture instantiation
        const auto image = m_image.lock();
        m_3d_ogre_texture = std::make_shared<sight::viz::scene3d::texture>(image.get_shared());

        // TF texture initialization
        const auto tf = m_tf.lock();
        m_gpu_tf = std::make_unique<sight::viz::scene3d::transfer_function>(tf.get_shared());
    }

    // scene node's instantiation
    Ogre::SceneNode* const root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* const transform_node  = this->get_or_create_transform_node(root_scene_node);
    m_negato_scene_node = transform_node->createChildSceneNode();

    // Instantiation of the planes
    for(auto& plane : m_planes)
    {
        plane = std::make_shared<sight::viz::scene3d::plane>(
            this->get_id(),
            m_negato_scene_node,
            this->get_scene_manager(),
            m_3d_ogre_texture,
            m_filtering,
            m_border,
            false
        );
    }

    if(m_auto_reset_camera)
    {
        this->render_service()->reset_camera_coordinates(m_layer_id);
    }

    this->new_image();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->add_interactor(interactor, m_priority);

        m_picking_cross = std::make_unique<sight::viz::scene3d::picking_cross>(
            this->get_id(),
            *this->get_scene_manager(),
            *m_negato_scene_node
        );
    }

    // Set the visibility of the 3D Negato
    this->set_visible(visible());
}

//-----------------------------------------------------------------------------

service::connections_t negato3d::auto_connections() const
{
    service::connections_t connections = {
        {m_image, data::image::MODIFIED_SIG, slots::UPDATE_IMAGE},
        {m_image, data::image::BUFFER_MODIFIED_SIG, slots::UPDATE_IMAGE},
        {m_tf, data::transfer_function::POINTS_MODIFIED_SIG, slots::UPDATE_TF},
        {m_tf, data::transfer_function::WINDOWING_MODIFIED_SIG, slots::UPDATE_TF},
        {m_tf, data::transfer_function::MODIFIED_SIG, slots::UPDATE_TF},
        {m_image, data::image::SLICE_TYPE_MODIFIED_SIG, slots::SLICE_TYPE},
        {m_image, data::image::SLICE_INDEX_MODIFIED_SIG, slots::SLICE_INDEX}
    };
    return connections + adaptor::auto_connections();
}

//------------------------------------------------------------------------------

void negato3d::updating()
{
    if(update_needed(update_flags::IMAGE))
    {
        this->new_image();
    }
    else if(update_needed(update_flags::TF))
    {
        this->update_tf();
    }

    this->update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void negato3d::stopping()
{
    this->render_service()->make_current();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->remove_interactor(interactor);
    }

    m_picked_plane.reset();
    std::ranges::for_each(m_planes, [](auto& _p){_p.reset();});

    m_negato_scene_node->removeAndDestroyAllChildren();
    this->get_scene_manager()->destroySceneNode(m_negato_scene_node);

    m_picking_cross.reset();

    m_3d_ogre_texture.reset();
    m_gpu_tf.reset();

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void negato3d::new_image()
{
    this->render_service()->make_current();

    int axial_idx    = 0;
    int frontal_idx  = 0;
    int sagittal_idx = 0;
    {
        const auto image = m_image.lock();

        if(!data::helper::medical_image::check_image_validity(image.get_shared()))
        {
            return;
        }

        // Retrieves or creates the slice index fields
        m_3d_ogre_texture->update();

        const auto [spacing, origin] = sight::viz::scene3d::utils::convert_spacing_and_origin(image.get_shared());

        // Fits the planes to the new texture
        for(int orientation_num = 0 ; const auto& plane : m_planes)
        {
            plane->update(static_cast<orientation_mode>(orientation_num++), spacing, origin, m_enable_alpha);
            plane->set_query_flags(m_query_flags);
        }

        // Update Slice
        namespace imHelper = data::helper::medical_image;

        axial_idx   = std::max(0, int(imHelper::get_slice_index(*image, imHelper::orientation_t::axial).value_or(0)));
        frontal_idx =
            std::max(0, int(imHelper::get_slice_index(*image, imHelper::orientation_t::frontal).value_or(0)));
        sagittal_idx =
            std::max(0, int(imHelper::get_slice_index(*image, imHelper::orientation_t::sagittal).value_or(0)));
    }

    this->change_slice_index(axial_idx, frontal_idx, sagittal_idx);

    // Update transfer function in Gpu programs
    this->update_tf();

    this->apply_visibility();

    this->request_render();
}

//------------------------------------------------------------------------------

void negato3d::change_slice_type(int /*unused*/, int /*unused*/)
{
    this->render_service()->make_current();

    this->update_tf();

    this->request_render();
}

//------------------------------------------------------------------------------

void negato3d::change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index)
{
    const auto image = m_image.lock();

    auto img_size = image->size();

    // Sometimes, the image can contain only one slice,
    // it results into a division by 0 when the range is transformed between [0-1].
    // So we increase the image size to 2 to divide by 1.
    img_size[0] = img_size[0] == 1 ? 2 : img_size[0];
    img_size[1] = img_size[1] == 1 ? 2 : img_size[1];
    img_size[2] = img_size[2] == 1 ? 2 : img_size[2];

    const std::array<float, 3> slice_indices = {
        static_cast<float>(_sagittal_index) / (static_cast<float>(img_size[0] - 1)),
        static_cast<float>(_frontal_index) / (static_cast<float>(img_size[1] - 1)),
        static_cast<float>(_axial_index) / (static_cast<float>(img_size[2] - 1))
    };

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        m_planes[i]->change_slice(slice_indices);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void negato3d::update_tf()
{
    this->render_service()->make_current();
    m_gpu_tf->update();

    // Sends the TF texture to the negato-related passes
    std::ranges::for_each(m_planes, [this](auto& _p){_p->set_tf_data(*m_gpu_tf.get());});

    this->request_render();
}

//-----------------------------------------------------------------------------

void negato3d::set_transparency(double _transparency)
{
    SIGHT_ASSERT("Service not started", this->started());

    const float opacity = 1.F - static_cast<float>(_transparency);
    std::ranges::for_each(m_planes, [opacity](auto& _p){_p->set_entity_opacity(opacity);});

    this->request_render();
}

//------------------------------------------------------------------------------

void negato3d::set_visible(bool _visible)
{
    std::ranges::for_each(m_planes, [_visible](auto& _p){_p->set_visible(_visible);});
    if(m_auto_reset_camera)
    {
        this->render_service()->reset_camera_coordinates(m_layer_id);
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void negato3d::set_planes_query_flags(std::uint32_t _flags)
{
    std::ranges::for_each(m_planes, [_flags](auto& _p){_p->set_query_flags(_flags);});
}

//------------------------------------------------------------------------------

void negato3d::mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked_plane)
    {
        if(_button == mouse_button::middle)
        {
            this->move_slices(_x, _y);
        }
        else if(_button == mouse_button::right)
        {
            const auto dx = static_cast<double>(_x - m_initial_pos[0]);
            const auto dy = static_cast<double>(m_initial_pos[1] - _y);

            this->update_windowing(dx, dy);
        }
        else if(_button == mouse_button::left)
        {
            this->pick_intensity(_x, _y);
        }

        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato3d::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    m_picked_plane.reset();
    m_picking_cross->set_visible(false);

    if(_button == mouse_button::middle)
    {
        this->move_slices(_x, _y);
    }
    else if(_button == mouse_button::right)
    {
        if(this->get_picked_slices(_x, _y) != std::nullopt)
        {
            const auto tf = m_tf.const_lock();

            m_initial_level  = tf->level();
            m_initial_window = tf->window();

            m_initial_pos = {_x, _y};
        }
    }
    else if(_button == mouse_button::left)
    {
        this->pick_intensity(_x, _y);
    }

    if(m_picked_plane)
    {
        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato3d::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_picked_plane)
    {
        m_picked_plane->set_render_queuer_group_and_priority(sight::viz::scene3d::rq::SURFACE_ID, 0);
        m_picked_plane.reset();
    }

    m_picking_cross->set_visible(false);
    this->signal<signals::picked_voxel_t>(signals::PICKED_VOXEL)->async_emit("");
    this->set_planes_query_flags(m_query_flags); // Make all planes pickable again.
}

//------------------------------------------------------------------------------

void negato3d::move_slices(int _x, int _y)
{
    const auto pick_res = this->get_picked_slices(_x, _y);

    if(pick_res.has_value())
    {
        const auto image = m_image.lock();

        auto picked_pt = pick_res.value();

        std::ranges::for_each(
            m_planes,
            [this](auto& _p)
            {
                if(_p != m_picked_plane)
                {
                    _p->set_query_flags(0x0);
                }
            });

        const auto [spacing, origin] = sight::viz::scene3d::utils::convert_spacing_and_origin(image.get_shared());
        picked_pt                    = (picked_pt - origin) / spacing;

        const Ogre::Vector3i picked_pt_i(picked_pt);
        const auto sig = image->signal<data::image::slice_index_modified_signal_t>
                             (data::image::SLICE_INDEX_MODIFIED_SIG);
        sig->async_emit(picked_pt_i[2], picked_pt_i[1], picked_pt_i[0]);
    }
}

//------------------------------------------------------------------------------

void negato3d::update_slices_from_world(double _x, double _y, double _z)
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

    const auto sig = image->signal<data::image::slice_index_modified_signal_t>
                         (data::image::SLICE_INDEX_MODIFIED_SIG);

    sig->async_emit(slice_idx[2], slice_idx[1], slice_idx[0]);
}

//------------------------------------------------------------------------------

void negato3d::pick_intensity(int _x, int _y)
{
    auto sig = this->signal<signals::picked_voxel_t>(signals::PICKED_VOXEL);
    if(sig->num_connections() > 0)
    {
        const auto picked_pos = this->get_picked_slices(_x, _y);

        if(picked_pos.has_value())
        {
            const auto image = m_image.lock();

            if(!data::helper::medical_image::check_image_validity(image.get_shared()))
            {
                return;
            }

            const auto image_buffer_lock = image->dump_lock();

            const auto [spacing, origin] = sight::viz::scene3d::utils::convert_spacing_and_origin(image.get_shared());

            auto cross_lines = m_picked_plane->compute_cross(*picked_pos, origin);
            m_picking_cross->update(cross_lines[0], cross_lines[1], cross_lines[2], cross_lines[3]);
            const auto picking_text = sight::viz::scene3d::utils::pick_image(*image, *picked_pos, origin, spacing);
            sig->async_emit(picking_text);

            this->request_render();

            // Render the picked plane before the widget.
            m_picked_plane->set_render_queuer_group_and_priority(sight::viz::scene3d::rq::NEGATO_WIDGET_ID, 0);
        }
    }
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> negato3d::get_picked_slices(int _x, int _y)
{
    Ogre::SceneManager* sm = this->get_scene_manager();
    const auto result      = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_flags, *sm);

    if(result.has_value())
    {
        const auto is_picked = [&result](const auto& _p){return _p->get_movable_object() == result->first;};
        const auto it = std::ranges::find_if(m_planes, is_picked); // NOLINT(readability-qualified-auto,llvm-qualified-auto)

        if(it != m_planes.cend())
        {
            m_picked_plane = *it;
            return result->second;
        }
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void negato3d::update_windowing(double _dw, double _dl)
{
    const double new_window = m_initial_window + _dw;
    const double new_level  = m_initial_level - _dl;

    {
        const auto tf = m_tf.lock();

        tf->set_window(new_window);
        tf->set_level(new_level);
        const auto sig = tf->signal<data::transfer_function::windowing_modified_signal_t>(
            data::transfer_function::WINDOWING_MODIFIED_SIG
        );
        {
            sig->async_emit(new_window, new_level);
        }
    }
}

} // namespace sight::module::viz::scene3d::adaptor.

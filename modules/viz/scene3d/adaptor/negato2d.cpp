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

#include "modules/viz/scene3d/adaptor/negato2d.hpp"

#include <core/com/signals.hpp>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OgreSceneNode.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::slots::key_t SLICETYPE_SLOT        = "sliceType";
const core::com::slots::key_t SLICEINDEX_SLOT       = "sliceIndex";
static const core::com::slots::key_t UPDATE_TF_SLOT = "updateTF";

static const core::com::slots::key_t UPDATE_SLICES_FROM_WORLD = "updateSlicesFromWorld";

static const core::com::signals::key_t SLICE_INDEX_CHANGED_SIG = "sliceIndexChanged";
static const core::com::signals::key_t PICKED_VOXEL_SIG        = "pickedVoxel";

//------------------------------------------------------------------------------

negato2d::negato2d() noexcept
{
    new_slot(SLICETYPE_SLOT, &negato2d::change_slice_type, this);
    new_slot(SLICEINDEX_SLOT, &negato2d::change_slice_index, this);
    new_slot(UPDATE_SLICES_FROM_WORLD, &negato2d::update_slices_from_world, this);
    new_slot(UPDATE_TF_SLOT, &negato2d::update_tf, this);

    m_slice_index_changed_sig = new_signal<slice_index_changed_signal_type>(SLICE_INDEX_CHANGED_SIG);
    m_picked_voxel_signal     = new_signal<picked_voxel_sig_t>(PICKED_VOXEL_SIG);
}

//------------------------------------------------------------------------------

void negato2d::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_SLICE_INDEX_CONFIG  = CONFIG + "sliceIndex";
    static const std::string s_FILTERING_CONFIG    = CONFIG + "filtering";
    static const std::string s_TF_ALPHA_CONFIG     = CONFIG + "tfAlpha";
    static const std::string s_BORDER_CONFIG       = CONFIG + "border";
    static const std::string s_SLICES_CROSS_CONFIG = CONFIG + "slicesCross";
    static const std::string s_INTERACTIVE_CONFIG  = CONFIG + "interactive";

    const auto orientation = config.get<std::string>(s_SLICE_INDEX_CONFIG, "axial");
    if(orientation == "axial")
    {
        m_orientation = orientation_mode::z_axis;
    }
    else if(orientation == "frontal")
    {
        m_orientation = orientation_mode::y_axis;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = orientation_mode::x_axis;
    }

    if(const auto filtering_cfg = config.get_optional<std::string>(s_FILTERING_CONFIG); filtering_cfg.has_value())
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

    m_enable_alpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enable_alpha);
    m_border       = config.get<bool>(s_BORDER_CONFIG, m_border);
    m_slices_cross = config.get<bool>(s_SLICES_CROSS_CONFIG, m_slices_cross);
    m_interactive  = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);

    const std::string transform_id =
        config.get<std::string>(sight::viz::scene3d::transformable::TRANSFORM_CONFIG, this->get_id() + "_transform");
    this->set_transform_id(transform_id);
}

//------------------------------------------------------------------------------

void negato2d::starting()
{
    this->initialize();
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

    // Plane's instantiation
    m_plane = std::make_unique<sight::viz::scene3d::plane>(
        this->get_id(),
        m_negato_scene_node,
        get_scene_manager(),
        m_3d_ogre_texture,
        m_filtering,
        m_border,
        m_slices_cross
    );

    this->new_image();
    this->set_visible(m_visible);

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->layer()->add_interactor(interactor, 0);

        m_picking_cross = std::make_unique<sight::viz::scene3d::picking_cross>(
            this->get_id(),
            *this->get_scene_manager(),
            *m_negato_scene_node
        );
    }
}

//------------------------------------------------------------------------------

void negato2d::stopping()
{
    this->render_service()->make_current();

    m_picking_cross.reset();
    m_plane.reset();
    m_3d_ogre_texture.reset();
    m_gpu_tf.reset();

    this->request_render();
}

//------------------------------------------------------------------------------

void negato2d::updating()
{
    this->new_image();
    this->set_visible(m_visible);
}

//------------------------------------------------------------------------------

void negato2d::new_image()
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
        const auto image = m_image.lock();

        if(!data::helper::medical_image::check_image_validity(image.get_shared()))
        {
            return;
        }

        // Retrieves or creates the slice index fields
        m_3d_ogre_texture->update();

        const auto [spacing, origin] = sight::viz::scene3d::utils::convert_spacing_and_origin(image.get_shared());

        // Fits the plane to the new texture
        m_plane->update(m_orientation, spacing, origin, m_enable_alpha);

        // Render the picked plane before the picking cross
        m_plane->set_render_queuer_group_and_priority(sight::viz::scene3d::rq::NEGATO_WIDGET_ID, 0);

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

    this->request_render();
}

//------------------------------------------------------------------------------

void negato2d::change_slice_type(int _from, int _to)
{
    const auto image = m_image.lock();

    const auto to_orientation   = static_cast<orientation_mode>(_to);
    const auto from_orientation = static_cast<orientation_mode>(_from);

    const auto plane_orientation = m_orientation;
    const auto new_orientation   = m_orientation == to_orientation ? from_orientation
                                                                   : plane_orientation
                                   == from_orientation ? to_orientation : plane_orientation;

    if(plane_orientation != new_orientation)
    {
        m_orientation = new_orientation;
        this->render_service()->make_current();

        const auto& [spacing, origin] = sight::viz::scene3d::utils::convert_spacing_and_origin(image.get_shared());
        m_plane->update(m_orientation, spacing, origin, m_enable_alpha);

        // Update threshold if necessary
        this->update_tf();

        this->update_shader_slice_index_parameter();

        this->request_render();
    }
}

//------------------------------------------------------------------------------

void negato2d::change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index)
{
    const auto image = m_image.lock();

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

    this->update_shader_slice_index_parameter();

    m_slice_index_changed_sig->emit();

    this->request_render();
}

//------------------------------------------------------------------------------

void negato2d::update_slices_from_world(double _x, double _y, double _z)
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

void negato2d::update_shader_slice_index_parameter()
{
    m_plane->change_slice(m_current_slice_index);
}

//------------------------------------------------------------------------------

void negato2d::update_tf()
{
    this->render_service()->make_current();

    m_gpu_tf->update();

    // Sends the TF texture to the negato-related passes
    m_plane->set_tf_data(*m_gpu_tf);

    this->request_render();
}

//------------------------------------------------------------------------------

void negato2d::mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked)
    {
        if(_button == mouse_button::left)
        {
            this->pick_intensity(_x, _y);
        }

        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato2d::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    m_picking_cross->set_visible(false);
    if(_button == mouse_button::left)
    {
        this->pick_intensity(_x, _y);
    }

    if(m_picked)
    {
        this->layer()->cancel_further_interaction();
    }
}

//------------------------------------------------------------------------------

void negato2d::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    m_picked = false;
    m_picking_cross->set_visible(false);
    m_picked_voxel_signal->async_emit("");
}

//-----------------------------------------------------------------------------

service::connections_t negato2d::auto_connections() const
{
    return {
        {IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::SLICE_TYPE_MODIFIED_SIG, SLICETYPE_SLOT},
        {IMAGE_IN, data::image::SLICE_INDEX_MODIFIED_SIG, SLICEINDEX_SLOT},
        {TF_IN, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {TF_IN, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {TF_IN, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------
void negato2d::set_visible(bool _visible)
{
    if(m_negato_scene_node != nullptr)
    {
        m_negato_scene_node->setVisible(_visible);

        this->request_render();
    }
}

//------------------------------------------------------------------------------

void negato2d::pick_intensity(int _x, int _y)
{
    Ogre::SceneManager* sm = this->get_scene_manager();

    const auto result = sight::viz::scene3d::utils::pick_object(_x, _y, Ogre::SceneManager::ENTITY_TYPE_MASK, *sm);

    if(result.has_value())
    {
        if(m_plane->get_movable_object() == result->first)
        {
            m_picked = true;
            const auto image = m_image.lock();

            if(!data::helper::medical_image::check_image_validity(image.get_shared()))
            {
                return;
            }

            const auto image_buffer_lock = image->dump_lock();

            const auto [spacing, origin] = sight::viz::scene3d::utils::convert_spacing_and_origin(image.get_shared());
            auto cross_lines = m_plane->compute_cross(result->second, origin);
            m_picking_cross->update(cross_lines[0], cross_lines[1], cross_lines[2], cross_lines[3]);

            const auto picking_text = sight::viz::scene3d::utils::pick_image(*image, result->second, origin, spacing);
            m_picked_voxel_signal->async_emit(picking_text);

            this->request_render();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

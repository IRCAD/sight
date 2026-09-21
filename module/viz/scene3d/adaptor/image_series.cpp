/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "image_series.hpp"

#include "negato.hpp"

#include <data/image_series.hpp>

#include <algorithm>
#include <cstring>
#include <unordered_set>
#include <utility>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

image_series::image_series() noexcept
{
    new_signal<signals::picked_voxel_t>(signals::PICKED_VOXEL);

    new_slot(slots::SET_IMAGE_VISIBILITY, &image_series::set_image_visibility, this);
    new_slot(slots::UPDATE_CLIPPING_BOX, &image_series::update_clipping_box, this);
    new_slot(slots::UPDATE_SLICES_FROM_WORLD, &image_series::update_slices_from_world, this);
    new_slot(slots::SET_SLICE_INDEX, &image_series::set_slice_index, this);
    new_slot(slots::SET_SLICE_TYPE, &image_series::set_slice_type, this);
    new_slot(slots::FORWARD_PICKED_VOXEL, &image_series::forward_picked_voxel, this);
    new_slot(slots::SET_TRANSPARENCY, &image_series::set_transparency, this);
    new_slot(slots::RESET_CLIPPING_BOX, &image_series::reset_clipping_box, this);
}

//-----------------------------------------------------------------------------

void image_series::configuring()
{
    this->configure_params();

    const auto representation = this->get_config().get<std::string>(
        CONFIG + "representation",
        "volume"
    );

    m_orientation = this->get_config().get<std::string>(CONFIG + "orientation", "axial");

    if(representation == "volume")
    {
        m_representation = representation_t::volume;
    }
    else if(representation == "negato2d")
    {
        m_representation = representation_t::negato2d;
    }
    else if(representation == "negato3d")
    {
        m_representation = representation_t::negato3d;
    }
    else
    {
        SIGHT_THROW("Unsupported image series representation: " << representation);
    }
}

//-----------------------------------------------------------------------------

void image_series::starting()
{
    this->init();
    this->updating();
}

//-----------------------------------------------------------------------------

void image_series::stopping()
{
    m_child_connections.disconnect();
    this->unregister_services();
    m_children.clear();
    m_masks.clear();
    m_extruders.clear();
    m_clipping_matrices.clear();
    m_child_visibility.clear();
    m_landmarks.clear();
    m_rulers.clear();
    this->deinit();
}

//-----------------------------------------------------------------------------

service::connections_t image_series::auto_connections() const
{
    auto connections = adaptor::auto_connections();
    connections.push("data.series", data::series_set::signals::ADDED_OBJECTS, adaptor::slots::LAZY_UPDATE);
    connections.push("data.series", data::series_set::signals::REMOVED_OBJECTS, adaptor::slots::LAZY_UPDATE);
    connections.push("data.series", data::series_set::signals::CHANGED_OBJECTS, adaptor::slots::LAZY_UPDATE);
    connections.push("data.series", data::signals::MODIFIED, adaptor::slots::LAZY_UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void image_series::updating()
{
    const auto series = m_series.lock();

    if(!series)
    {
        m_child_connections.disconnect();
        this->unregister_services();
        m_children.clear();
        m_masks.clear();

        m_extruders.clear();
        m_clipping_matrices.clear();
        m_child_visibility.clear();
        m_image_visibility.clear();
        m_landmarks.clear();
        m_rulers.clear();
        m_selection_initialized = false;
        this->update_done();
        return;
    }

    std::unordered_set<std::string> active_images;
    active_images.reserve(series->size());

    for(const auto& object : *series)
    {
        if(const auto image = std::dynamic_pointer_cast<data::image_series>(object); image && !image->get_id().empty())
        {
            active_images.insert(image->get_id());
        }
    }

    if(const auto meshes_by_image = m_extruded_meshes_by_image.lock(); meshes_by_image)
    {
        for(auto it = meshes_by_image->begin() ; it != meshes_by_image->end() ; )
        {
            if(!active_images.contains(it->first))
            {
                it = meshes_by_image->erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    for(auto it = m_image_visibility.begin() ; it != m_image_visibility.end() ; )
    {
        if(!active_images.contains(it->first))
        {
            if(const auto transfer_functions = m_image_transfer_functions.lock(); transfer_functions)
            {
                transfer_functions->erase(it->first);
            }

            m_clipping_matrices.erase(it->first);
            m_child_visibility.erase(it->first);
            it = m_image_visibility.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if(active_images.empty())
    {
        m_child_connections.disconnect();
        this->unregister_services();
        m_children.clear();
        m_masks.clear();
        m_extruders.clear();
        m_clipping_matrices.clear();
        m_child_visibility.clear();

        m_selection_initialized = false;
        this->update_done();
        this->request_render();
        return;
    }

    const bool topology_changed =
        active_images.size() != m_children.size()
        || std::ranges::any_of(
            active_images,
            [this](const std::string& _id)
        {
            return !m_children.contains(_id);
        });

    if(!topology_changed)
    {
        this->set_visible(this->visible());

        this->update_done();
        this->request_render();
        return;
    }

    m_child_connections.disconnect();

    for(auto it = m_children.begin() ; it != m_children.end() ; )
    {
        const std::string id = it->first;
        if(active_images.contains(id))
        {
            ++it;
            continue;
        }

        this->unregister_service(it->second);
        it = m_children.erase(it);

        if(const auto extruder = m_extruders.find(id); extruder != m_extruders.end())
        {
            this->unregister_service(extruder->second);
            m_extruders.erase(extruder);
        }

        if(const auto landmark = m_landmarks.find(id); landmark != m_landmarks.end())
        {
            this->unregister_service(landmark->second);
            m_landmarks.erase(landmark);
        }

        if(const auto ruler = m_rulers.find(id); ruler != m_rulers.end())
        {
            this->unregister_service(ruler->second);
            m_rulers.erase(ruler);
        }

        m_masks.erase(id);
        m_clipping_matrices.erase(id);
        m_child_visibility.erase(id);
    }

    bool first_image         = true;
    bool reset_camera        = true;
    std::size_t negato_index = 0;

    for(const auto& object : *series)
    {
        const auto image = std::dynamic_pointer_cast<data::image_series>(object);
        if(!image || image->get_id().empty())
        {
            continue;
        }

        const std::string id          = image->get_id();
        const bool default_visibility = !m_selection_initialized && first_image;
        const bool image_visible      = m_image_visibility.try_emplace(id, default_visibility).first->second;
        first_image = false;

        if(m_children.contains(id))
        {
            reset_camera = reset_camera && !image_visible;
            ++negato_index;
            continue;
        }

        std::string service_type;
        switch(m_representation)
        {
            case representation_t::volume:
                service_type = "sight::module::viz::scene3d::adaptor::volume_render";
                break;

            case representation_t::negato2d:
                service_type = "sight::module::viz::scene3d::adaptor::negato2d";
                break;

            case representation_t::negato3d:
                service_type = "sight::module::viz::scene3d::adaptor::negato3d";
                break;
        }

        const bool is_volume = m_representation == representation_t::volume;
        auto child           = this->register_service<sight::viz::scene3d::adaptor>(service_type);
        child->set_input(image, "data.image", true);
        auto visibility = std::make_shared<data::boolean>(this->visible() && image_visible);
        child->set_input(visibility, "config.visible", true);
        m_child_visibility.emplace(id, std::move(visibility));

        data::transfer_function::sptr tf;
        if(const auto transfer_functions = m_image_transfer_functions.lock(); transfer_functions)
        {
            tf = transfer_functions->get<data::transfer_function>(id);
            if(!tf)
            {
                tf                        = data::transfer_function::create_default_tf(image->type());
                (*transfer_functions)[id] = tf;
            }
        }
        else if(const auto default_tf = m_tf.lock(); default_tf)
        {
            tf = std::make_shared<data::transfer_function>();
            tf->deep_copy(default_tf.get_shared());
        }
        else
        {
            tf = data::transfer_function::create_default_tf(image->type());
        }

        if(is_volume)
        {
            child->set_input(m_preintegration.lock().get_shared(), "volume_rendering.config.pre_integration", true);
            child->set_input(
                m_ambient_occlusion.lock().get_shared(),
                "volume_rendering.config.ambient_occlusion",
                true
            );
            child->set_input(m_color_bleeding.lock().get_shared(), "volume_rendering.config.color_bleeding", true);
            child->set_input(m_shadows.lock().get_shared(), "volume_rendering.config.shadows", true);
            child->set_input(m_widgets.lock().get_shared(), "volume_rendering.config.widgets", true);
            child->set_input(m_sampling.lock().get_shared(), "volume_rendering.config.sampling", true);
            child->set_input(
                m_opacity_correction.lock().get_shared(),
                "volume_rendering.config.opacity_correction",
                true
            );
            child->set_input(
                m_sat_shells_number.lock().get_shared(),
                "volume_rendering.config.sat_shells_number",
                true
            );
            child->set_input(m_sat_shell_radius.lock().get_shared(), "volume_rendering.config.sat_shell_radius", true);
            child->set_input(m_sat_cone_samples.lock().get_shared(), "volume_rendering.config.sat_cone_samples", true);
            child->set_input(
                m_color_bleeding_factor.lock().get_shared(),
                "volume_rendering.config.color_bleeding_factor",
                true
            );
            child->set_input(m_ao_factor.lock().get_shared(), "volume_rendering.config.ao_factor", true);
            child->set_input(m_sat_cone_angle.lock().get_shared(), "volume_rendering.config.sat_cone_angle", true);
            child->set_input(m_sat_size_ratio.lock().get_shared(), "volume_rendering.config.sat_size_ratio", true);

            child->set_input(tf, "data.tf", true);

            auto clipping_it = m_clipping_matrices.try_emplace(
                id,
                std::make_shared<data::matrix4>(data::matrix4::identity())
            ).first;
            child->set_inout(clipping_it->second, "data.clipping_matrix", true);

            auto mask = std::make_shared<data::image>();
            mask->resize(image->size(), core::type::UINT8, data::image::pixel_format_t::gray_scale);
            mask->set_spacing(image->spacing());
            mask->set_origin(image->origin());
            mask->set_orientation(image->orientation());
            {
                const auto dump_lock = mask->dump_lock();
                std::memset(mask->buffer(), 0xFF, mask->size_in_bytes());
            }
            m_masks.emplace(id, mask);

            auto extruded_meshes = m_extruded_meshes.lock().get_shared();
            if(const auto meshes_by_image = m_extruded_meshes_by_image.lock(); meshes_by_image)
            {
                auto meshes = meshes_by_image->get<data::model_series>(id);
                if(!meshes)
                {
                    meshes                 = std::make_shared<data::model_series>();
                    (*meshes_by_image)[id] = meshes;
                }

                extruded_meshes = meshes;
            }

            if(extruded_meshes)
            {
                auto extruder = this->register_service(
                    "sight::module::filter::image::image_extruder",
                    this->gen_id("image_extruder_" + id)
                );
                extruder->set_input(extruded_meshes, "input.meshes", true);
                extruder->set_input(image, "input.image", true);
                extruder->set_inout(mask, "output.mask", true);
                extruder->configure(service::config_t {});
                extruder->start().wait();
                extruder->update().wait();
                m_extruders.emplace(id, extruder);
            }

            child->set_input(mask, "data.mask", true);
        }
        else
        {
            child->set_inout(tf, "data.tf", true);
        }

        service::config_t child_config = this->get_config();
        if(m_representation == representation_t::negato2d)
        {
            child_config.put("config.<xmlattr>.orientation", m_orientation);
        }

        if(is_volume)
        {
            child_config.put("config.<xmlattr>.autoresetcamera", reset_camera && image_visible);
        }
        else
        {
            const double depth_bias = child_config.get<double>("config.<xmlattr>.depth_bias", 0.);
            child_config.put(
                "config.<xmlattr>.depth_bias",
                depth_bias + (0.0001 * static_cast<double>(negato_index++))
            );
        }

        child->configure(child_config);
        child->set_id(this->get_id(), child->get_id());
        child->set_render_service(this->render_service());
        child->set_layer_id(this->layer_id());
        m_children.emplace(id, child);

        child->start().wait();

        if(is_volume)
        {
            auto landmarks = this->register_service<sight::viz::scene3d::adaptor>(
                "sight::module::viz::scene3d_qt::adaptor::fiducials::point",
                this->gen_id("landmarks_" + id)
            );

            landmarks->set_inout(image, "data.imageSeries", true);

            service::config_t landmark_config;
            if(const auto config = this->get_config().get_child_optional("landmarks"); config)
            {
                landmark_config = *config;
            }

            landmarks->configure(landmark_config);
            landmarks->set_render_service(this->render_service());
            landmarks->set_layer_id(this->layer_id());
            landmarks->start().wait();
            landmarks->slot("set_image_visibility")->run(id, image_visible);

            m_landmarks.emplace(id, landmarks);

            auto ruler = this->register_service<sight::viz::scene3d::adaptor>(
                "sight::module::viz::scene3d_qt::adaptor::fiducials::ruler",
                this->gen_id("ruler_" + id)
            );

            ruler->set_inout(image, "data.image", true);

            service::config_t ruler_config;
            if(const auto config = this->get_config().get_child_optional("ruler"); config)
            {
                ruler_config = *config;
            }

            ruler->configure(ruler_config);
            ruler->set_render_service(this->render_service());
            ruler->set_layer_id(this->layer_id());
            ruler->start().wait();
            ruler->slot("set_image_visibility")->run(id, image_visible);

            m_rulers.emplace(id, ruler);
        }

        if(image_visible)
        {
            reset_camera = false;
        }
    }

    if(m_representation != representation_t::volume)
    {
        for(const auto& object : *series)
        {
            const auto image = std::dynamic_pointer_cast<data::image_series>(object);
            if(!image || image->get_id().empty())
            {
                continue;
            }

            const auto child = m_children.find(image->get_id());
            if(child == m_children.end())
            {
                continue;
            }

            m_child_connections.connect(
                child->second,
                negato::signals::PICKED_VOXEL,
                this->get_sptr(),
                slots::FORWARD_PICKED_VOXEL
            );
            m_child_connections.connect(
                image,
                data::image::signals::SLICE_INDEX_MODIFIED,
                this->get_sptr(),
                slots::SET_SLICE_INDEX
            );
            m_child_connections.connect(
                image,
                data::image::signals::SLICE_TYPE_MODIFIED,
                this->get_sptr(),
                slots::SET_SLICE_TYPE
            );
        }
    }

    this->set_visible(this->visible());

    m_selection_initialized = true;
    this->update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void image_series::set_visible(bool _visible)
{
    for(const auto& [id, visible] : m_image_visibility)
    {
        this->set_child_visibility(id, _visible && visible);
    }
}

//-----------------------------------------------------------------------------
void image_series::set_child_visibility(const std::string& _image_id, bool _visible)
{
    if(const auto it = m_child_visibility.find(_image_id);
       it != m_child_visibility.end() && it->second->value() != _visible)
    {
        it->second->value() = _visible;
        if(const auto child = m_children.find(_image_id); child != m_children.end())
        {
            child->second->update_visibility(_visible);
        }
    }
}

//-----------------------------------------------------------------------------
void image_series::set_image_visibility(std::string _image_id, bool _visible)
{
    if(m_representation == representation_t::negato2d && _visible)
    {
        m_image_visibility[_image_id] = true;
        for(auto& [id, visible] : m_image_visibility)
        {
            visible = id == _image_id;
        }

        this->set_visible(this->visible());
    }
    else
    {
        m_image_visibility[_image_id] = _visible;
        this->set_visible(this->visible());
    }

    if(const auto landmarks = m_landmarks.find(_image_id); landmarks != m_landmarks.end())
    {
        landmarks->second->slot("set_image_visibility")->run(_image_id, _visible);
    }

    if(const auto rulers = m_rulers.find(_image_id); rulers != m_rulers.end())
    {
        rulers->second->slot("set_image_visibility")->run(_image_id, _visible);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

template<typename ... Args>
void image_series::forward_to_children(const std::string& _slot, const Args& ... _args)
{
    for(const auto& [id, child] : m_children)
    {
        const auto visibility = m_image_visibility.find(id);
        const auto child_slot = child->slot(_slot);

        if(visibility != m_image_visibility.end() && visibility->second && child_slot)
        {
            child_slot->run(_args ...);
        }
    }
}

//-----------------------------------------------------------------------------

void image_series::reset_clipping_box()
{
    if(m_representation == representation_t::volume)
    {
        for(const auto& entry : m_clipping_matrices)
        {
            *entry.second = data::matrix4::identity();
            entry.second->async_emit(data::signals::MODIFIED);
        }
    }
}

//-----------------------------------------------------------------------------

void image_series::update_clipping_box()
{
    if(m_representation == representation_t::volume)
    {
        this->forward_to_children(slots::UPDATE_CLIPPING_BOX);
    }
}

//-----------------------------------------------------------------------------

void image_series::update_slices_from_world(double _x, double _y, double _z)
{
    if(m_representation != representation_t::volume)
    {
        this->forward_to_children(slots::UPDATE_SLICES_FROM_WORLD, _x, _y, _z);
    }
}

//-----------------------------------------------------------------------------

void image_series::set_slice_index(int _axial_index, int _frontal_index, int _sagittal_index)
{
    static_cast<void>(_axial_index);
    static_cast<void>(_frontal_index);
    static_cast<void>(_sagittal_index);

    if(m_representation != representation_t::volume)
    {
        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void image_series::set_slice_type(int _from, int _to)
{
    static_cast<void>(_from);
    static_cast<void>(_to);

    if(m_representation != representation_t::volume)
    {
        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void image_series::set_transparency(double _transparency)
{
    if(m_representation == representation_t::negato3d)
    {
        this->forward_to_children(slots::SET_TRANSPARENCY, _transparency);
    }
}

//-----------------------------------------------------------------------------

void image_series::forward_picked_voxel(std::string _text)
{
    this->async_emit(signals::PICKED_VOXEL, std::move(_text));
}

} // namespace sight::module::viz::scene3d::adaptor

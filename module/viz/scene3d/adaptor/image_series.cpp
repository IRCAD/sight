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
    new_slot(slots::TOGGLE_WIDGETS, &image_series::toggle_widgets, this);
    new_slot(slots::SET_BOOL_PARAMETER, &image_series::set_bool_parameter, this);
    new_slot(slots::SET_INT_PARAMETER, &image_series::set_int_parameter, this);
    new_slot(slots::SET_DOUBLE_PARAMETER, &image_series::set_double_parameter, this);
    new_slot(slots::UPDATE_CLIPPING_BOX, &image_series::update_clipping_box, this);
    new_slot(slots::UPDATE_SLICES_FROM_WORLD, &image_series::update_slices_from_world, this);
    new_slot(slots::SET_SLICE_INDEX, &image_series::set_slice_index, this);
    new_slot(slots::SET_SLICE_TYPE, &image_series::set_slice_type, this);
    new_slot(slots::SET_TRANSPARENCY, &image_series::set_transparency, this);
    new_slot(slots::FORWARD_PICKED_VOXEL, &image_series::forward_picked_voxel, this);
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

    for(auto it = m_image_visibility.begin() ; it != m_image_visibility.end() ; )
    {
        if(!active_images.contains(it->first))
        {
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
        for(const auto& [id, child] : m_children)
        {
            const auto visibility = m_image_visibility.find(id);
            if(visibility != m_image_visibility.end())
            {
                child->update_visibility(this->visible() && visibility->second);
            }
        }

        this->update_done();
        this->request_render();
        return;
    }

    m_child_connections.disconnect();
    this->unregister_services();
    m_children.clear();
    m_masks.clear();

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

        if(is_volume)
        {
            if(const auto tf = m_tf.lock(); tf)
            {
                child->set_input(tf.get_shared(), "data.tf", true);
            }

            auto mask = std::make_shared<data::image>();
            mask->resize(image->size(), core::type::UINT8, data::image::pixel_format_t::gray_scale);
            const auto dump_lock = mask->dump_lock();
            std::memset(mask->buffer(), 0xFF, mask->size_in_bytes());
            m_masks.emplace(id, mask);
            child->set_input(mask, "data.mask", true);
        }
        else if(const auto tf = m_tf.lock(); tf)
        {
            child->set_inout(tf.get_shared(), "data.tf", true);
        }

        service::config_t child_config = this->get_config();
        child_config.put("config.<xmlattr>.visible", this->visible() && image_visible);
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

        if(m_representation != representation_t::volume)
        {
            m_child_connections.connect(
                child,
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

        if(image_visible)
        {
            reset_camera = false;
        }
    }

    m_selection_initialized = true;
    this->update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void image_series::set_visible(bool _visible)
{
    for(const auto& [id, child] : m_children)
    {
        child->update_visibility(_visible && m_image_visibility.at(id));
    }
}

//-----------------------------------------------------------------------------
void image_series::set_image_visibility(std::string _image_id, bool _visible)
{
    const auto selected = m_children.find(_image_id);

    if(m_representation == representation_t::negato2d && _visible)
    {
        m_image_visibility[_image_id] = true;
        for(auto& [id, visible] : m_image_visibility)
        {
            visible = id == _image_id;
        }

        for(auto& [id, child] : m_children)
        {
            child->update_visibility(this->visible() && id == _image_id);
        }
    }
    else
    {
        m_image_visibility[_image_id] = _visible;
        if(selected != m_children.end())
        {
            selected->second->update_visibility(this->visible() && _visible);
        }
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

template<typename ... Args>
void image_series::forward_to_children(const std::string& _slot, const Args& ... _args)
{
    for(const auto& [id, child] : m_children)
    {
        if(m_image_visibility.at(id))
        {
            child->slot(_slot)->run(_args ...);
        }
    }
}

//-----------------------------------------------------------------------------

void image_series::toggle_widgets(bool _visible)
{
    if(m_representation == representation_t::volume)
    {
        this->forward_to_children(slots::TOGGLE_WIDGETS, _visible);
    }
}

//-----------------------------------------------------------------------------

void image_series::set_bool_parameter(bool _value, std::string _key)
{
    if(m_representation == representation_t::volume)
    {
        this->forward_to_children(slots::SET_BOOL_PARAMETER, _value, _key);
    }
}

//-----------------------------------------------------------------------------

void image_series::set_int_parameter(int _value, std::string _key)
{
    if(m_representation == representation_t::volume)
    {
        this->forward_to_children(slots::SET_INT_PARAMETER, _value, _key);
    }
}

//-----------------------------------------------------------------------------

void image_series::set_double_parameter(double _value, std::string _key)
{
    if(m_representation == representation_t::volume)
    {
        this->forward_to_children(slots::SET_DOUBLE_PARAMETER, _value, _key);
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

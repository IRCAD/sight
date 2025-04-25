/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "module/viz/scene3d/adaptor/model_series.hpp"

#include "module/viz/scene3d/adaptor/mesh.hpp"
#include "module/viz/scene3d/adaptor/reconstruction.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/material.hpp>
#include <data/matrix4.hpp>
#include <data/reconstruction.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

static const core::com::slots::key_t CHANGE_FIELD_SLOT = "changeField";

//------------------------------------------------------------------------------

model_series::model_series() noexcept
{
    new_slot(CHANGE_FIELD_SLOT, &model_series::show_reconstructions_on_field_changed, this);
}

//------------------------------------------------------------------------------

void model_series::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );

    static const std::string s_AUTORESET_CAMERA_CONFIG  = CONFIG + "autoresetcamera";
    static const std::string s_MATERIAL_TEMPLATE_CONFIG = CONFIG + "material_template";
    static const std::string s_DYNAMIC_CONFIG           = CONFIG + "dynamic";
    static const std::string s_DYNAMIC_VERTICES_CONFIG  = CONFIG + "dynamicVertices";
    static const std::string s_QUERY_CONFIG             = CONFIG + "queryFlags";

    m_auto_reset_camera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    m_material_template_name = config.get<std::string>(s_MATERIAL_TEMPLATE_CONFIG, m_material_template_name);
    m_is_dynamic             = config.get<bool>(s_DYNAMIC_CONFIG, m_is_dynamic);
    m_is_dynamic_vertices    = config.get<bool>(s_DYNAMIC_VERTICES_CONFIG, m_is_dynamic_vertices);

    if(config.count(s_QUERY_CONFIG) != 0U)
    {
        const auto hexa_mask = config.get<std::string>(s_QUERY_CONFIG);
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_flags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    if(config.get_optional<bool>(CONFIG + "visible") || config.get_optional<bool>("properties.<xmlattr>.visible"))
    {
        m_is_visible_tag = true;
    }
}

//------------------------------------------------------------------------------

void model_series::starting()
{
    adaptor::init();

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t model_series::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(MODEL_INPUT, data::model_series::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(MODEL_INPUT, data::model_series::RECONSTRUCTIONS_ADDED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(MODEL_INPUT, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(MODEL_INPUT, data::model_series::ADDED_FIELDS_SIG, CHANGE_FIELD_SLOT);
    connections.push(MODEL_INPUT, data::model_series::REMOVED_FIELDS_SIG, CHANGE_FIELD_SLOT);
    connections.push(MODEL_INPUT, data::model_series::CHANGED_FIELDS_SIG, CHANGE_FIELD_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void model_series::updating()
{
    // Retrieves the associated Sight ModelSeries object
    const auto model_series = m_model.lock();

    this->unregister_services();

    for(const auto& reconstruction : model_series->get_reconstruction_db())
    {
        auto adaptor = this->register_service<module::viz::scene3d::adaptor::reconstruction>(
            "sight::module::viz::scene3d::adaptor::reconstruction"
        );
        adaptor->set_input(reconstruction, "reconstruction", true);

        bool is_visible = visible();
        if(const auto visibility_field = model_series->get_field("ShowReconstructions"); visibility_field)
        {
            const bool show_rec = std::dynamic_pointer_cast<sight::data::boolean>(visibility_field)->value();
            is_visible = is_visible && show_rec;
        }

        config_t rec_adaptor_config;
        rec_adaptor_config.put("properties.<xmlattr>.visible", is_visible);
        rec_adaptor_config.put("config.<xmlattr>.material_template", m_material_template_name);

        if(m_uniforms.size() > 0)
        {
            std::size_t i = 0;
            for(const auto& uniform_data : m_uniforms)
            {
                adaptor->set_inout(uniform_data.second->lock().get_shared(), "uniforms", true, {}, i++);
            }

            const auto config = this->get_config();
            if(const auto inouts_cfg = config.get_child_optional("inout"); inouts_cfg.has_value())
            {
                const auto group = inouts_cfg->get<std::string>("<xmlattr>.group");
                if(group == "uniforms")
                {
                    rec_adaptor_config.add_child("inout", inouts_cfg.value());
                }
            }
        }

        adaptor->configure(rec_adaptor_config);

        // We use the default service ID to get a unique number because a ModelSeries contains several Reconstructions
        adaptor->set_id(this->get_id(), adaptor->get_id());

        adaptor->set_render_service(this->render_service());
        adaptor->set_layer_id(m_layer_id);
        adaptor->set_transform_id(this->get_transform_id());
        adaptor->set_auto_reset_camera(m_auto_reset_camera);
        adaptor->set_query_flags(m_query_flags);

        adaptor->start();

        module::viz::scene3d::adaptor::mesh::sptr mesh_adaptor = adaptor->get_mesh_adaptor();
        mesh_adaptor->set_dynamic(m_is_dynamic);
        mesh_adaptor->set_dynamic_vertices(m_is_dynamic_vertices);
    }

    this->update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void model_series::stopping()
{
    this->unregister_services();

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void model_series::set_visible(bool _visible)
{
    auto adaptors = this->get_registered_services();
    for(const auto& adaptor : adaptors)
    {
        auto rec_adaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::reconstruction>(adaptor.lock());
        rec_adaptor->update_visibility(_visible);
    }
}

//------------------------------------------------------------------------------

void model_series::show_reconstructions_on_field_changed()
{
    const auto model_series = m_model.lock();

    const bool show_rec =
        model_series->get_field("ShowReconstructions", std::make_shared<data::boolean>(true))->value();

    auto adaptors = this->get_registered_services();
    for(const auto& adaptor : adaptors)
    {
        auto rec_adaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::reconstruction>(adaptor.lock());
        rec_adaptor->update_visibility(show_rec);
    }
}

} // namespace sight::module::viz::scene3d::adaptor.

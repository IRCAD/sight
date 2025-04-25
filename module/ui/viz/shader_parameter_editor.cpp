/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "shader_parameter_editor.hpp"

#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/reconstruction.hpp>

#include <service/op.hpp>

#include <ui/__/registry.hpp>

#include <viz/scene3d/adaptor.hpp>

#include <module/ui/viz/helper/parameter_editor.hpp>

#include <QWidget>

namespace sight::module::ui::viz
{

//------------------------------------------------------------------------------
shader_parameter_editor::shader_parameter_editor() noexcept =
    default;

//------------------------------------------------------------------------------

shader_parameter_editor::~shader_parameter_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void shader_parameter_editor::starting()
{
    {
        const auto rec                = m_reconstruction.lock();
        data::material::sptr material = rec->get_material();
        m_connections.connect(material, data::material::MODIFIED_SIG, this->get_sptr(), service::slots::UPDATE);
    }

    this->create();

    const QString service_id = QString::fromStdString(base_id());

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(service_id);

    m_sizer = new QVBoxLayout();
    m_sizer->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(m_sizer);

    this->updating();
}

//------------------------------------------------------------------------------

void shader_parameter_editor::stopping()
{
    m_connections.disconnect();
    this->clear();
    this->destroy();
}

//------------------------------------------------------------------------------

void shader_parameter_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void shader_parameter_editor::updating()
{
    this->clear();
    this->update_gui_info();
    this->fill_gui();
}

//------------------------------------------------------------------------------
void shader_parameter_editor::clear()
{
    m_editor_info.connections.disconnect();

    service::base::sptr obj_service = m_editor_info.srv.lock();

    if(obj_service)
    {
        obj_service->stop();

        sight::ui::registry::unregister_sid_container(m_editor_info.uuid);

        sight::service::remove(obj_service);

        m_sizer->removeWidget(m_editor_info.editor_panel->get_qt_container());
        m_editor_info.editor_panel->destroy_container();
        m_editor_info.editor_panel.reset();
    }
}

//------------------------------------------------------------------------------

void shader_parameter_editor::update_gui_info()
{
    /// Getting all Material adaptors
    const auto reconstruction = m_reconstruction.lock();

    const auto srv_vec = sight::service::get_services("sight::module::viz::scene3d::adaptor::material");

    /// Stop if no Material adaptors have been find
    if(srv_vec.empty())
    {
        SIGHT_WARN("No module::viz::scene3d::adaptor::material found in the application");
        return;
    }

    /// Try to find the material adaptor working with the same data::material
    /// as the one contained by the current reconstruction
    sight::viz::scene3d::adaptor::sptr mat_service;
    for(const auto& srv : srv_vec)
    {
        if(srv->inout("material").lock()->get_id() == reconstruction->get_material()->get_id())
        {
            mat_service = std::dynamic_pointer_cast<sight::viz::scene3d::adaptor>(srv);
            break;
        }
    }

    SIGHT_ASSERT("Material adaptor corresponding to the current Reconstruction must exist", mat_service);

    bool found = false;

    // Is there at least one parameter that we can handle ?
    for(const auto& w_param_srv : mat_service->get_registered_services())
    {
        const auto param_srv = w_param_srv.lock();
        if(param_srv->get_classname() == "sight::module::viz::scene3d::adaptor::shader_parameter")
        {
            /// filter object types
            const auto shader_obj =
                param_srv->inout(sight::viz::scene3d::parameter_adaptor::PARAMETER_INOUT).lock();
            const object_classname_t obj_type = shader_obj->get_classname();

            if(obj_type == "sight::data::boolean" || obj_type == "sight::data::real"
               || obj_type == "sight::data::integer" || obj_type == "sight::data::array")
            {
                found = true;
                break;
            }
        }
    }

    if(!found)
    {
        return;
    }

    /// Getting this widget's container
    auto qt_container  = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QWidget* container = qt_container->get_qt_container();

    auto* p2 = new QWidget(container);
    m_editor_info.editor_panel = sight::ui::qt::container::widget::make();
    m_editor_info.editor_panel->set_qt_container(p2);

    const std::string uuid = this->get_id();
    m_editor_info.uuid = uuid + "-editor";

    sight::ui::registry::register_sid_container(m_editor_info.uuid, m_editor_info.editor_panel);

    auto editor_service = sight::service::add("sight::module::ui::qt::parameters", m_editor_info.uuid);
    m_editor_info.srv = editor_service;

    service::config_t editor_config;

    // Get all ShaderParameter subservices from the corresponding Material adaptor
    for(const auto& w_adaptor : mat_service->get_registered_services())
    {
        const auto adaptor = w_adaptor.lock();
        if(adaptor->get_classname() == "sight::module::viz::scene3d::adaptor::shader_parameter")
        {
            auto param_adaptor = std::dynamic_pointer_cast<sight::viz::scene3d::parameter_adaptor>(adaptor);
            auto param_config  = module::ui::viz::helper::parameter_editor::create_config(
                param_adaptor,
                m_editor_info.srv.lock(),
                m_editor_info.connections
            );

            if(!param_config.empty())
            {
                editor_config.add_child("parameters.param", param_config);
            }
        }
    }

    editor_service->set_config(editor_config);
    editor_service->configure();

    editor_service->start();
}

//------------------------------------------------------------------------------

void shader_parameter_editor::fill_gui()
{
    auto editor_service = m_editor_info.srv.lock();
    if(editor_service)
    {
        m_sizer->addWidget(m_editor_info.editor_panel->get_qt_container(), 0);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz

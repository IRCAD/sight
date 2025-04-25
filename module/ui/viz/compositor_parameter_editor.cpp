/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "compositor_parameter_editor.hpp"

#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/render.hpp>

#include <module/ui/viz/helper/parameter_editor.hpp>

#include <QWidget>

namespace sight::module::ui::viz
{

const core::com::slots::key_t compositor_parameter_editor::UPDATE_COMPOSITOR_SLOT = "updateCompositor";

//------------------------------------------------------------------------------
compositor_parameter_editor::compositor_parameter_editor() noexcept
{
    new_slot(UPDATE_COMPOSITOR_SLOT, &compositor_parameter_editor::update_compositor, this);
}

//------------------------------------------------------------------------------

compositor_parameter_editor::~compositor_parameter_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void compositor_parameter_editor::configuring()
{
    this->initialize();

    auto config = this->get_config();

    m_layer_id = config.get<std::string>("layer.<xmlattr>.id", "");
}

//------------------------------------------------------------------------------

void compositor_parameter_editor::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    m_sizer = new QVBoxLayout();
    m_sizer->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(m_sizer);

    this->updating();
}

//------------------------------------------------------------------------------

void compositor_parameter_editor::stopping()
{
    this->clear();
    this->destroy();
}

//------------------------------------------------------------------------------

void compositor_parameter_editor::updating()
{
}

//------------------------------------------------------------------------------

void compositor_parameter_editor::update_compositor(
    std::string /*_compositorName*/,
    bool /*_enabled*/,
    sight::viz::scene3d::layer::sptr _layer
)
{
    if(_layer->layer_id() == m_layer_id)
    {
        // We will create a new layout so clear everything before
        this->clear();

        bool found = false;

        const auto adaptors = _layer->get_registered_adaptors();

        // Is there at least one parameter that we can handle ?
        for(const auto& w_adaptor : adaptors)
        {
            const auto adaptor = w_adaptor.lock();
            if(adaptor->get_classname() == "sight::module::viz::scene3d::adaptor::compositor_parameter")
            {
                /// filter object types
                const auto shader_obj =
                    adaptor->inout(sight::viz::scene3d::parameter_adaptor::PARAMETER_INOUT).lock();
                const auto& obj_type = shader_obj->get_classname();

                if(obj_type == "sight::data::boolean" || obj_type == "sight::data::real"
                   || obj_type == "sight::data::integer")
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
        for(const auto& w_adaptor : adaptors)
        {
            const auto adaptor = w_adaptor.lock();
            if(adaptor->get_classname() == "sight::module::viz::scene3d::adaptor::compositor_parameter")
            {
                auto param_adaptor = std::dynamic_pointer_cast<const sight::viz::scene3d::parameter_adaptor>(adaptor);
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

        this->fill_gui();
    }
}

//------------------------------------------------------------------------------

void compositor_parameter_editor::clear()
{
    service::base::sptr obj_service = m_editor_info.srv.lock();

    if(obj_service)
    {
        obj_service->stop();

        sight::ui::registry::unregister_sid_container(m_editor_info.uuid);

        sight::service::unregister_service(obj_service);

        m_sizer->removeWidget(m_editor_info.editor_panel->get_qt_container());
        m_editor_info.editor_panel->destroy_container();
        m_editor_info.editor_panel.reset();
    }
}

//------------------------------------------------------------------------------

void compositor_parameter_editor::fill_gui()
{
    auto editor_service = m_editor_info.srv.lock();
    if(editor_service)
    {
        m_sizer->addWidget(m_editor_info.editor_panel->get_qt_container(), 0);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz

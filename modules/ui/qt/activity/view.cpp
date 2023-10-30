/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "view.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

#include <service/extension/config.hpp>
#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::activity
{

const core::com::signals::key_t ACTIVITY_LAUNCHED_SIG = "activityLaunched";

static const std::string BORDER_CONFIG = "border";

//------------------------------------------------------------------------------

view::view() :
    m_sig_activity_launched(new_signal<activity_launched_signal_t>(ACTIVITY_LAUNCHED_SIG))
{
}

//------------------------------------------------------------------------------

view::~view()
= default;

//-----------------------------------------------------------------------------

void view::configuring()
{
    this->sight::ui::activity_view::configuring();

    const config_t config_t = this->get_config();
    const auto config       = config_t.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_border = config->get<int>(BORDER_CONFIG, m_border);
    }
}

//------------------------------------------------------------------------------

void view::starting()
{
    this->sight::ui::service::create();

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto parent_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    parent_container->get_qt_container()->setObjectName(service_id);

    auto* layout = new QVBoxLayout();
    if(m_border >= 0)
    {
        layout->setContentsMargins(m_border, m_border, m_border, m_border);
    }

    auto* widget = new QWidget();
    layout->addWidget(widget);
    widget->setObjectName(service_id + "/container");

    auto sub_container = sight::ui::qt::container::widget::make();

    sub_container->set_qt_container(widget);
    m_wid = this->get_id() + "_container";
    sight::ui::registry::register_wid_container(m_wid, sub_container);

    parent_container->set_layout(layout);

    m_config_manager = sight::app::config_manager::make();

    if(!m_main_activity_id.empty())
    {
        data::activity::sptr activity = this->create_main_activity();
        if(activity)
        {
            this->launch_activity(activity);
        }
    }
}

//------------------------------------------------------------------------------

void view::stopping()
{
    if(m_config_manager && m_config_manager->started())
    {
        m_config_manager->stop_and_destroy();
    }

    auto sub_container = sight::ui::registry::get_wid_container(m_wid);
    sight::ui::registry::unregister_wid_container(m_wid);

    sub_container->destroy_container();

    this->destroy();
}

//------------------------------------------------------------------------------

void view::updating()
{
}

//------------------------------------------------------------------------------

void view::launch_activity(data::activity::sptr _activity)
{
    if(this->validate_activity(_activity))
    {
        if(m_config_manager->started())
        {
            m_config_manager->stop_and_destroy();
        }

        auto [info, replacementMap] = sight::activity::extension::activity::get_default()->get_info_and_replacement_map(
            *_activity,
            m_parameters
        );

        replacementMap["WID_PARENT"]  = m_wid;
        replacementMap["GENERIC_UID"] = sight::app::extension::config::get_unique_identifier(info.app_config.id);

        try
        {
            m_config_manager->set_config(info.app_config.id, replacementMap);
            m_config_manager->launch();

            m_sig_activity_launched->async_emit(_activity);
        }
        catch(std::exception& e)
        {
            sight::ui::dialog::message::show(
                "Activity launch failed",
                e.what(),
                sight::ui::dialog::message::critical
            );
            SIGHT_ERROR(e.what());
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity

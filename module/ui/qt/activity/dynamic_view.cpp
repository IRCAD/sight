/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "dynamic_view.hpp"

#include <app/extension/config.hpp>

#include <algorithm>

#include <ui/__/dialog/message.hpp>
#include <ui/__/registry.hpp>

#include <QBoxLayout>

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

dynamic_view::dynamic_view() noexcept
{
    new_slot(slots::CREATE_TAB, &dynamic_view::create_tab, this);

    new_signal<signals::activity_selected_t>(signals::ACTIVITY_SELECTED);
    new_signal<signals::nothing_selected_t>(signals::NOTHING_SELECTED);
}

//------------------------------------------------------------------------------

dynamic_view::~dynamic_view() noexcept =
    default;

//------------------------------------------------------------------------------

void dynamic_view::configuring()
{
    this->sight::ui::activity_view::configuring();

    const auto& config = this->get_config();

    m_main_activity_closable = config.get<bool>("mainActivity.<xmlattr>.closable", m_main_activity_closable);
    m_document_mode          = config.get<bool>("config.<xmlattr>.document", m_document_mode);
}

//------------------------------------------------------------------------------

void dynamic_view::starting()
{
    this->sight::ui::service::create();

    auto parent_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    m_tab_widget = new QTabWidget();
    m_tab_widget->setTabsClosable(true);
    m_tab_widget->setDocumentMode(m_document_mode);
    m_tab_widget->setMovable(true);

    QObject::connect(m_tab_widget, &QTabWidget::tabCloseRequested, this, &dynamic_view::close_tab_signal);
    QObject::connect(m_tab_widget, &QTabWidget::currentChanged, this, &dynamic_view::changed_tab);

    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget(m_tab_widget);

    parent_container->set_layout(layout);

    m_current_widget = nullptr;

    if(!m_main_activity_id.empty())
    {
        this->build_main_activity();
    }
}

//------------------------------------------------------------------------------

void dynamic_view::stopping()
{
    while(m_tab_widget->count() != 0)
    {
        this->close_tab(0, true);
    }

    m_tab_widget->clear();

    this->destroy();
    m_tab_widget = nullptr;
}

//------------------------------------------------------------------------------

void dynamic_view::updating()
{
}

//------------------------------------------------------------------------------

void dynamic_view::launch_activity(data::activity::sptr _activity)
{
    if(this->validate_activity(_activity))
    {
        dynamic_view_info view_info = this->create_view_info(_activity);
        view_info.closable = true;

        this->launch_tab(view_info);
    }
}

//------------------------------------------------------------------------------

void dynamic_view::create_tab(sight::activity::message _info)
{
    dynamic_view_info view_info;
    view_info.title           = _info.get_title();
    view_info.tab_id          = _info.get_tab_id();
    view_info.closable        = _info.is_closable();
    view_info.icon            = _info.get_icon_path();
    view_info.tooltip         = _info.get_tool_tip();
    view_info.view_config_id  = _info.get_app_config_id();
    view_info.replacement_map = _info.get_replacement_map();
    view_info.activity        = _info.get_activity();

    this->launch_tab(view_info);
}

//------------------------------------------------------------------------------

void dynamic_view::launch_tab(dynamic_view_info& _info)
{
    static int count = 0;
    auto iter        = std::ranges::find(m_activity_ids, _info.activity->get_id());

    if(iter != m_activity_ids.end())
    {
        sight::ui::dialog::message::show(
            "Launch Activity",
            "The current activity is already launched. \n"
            "It cannot be launched twice.",
            sight::ui::dialog::message::warning
        );
        return;
    }

    if(m_title_to_count.contains(_info.title))
    {
        m_title_to_count[_info.title]++;
    }
    else
    {
        m_title_to_count[_info.title] = 1;
    }

    QString final_title = QString("%1 %2").arg(_info.title.c_str(), "(%1)").arg(m_title_to_count[_info.title]);
    _info.wid = QString("dynamic_view-%1").arg(count++).toStdString();

    auto sub_container = sight::ui::qt::container::widget::make();
    auto* widget       = new QWidget(m_tab_widget);
    sub_container->set_qt_container(widget);
    sight::ui::registry::register_wid_container(_info.wid, sub_container);

    _info.replacement_map["WID_PARENT"]  = _info.wid;
    _info.replacement_map["GENERIC_UID"] = sight::app::extension::config::get_unique_identifier(_info.view_config_id);

    auto helper = sight::app::config_manager::make();

    try
    {
        _info.local_value_objects = sight::service::materialize_value_parameters(
            m_value_parameters,
            _info.replacement_map,
            [&_info](const std::string& _key) -> std::string
            {
                const auto object_parameter =
                    app::extension::config::get()->get_object_parameter(_info.view_config_id, _key);
                return object_parameter.has_value() ? object_parameter->type : std::string {};
            },
            [](const std::string& _key) -> std::string
            {
                return app::extension::config::get_unique_identifier("activity_value_" + _key);
            },
            _info.view_config_id
        );
        helper->set_config(_info.view_config_id, _info.replacement_map);
        if(!m_dynamic_config_start_stop)
        {
            helper->launch();
        }
        else
        {
            helper->create();
        }
    }
    catch(std::exception& e)
    {
        sight::ui::dialog::message::show(
            "Activity launch failed",
            e.what(),
            sight::ui::dialog::message::critical
        );
        SIGHT_ERROR(e.what());
        return;
    }

    _info.container = sub_container;
    _info.helper    = helper;
    m_activity_ids.insert(_info.activity->get_id());

    m_dynamic_info_map[widget] = _info;
    m_tab_id_list.insert(_info.tab_id);

    int index = m_tab_widget->addTab(widget, final_title);
    if(!_info.tooltip.empty())
    {
        m_tab_widget->setTabToolTip(index, QString::fromStdString(_info.tooltip));
    }

    if(!_info.icon.empty())
    {
        m_tab_widget->setTabIcon(index, QIcon(QString::fromStdString(_info.icon)));
    }

    m_tab_widget->setCurrentWidget(widget);
}

//------------------------------------------------------------------------------

void dynamic_view::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void dynamic_view::close_tab_signal(int _index)
{
    close_tab(_index, false);
}

//------------------------------------------------------------------------------

void dynamic_view::close_tab(int _index, bool _force_close)
{
    QWidget* widget = m_tab_widget->widget(_index);

    SIGHT_ASSERT("Widget is not in dynamicInfoMap", m_dynamic_info_map.contains(widget));
    dynamic_view_info info = m_dynamic_info_map[widget];
    if(info.closable || _force_close)
    {
        m_tab_id_list.erase(info.tab_id);
        if(!m_dynamic_config_start_stop)
        {
            info.helper->stop_and_destroy();
        }
        else
        {
            if(info.helper->started())
            {
                info.helper->stop();
            }

            info.helper->destroy();
        }

        info.helper.reset();

        //Remove tab first, to avoid tab beeing removed by container->destroy
        m_current_widget = nullptr;
        m_tab_widget->removeTab(_index);

        sight::ui::registry::unregister_wid_container(info.wid);

        info.container->destroy_container();
        info.container.reset();
        m_dynamic_info_map.erase(widget);
        m_activity_ids.erase(info.activity->get_id());
    }
    else
    {
        sight::ui::dialog::message::show(
            "Close tab",
            "The tab " + info.title + " can not be closed.",
            sight::ui::dialog::message::info
        );
    }
}

//------------------------------------------------------------------------------

void dynamic_view::changed_tab(int _index)
{
    QWidget* widget = m_tab_widget->widget(_index);

    if(m_dynamic_config_start_stop && widget != m_current_widget)
    {
        if(m_current_widget != nullptr)
        {
            dynamic_view_info oldinfo = m_dynamic_info_map[m_current_widget];
            oldinfo.helper->stop();
        }

        if(widget != nullptr)
        {
            dynamic_view_info newinfo = m_dynamic_info_map[widget];
            if(!newinfo.helper->started())
            {
                newinfo.helper->start();
                newinfo.helper->update();
            }
        }
    }

    m_current_widget = widget;

    if(_index >= 0)
    {
        dynamic_view_info info = m_dynamic_info_map[widget];
        this->async_emit(signals::ACTIVITY_SELECTED, info.activity);
    }
    else
    {
        this->async_emit(signals::NOTHING_SELECTED);
    }
}

//------------------------------------------------------------------------------

void dynamic_view::build_main_activity()
{
    auto activity = this->create_main_activity();

    if(activity)
    {
        dynamic_view_info view_info;
        view_info          = this->create_view_info(activity);
        view_info.closable = m_main_activity_closable;

        this->launch_tab(view_info);
    }
}

//------------------------------------------------------------------------------

dynamic_view::dynamic_view_info dynamic_view::create_view_info(data::activity::sptr _activity)
{
    auto [info, replacementMap] = sight::activity::extension::activity::get_default()->get_info_and_replacement_map(
        *_activity,
        m_parameters
    );

    dynamic_view_info view_info;
    view_info.title           = info.title;
    view_info.icon            = info.icon;
    view_info.tooltip         = info.tab_info.empty() ? info.title : info.tab_info;
    view_info.view_config_id  = info.app_config.id;
    view_info.activity        = _activity;
    view_info.replacement_map = replacementMap;

    return view_info;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity

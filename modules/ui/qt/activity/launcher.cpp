/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "launcher.hpp"

#include <activity/builder/base.hpp>
#include <activity/validator/activity.hpp>
#include <activity/validator/base.hpp>

#include <app/config_manager.hpp>

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/runtime.hpp>

#include <data/activity.hpp>

#include <service/extension/config.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <boost/foreach.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVBoxLayout>

Q_DECLARE_METATYPE(sight::activity::extension::activity_info)

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

const core::com::slots::key_t launcher::LAUNCH_SERIES_SLOT      = "launchSeries";
const core::com::slots::key_t launcher::LAUNCH_ACTIVITY_SLOT    = "launchActivity";
const core::com::slots::key_t launcher::UPDATE_STATE_SLOT       = "updateState";
const core::com::signals::key_t launcher::ACTIVITY_LAUNCHED_SIG = "activityLaunched";

using sight::activity::extension::activity;
using sight::activity::extension::activity_info;
using sight::activity::message;
using sight::activity::validator::base;
using sight::activity::validator::base;

//------------------------------------------------------------------------------

launcher::launcher() noexcept :
    m_sig_activity_launched(new_signal<activity_launched_signal_t>(ACTIVITY_LAUNCHED_SIG)),
    m_mode("message")
{
    new_slot(LAUNCH_SERIES_SLOT, &launcher::launch_series, this);
    new_slot(LAUNCH_ACTIVITY_SLOT, &launcher::launch_activity, this);
    new_slot(UPDATE_STATE_SLOT, &launcher::update_state, this);
}

//------------------------------------------------------------------------------

launcher::~launcher() noexcept =
    default;

//------------------------------------------------------------------------------

void launcher::starting()
{
    this->action_service_starting();
    this->update_state();
}

//------------------------------------------------------------------------------

void launcher::stopping()
{
    this->action_service_stopping();
}

//------------------------------------------------------------------------------

void launcher::configuring()
{
    this->sight::ui::action::initialize();
    using config_t = service::config_t;

    m_parameters.clear();
    if(this->get_config().count("config") > 0)
    {
        SIGHT_ASSERT(
            "There must be one (and only one) <config/> element.",
            this->get_config().count("config") == 1
        );

        const service::config_t srvconfig = this->get_config();
        const service::config_t& config   = srvconfig.get_child("config");

        m_mode = config.get_optional<std::string>("mode").get_value_or("message");
        SIGHT_ASSERT(
            "launcher mode must be either 'immediate' or 'message'",
            "message" == m_mode || "immediate" == m_mode
        );

        if(config.count("parameters") == 1)
        {
            const service::config_t& config_parameters = config.get_child("parameters");
            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const config_t::value_type& v, config_parameters.equal_range("parameter"))
            {
                parameters_t::value_type parameter(v.second);
                m_parameters.push_back(parameter);
            }
        }

        SIGHT_ASSERT("A maximum of 1 <parameters> tag is allowed", config.count("parameters") < 2);

        if(config.count("filter") == 1)
        {
            const service::config_t& config_filter = config.get_child("filter");
            SIGHT_ASSERT("A maximum of 1 <mode> tag is allowed", config_filter.count("mode") < 2);

            const auto mode = config_filter.get<std::string>("mode");
            SIGHT_ASSERT(
                "'" << mode << "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
                mode == "include" || mode == "exclude"
            );
            m_filter_mode = mode;

            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const config_t::value_type& v, config_filter.equal_range("id"))
            {
                m_keys.push_back(v.second.get<std::string>(""));
            }
        }

        SIGHT_ASSERT("A maximum of 1 <filter> tag is allowed", config.count("filter") < 2);

        if(config.count("quickLaunch") == 1)
        {
            m_quick_launch.clear();
            const service::config_t& config_quick_launch = config.get_child("quickLaunch");
            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const config_t::value_type& v, config_quick_launch.equal_range("association"))
            {
                const service::config_t& association = v.second;
                const service::config_t xmlattr      = association.get_child("<xmlattr>");

                SIGHT_FATAL_IF("The attribute \"type\" is missing", xmlattr.count("type") != 1);
                SIGHT_FATAL_IF("The attribute \"id\" is missing", xmlattr.count("id") != 1);

                auto type = xmlattr.get<std::string>("type");
                auto id   = xmlattr.get<std::string>("id");

                m_quick_launch[type] = id;
            }
        }

        SIGHT_ASSERT("A maximum of 1 <quickLaunch> tag is allowed", config.count("quickLaunch") < 2);
    }
}

//------------------------------------------------------------------------------

activity_info launcher::show(const activity_infos_t& _infos)
{
    QWidget* parent = qApp->activeWindow();

    auto* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString("Choose an activity"));

    auto* model = new QStandardItemModel(dialog);
    for(const activity_info& info : _infos)
    {
        std::string text;
        if(info.title.empty())
        {
            text = info.id;
        }
        else
        {
            text = info.title + (info.description.empty() ? "" : "\n" + info.description);
        }

        auto* item = new QStandardItem(QIcon(info.icon.c_str()), QString::fromStdString(text));
        item->setData(QVariant::fromValue(info));
        item->setEditable(false);
        model->appendRow(item);
    }

    auto* selection_list = new QListView();
    selection_list->setIconSize(QSize(100, 100));
    selection_list->setUniformItemSizes(true);
    selection_list->setModel(model);

    QModelIndex index = model->index(0, 0);
    if(index.isValid())
    {
        selection_list->selectionModel()->select(index, QItemSelectionModel::Select);
    }

    auto* ok_button     = new QPushButton("Ok");
    auto* cancel_button = new QPushButton("Cancel");

    auto* h_layout = new QHBoxLayout();
    h_layout->addWidget(ok_button);
    h_layout->addWidget(cancel_button);

    auto* v_layout = new QVBoxLayout();
    v_layout->addWidget(selection_list);
    v_layout->addLayout(h_layout);

    dialog->setLayout(v_layout);
    QObject::connect(ok_button, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancel_button, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selection_list, SIGNAL(doubleClicked(const QModelIndex&)), dialog, SLOT(accept()));

    activity_info info;
    if(dialog->exec() != 0)
    {
        QModelIndex current_index = selection_list->selectionModel()->currentIndex();
        QStandardItem* item       = model->itemFromIndex(current_index);
        QVariant var              = item->data();
        info = var.value<activity_info>();
    }

    return info;
}

//------------------------------------------------------------------------------

launcher::activity_infos_t launcher::get_enabled_activities(const activity_infos_t& _infos)
{
    activity_infos_t configs;

    if(m_filter_mode == "include" || m_filter_mode == "exclude")
    {
        const bool is_include_mode = m_filter_mode == "include";

        for(const auto& info : _infos)
        {
            auto key_it = std::find(m_keys.begin(), m_keys.end(), info.id);

            if((key_it != m_keys.end() && is_include_mode) || (key_it == m_keys.end() && !is_include_mode))
            {
                configs.push_back(info);
            }
        }
    }
    else
    {
        configs = _infos;
    }

    return configs;
}

//------------------------------------------------------------------------------

void launcher::updating()
{
    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << SERIES << "' is not correctly set.", selection);

    const bool launch_as = this->launch_as(selection.get_shared());
    if(!launch_as)
    {
        activity_infos_t infos = activity::get_default()->get_infos(selection.get_shared());
        infos = this->get_enabled_activities(infos);

        if(!infos.empty())
        {
            activity_info info;
            if((m_keys.size() == 1 && m_filter_mode == "include") || (infos.size() == 1))
            {
                info = infos[0];
            }
            else
            {
                info = this->show(infos);
            }

            if(!info.id.empty())
            {
                this->send_config(info);
            }
        }
        else
        {
            sight::ui::dialog::message::show(
                "Activity launcher",
                "Not available activity for the current selection.",
                sight::ui::dialog::message::warning
            );
        }
    }
}

//------------------------------------------------------------------------------

void launcher::update_state()
{
    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << SERIES << "' is not correctly set.", selection);

    bool is_enabled = false;

    if(selection->size() == 1 && std::dynamic_pointer_cast<data::activity>((*selection)[0]))
    {
        data::activity::sptr as = std::dynamic_pointer_cast<data::activity>((*selection)[0]);

        if(m_filter_mode == "include" || m_filter_mode == "exclude")
        {
            const bool is_include_mode = m_filter_mode == "include";

            auto key_it = std::find(m_keys.begin(), m_keys.end(), as->get_activity_config_id());

            is_enabled = ((key_it != m_keys.end()) != is_include_mode);

            is_enabled &= activity::get_default()->has_info(
                as->get_activity_config_id()
            );
        }
        else
        {
            is_enabled = activity::get_default()->has_info(
                as->get_activity_config_id()
            );
        }
    }
    else
    {
        activity_info::data_count_t data_count;
        data_count = activity::get_default()->get_data_count(selection.get_shared());
        if(m_filter_mode.empty() && data_count.size() == 1)
        {
            data::object::sptr obj = selection->front();
            if(std::dynamic_pointer_cast<data::activity>(obj))
            {
                is_enabled = true;
            }
        }

        activity_infos_t infos = activity::get_default()->get_infos(selection.get_shared());
        infos = this->get_enabled_activities(infos);

        is_enabled |= !infos.empty();
    }

    this->set_enabled(is_enabled);
}

//------------------------------------------------------------------------------

void launcher::build_activity(
    const activity_info& _info,
    const data::vector::csptr& _selection
)
{
    auto builder = sight::activity::builder::factory::make(_info.builder_impl);
    SIGHT_ASSERT(_info.builder_impl << " instantiation failed", builder);

    auto activity = builder->build_data(_info, _selection);

    if(!activity)
    {
        const std::string msg = "The activity <" + _info.title + "> can't be launched. Builder <" + _info.builder_impl
                                + "> failed.";
        sight::ui::dialog::message::show(
            "Activity can not be launched",
            msg,
            sight::ui::dialog::message::warning
        );
        SIGHT_ERROR(msg);
        return;
    }

    // Applies activity validator on activity to check the data
    if(!_info.validators_impl.empty())
    {
        for(const std::string& validator_impl : _info.validators_impl)
        {
            /// Process activity validator
            auto validator = sight::activity::validator::factory::make(validator_impl);

            auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);

            if(activity_validator)
            {
                auto ret = activity_validator->validate(activity);
                if(!ret.first)
                {
                    const std::string message = "The activity '" + _info.title + "' can not be launched:\n"
                                                + ret.second;
                    sight::ui::dialog::message::show(
                        "Activity launch",
                        message,
                        sight::ui::dialog::message::critical
                    );
                    return;
                }
            }
        }
    }

    const message msg(activity, _info, m_parameters);

    if(m_mode == "message")
    {
        m_sig_activity_launched->async_emit(msg);
    }
    else
    {
        sight::ui::lock_action lock(this->get_sptr());

        const std::string& view_config_id = msg.get_app_config_id();
        auto replacement_map              = msg.get_replacement_map();
        replacement_map["GENERIC_UID"] = app::extension::config::get_unique_identifier();

        auto helper = app::config_manager::make();
        helper->set_config(view_config_id, replacement_map);
        helper->launch();
        helper->stop_and_destroy();
    }
}

//------------------------------------------------------------------------------

void launcher::send_config(const activity_info& _info)
{
    // Start module containing the activity if it is not started
    core::runtime::start_module(_info.bundle_id);

    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << SERIES << "' is not correctly set.", selection);

    sight::activity::validator::return_t validation;
    validation.first = true;

    for(auto const& validator_impl : _info.validators_impl)
    {
        auto validator = sight::activity::validator::factory::make(validator_impl);
        SIGHT_ASSERT(validator_impl << " instantiation failed", validator);

        auto valid = validator->validate(_info, selection.get_shared());
        validation.first &= valid.first;
        if(!valid.first)
        {
            validation.second += "\n" + valid.second;
        }
    }

    if(!validation.first)
    {
        sight::ui::dialog::message::show(
            "Activity can not be launched",
            "The activity " + _info.title + " can't be launched. Reason : " + validation.second,
            sight::ui::dialog::message::warning
        );
    }
    else
    {
        this->build_activity(_info, selection.get_shared());
    }
}

//------------------------------------------------------------------------------

bool launcher::launch_as(const data::vector::csptr& _selection)
{
    bool launch_as = false;
    activity_info::data_count_t data_count;
    data_count = activity::get_default()->get_data_count(_selection);
    if(data_count.size() == 1)
    {
        for(const data::object::sptr& obj : *_selection)
        {
            data::activity::sptr as = std::dynamic_pointer_cast<data::activity>(obj);
            if(!as)
            {
                launch_as = false;
                break;
            }

            this->launch_activity(as);
            launch_as = true;
        }
    }

    return launch_as;
}

//------------------------------------------------------------------------------

void launcher::launch_series(data::series::sptr _series)
{
    auto selection = std::make_shared<data::vector>();
    selection->push_back(_series);
    activity_infos_t infos = activity::get_default()->get_infos(selection);

    if(m_quick_launch.find(_series->get_classname()) != m_quick_launch.end())
    {
        std::string activity_id = m_quick_launch[_series->get_classname()];
        SIGHT_ASSERT(
            "Activity information not found for" + activity_id,
            activity::get_default()->has_info(activity_id)
        );
        this->send_config(activity::get_default()->get_info(activity_id));
    }
    else if(!infos.empty())
    {
        this->send_config(infos.front());
    }
    else
    {
        sight::ui::dialog::message::show(
            "Activity launcher",
            "Not available activity for the current selection.",
            sight::ui::dialog::message::warning
        );
    }
}

//------------------------------------------------------------------------------

void launcher::launch_activity(data::activity::sptr _activity)
{
    activity_info info;
    info = activity::get_default()->get_info(_activity->get_activity_config_id());

    // Applies activity validator on activity to check the data
    if(!info.validators_impl.empty())
    {
        for(const std::string& validator_impl : info.validators_impl)
        {
            /// Process activity validator
            auto validator          = sight::activity::validator::factory::make(validator_impl);
            auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);

            if(activity_validator)
            {
                auto ret = activity_validator->validate(_activity);
                if(!ret.first)
                {
                    const std::string message = "The activity '" + info.title + "' can not be launched:\n" + ret.second;
                    sight::ui::dialog::message::show(
                        "Activity launch",
                        message,
                        sight::ui::dialog::message::critical
                    );
                    return;
                }
            }
        }
    }

    m_sig_activity_launched->async_emit(message(_activity, info, m_parameters));
}

//------------------------------------------------------------------------------

service::connections_t launcher::auto_connections() const
{
    connections_t connections;

    connections.push(SERIES, data::vector::ADDED_OBJECTS_SIG, UPDATE_STATE_SLOT);
    connections.push(SERIES, data::vector::REMOVED_OBJECTS_SIG, UPDATE_STATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity

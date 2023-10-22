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
    m_sigActivityLaunched(new_signal<activity_launched_signal_t>(ACTIVITY_LAUNCHED_SIG)),
    m_mode("message")
{
    new_slot(LAUNCH_SERIES_SLOT, &launcher::launchSeries, this);
    new_slot(LAUNCH_ACTIVITY_SLOT, &launcher::launchActivity, this);
    new_slot(UPDATE_STATE_SLOT, &launcher::updateState, this);
}

//------------------------------------------------------------------------------

launcher::~launcher() noexcept =
    default;

//------------------------------------------------------------------------------

void launcher::starting()
{
    this->actionServiceStarting();
    this->updateState();
}

//------------------------------------------------------------------------------

void launcher::stopping()
{
    this->actionServiceStopping();
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
            m_filterMode = mode;

            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const config_t::value_type& v, config_filter.equal_range("id"))
            {
                m_keys.push_back(v.second.get<std::string>(""));
            }
        }

        SIGHT_ASSERT("A maximum of 1 <filter> tag is allowed", config.count("filter") < 2);

        if(config.count("quickLaunch") == 1)
        {
            m_quickLaunch.clear();
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

                m_quickLaunch[type] = id;
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

launcher::activity_infos_t launcher::getEnabledActivities(const activity_infos_t& _infos)
{
    activity_infos_t configs;

    if(m_filterMode == "include" || m_filterMode == "exclude")
    {
        const bool is_include_mode = m_filterMode == "include";

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
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    const bool launch_as = this->launchAS(selection.get_shared());
    if(!launch_as)
    {
        activity_infos_t infos = activity::getDefault()->getInfos(selection.get_shared());
        infos = this->getEnabledActivities(infos);

        if(!infos.empty())
        {
            activity_info info;
            if((m_keys.size() == 1 && m_filterMode == "include") || (infos.size() == 1))
            {
                info = infos[0];
            }
            else
            {
                info = this->show(infos);
            }

            if(!info.id.empty())
            {
                this->sendConfig(info);
            }
        }
        else
        {
            sight::ui::dialog::message::show(
                "Activity launcher",
                "Not available activity for the current selection.",
                sight::ui::dialog::message::WARNING
            );
        }
    }
}

//------------------------------------------------------------------------------

void launcher::updateState()
{
    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    bool is_enabled = false;

    if(selection->size() == 1 && std::dynamic_pointer_cast<data::activity>((*selection)[0]))
    {
        data::activity::sptr as = std::dynamic_pointer_cast<data::activity>((*selection)[0]);

        if(m_filterMode == "include" || m_filterMode == "exclude")
        {
            const bool is_include_mode = m_filterMode == "include";

            auto key_it = std::find(m_keys.begin(), m_keys.end(), as->getActivityConfigId());

            is_enabled = ((key_it != m_keys.end()) != is_include_mode);

            is_enabled &= activity::getDefault()->hasInfo(
                as->getActivityConfigId()
            );
        }
        else
        {
            is_enabled = activity::getDefault()->hasInfo(
                as->getActivityConfigId()
            );
        }
    }
    else
    {
        activity_info::data_count_t data_count;
        data_count = activity::getDefault()->getDataCount(selection.get_shared());
        if(m_filterMode.empty() && data_count.size() == 1)
        {
            data::object::sptr obj = selection->front();
            if(std::dynamic_pointer_cast<data::activity>(obj))
            {
                is_enabled = true;
            }
        }

        activity_infos_t infos = activity::getDefault()->getInfos(selection.get_shared());
        infos = this->getEnabledActivities(infos);

        is_enabled |= !infos.empty();
    }

    this->setEnabled(is_enabled);
}

//------------------------------------------------------------------------------

void launcher::buildActivity(
    const activity_info& _info,
    const data::vector::csptr& _selection
)
{
    auto builder = sight::activity::builder::factory::make(_info.builderImpl);
    SIGHT_ASSERT(_info.builderImpl << " instantiation failed", builder);

    auto activity = builder->buildData(_info, _selection);

    if(!activity)
    {
        const std::string msg = "The activity <" + _info.title + "> can't be launched. Builder <" + _info.builderImpl
                                + "> failed.";
        sight::ui::dialog::message::show(
            "Activity can not be launched",
            msg,
            sight::ui::dialog::message::WARNING
        );
        SIGHT_ERROR(msg);
        return;
    }

    // Applies activity validator on activity to check the data
    if(!_info.validatorsImpl.empty())
    {
        for(const std::string& validator_impl : _info.validatorsImpl)
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
                        sight::ui::dialog::message::CRITICAL
                    );
                    return;
                }
            }
        }
    }

    const message msg(activity, _info, m_parameters);

    if(m_mode == "message")
    {
        m_sigActivityLaunched->async_emit(msg);
    }
    else
    {
        sight::ui::LockAction lock(this->get_sptr());

        const std::string& view_config_id = msg.getAppConfigID();
        auto replacement_map              = msg.getReplacementMap();
        replacement_map["GENERIC_UID"] = app::extension::config::getUniqueIdentifier();

        auto helper = app::config_manager::make();
        helper->setConfig(view_config_id, replacement_map);
        helper->launch();
        helper->stopAndDestroy();
    }
}

//------------------------------------------------------------------------------

void launcher::sendConfig(const activity_info& _info)
{
    // Start module containing the activity if it is not started
    core::runtime::start_module(_info.bundleId);

    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    sight::activity::validator::return_t validation;
    validation.first = true;

    for(auto const& validator_impl : _info.validatorsImpl)
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
            sight::ui::dialog::message::WARNING
        );
    }
    else
    {
        this->buildActivity(_info, selection.get_shared());
    }
}

//------------------------------------------------------------------------------

bool launcher::launchAS(const data::vector::csptr& _selection)
{
    bool launch_as = false;
    activity_info::data_count_t data_count;
    data_count = activity::getDefault()->getDataCount(_selection);
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

            this->launchActivity(as);
            launch_as = true;
        }
    }

    return launch_as;
}

//------------------------------------------------------------------------------

void launcher::launchSeries(data::series::sptr _series)
{
    auto selection = std::make_shared<data::vector>();
    selection->push_back(_series);
    activity_infos_t infos = activity::getDefault()->getInfos(selection);

    if(m_quickLaunch.find(_series->get_classname()) != m_quickLaunch.end())
    {
        std::string activity_id = m_quickLaunch[_series->get_classname()];
        SIGHT_ASSERT(
            "Activity information not found for" + activity_id,
            activity::getDefault()->hasInfo(activity_id)
        );
        this->sendConfig(activity::getDefault()->getInfo(activity_id));
    }
    else if(!infos.empty())
    {
        this->sendConfig(infos.front());
    }
    else
    {
        sight::ui::dialog::message::show(
            "Activity launcher",
            "Not available activity for the current selection.",
            sight::ui::dialog::message::WARNING
        );
    }
}

//------------------------------------------------------------------------------

void launcher::launchActivity(data::activity::sptr _activity)
{
    activity_info info;
    info = activity::getDefault()->getInfo(_activity->getActivityConfigId());

    // Applies activity validator on activity to check the data
    if(!info.validatorsImpl.empty())
    {
        for(const std::string& validator_impl : info.validatorsImpl)
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
                        sight::ui::dialog::message::CRITICAL
                    );
                    return;
                }
            }
        }
    }

    m_sigActivityLaunched->async_emit(message(_activity, info, m_parameters));
}

//------------------------------------------------------------------------------

service::connections_t launcher::auto_connections() const
{
    connections_t connections;

    connections.push(s_SERIES, data::vector::ADDED_OBJECTS_SIG, UPDATE_STATE_SLOT);
    connections.push(s_SERIES, data::vector::REMOVED_OBJECTS_SIG, UPDATE_STATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity

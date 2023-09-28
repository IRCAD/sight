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

#include "SLauncher.hpp"

#include <activity/builder/base.hpp>
#include <activity/validator/activity.hpp>
#include <activity/validator/base.hpp>

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/runtime.hpp>

#include <data/Activity.hpp>

#include <service/app_config_manager.hpp>
#include <service/extension/AppConfig.hpp>

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

Q_DECLARE_METATYPE(sight::activity::extension::ActivityInfo)

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

const core::com::slots::key_t SLauncher::LAUNCH_SERIES_SLOT      = "launchSeries";
const core::com::slots::key_t SLauncher::LAUNCH_ACTIVITY_SLOT    = "launchActivity";
const core::com::slots::key_t SLauncher::UPDATE_STATE_SLOT       = "updateState";
const core::com::signals::key_t SLauncher::ACTIVITY_LAUNCHED_SIG = "activityLaunched";

using sight::activity::extension::Activity;
using sight::activity::extension::ActivityInfo;
using sight::activity::message;
using sight::activity::validator::base;
using sight::activity::validator::base;

//------------------------------------------------------------------------------

SLauncher::SLauncher() noexcept :
    m_sigActivityLaunched(new_signal<ActivityLaunchedSignalType>(ACTIVITY_LAUNCHED_SIG)),
    m_mode("message")
{
    new_slot(LAUNCH_SERIES_SLOT, &SLauncher::launchSeries, this);
    new_slot(LAUNCH_ACTIVITY_SLOT, &SLauncher::launchActivity, this);
    new_slot(UPDATE_STATE_SLOT, &SLauncher::updateState, this);
}

//------------------------------------------------------------------------------

SLauncher::~SLauncher() noexcept =
    default;

//------------------------------------------------------------------------------

void SLauncher::starting()
{
    this->actionServiceStarting();
    this->updateState();
}

//------------------------------------------------------------------------------

void SLauncher::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SLauncher::configuring()
{
    this->sight::ui::action::initialize();
    using ConfigType = service::config_t;

    m_parameters.clear();
    if(this->getConfiguration().count("config") > 0)
    {
        SIGHT_ASSERT(
            "There must be one (and only one) <config/> element.",
            this->getConfiguration().count("config") == 1
        );

        const service::config_t srvconfig = this->getConfiguration();
        const service::config_t& config   = srvconfig.get_child("config");

        m_mode = config.get_optional<std::string>("mode").get_value_or("message");
        SIGHT_ASSERT(
            "SLauncher mode must be either 'immediate' or 'message'",
            "message" == m_mode || "immediate" == m_mode
        );

        if(config.count("parameters") == 1)
        {
            const service::config_t& configParameters = config.get_child("parameters");
            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const ConfigType::value_type& v, configParameters.equal_range("parameter"))
            {
                ParametersType::value_type parameter(v.second);
                m_parameters.push_back(parameter);
            }
        }

        SIGHT_ASSERT("A maximum of 1 <parameters> tag is allowed", config.count("parameters") < 2);

        if(config.count("filter") == 1)
        {
            const service::config_t& configFilter = config.get_child("filter");
            SIGHT_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

            const auto mode = configFilter.get<std::string>("mode");
            SIGHT_ASSERT(
                "'" << mode << "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
                mode == "include" || mode == "exclude"
            );
            m_filterMode = mode;

            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const ConfigType::value_type& v, configFilter.equal_range("id"))
            {
                m_keys.push_back(v.second.get<std::string>(""));
            }
        }

        SIGHT_ASSERT("A maximum of 1 <filter> tag is allowed", config.count("filter") < 2);

        if(config.count("quickLaunch") == 1)
        {
            m_quickLaunch.clear();
            const service::config_t& configQuickLaunch = config.get_child("quickLaunch");
            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const ConfigType::value_type& v, configQuickLaunch.equal_range("association"))
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

ActivityInfo SLauncher::show(const ActivityInfoContainer& infos)
{
    QWidget* parent = qApp->activeWindow();

    auto* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString("Choose an activity"));

    auto* model = new QStandardItemModel(dialog);
    for(const ActivityInfo& info : infos)
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

    auto* selectionList = new QListView();
    selectionList->setIconSize(QSize(100, 100));
    selectionList->setUniformItemSizes(true);
    selectionList->setModel(model);

    QModelIndex index = model->index(0, 0);
    if(index.isValid())
    {
        selectionList->selectionModel()->select(index, QItemSelectionModel::Select);
    }

    auto* okButton     = new QPushButton("Ok");
    auto* cancelButton = new QPushButton("Cancel");

    auto* h_layout = new QHBoxLayout();
    h_layout->addWidget(okButton);
    h_layout->addWidget(cancelButton);

    auto* vLayout = new QVBoxLayout();
    vLayout->addWidget(selectionList);
    vLayout->addLayout(h_layout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selectionList, SIGNAL(doubleClicked(const QModelIndex&)), dialog, SLOT(accept()));

    ActivityInfo info;
    if(dialog->exec() != 0)
    {
        QModelIndex currentIndex = selectionList->selectionModel()->currentIndex();
        QStandardItem* item      = model->itemFromIndex(currentIndex);
        QVariant var             = item->data();
        info = var.value<ActivityInfo>();
    }

    return info;
}

//------------------------------------------------------------------------------

SLauncher::ActivityInfoContainer SLauncher::getEnabledActivities(const ActivityInfoContainer& infos)
{
    ActivityInfoContainer configs;

    if(m_filterMode == "include" || m_filterMode == "exclude")
    {
        const bool isIncludeMode = m_filterMode == "include";

        for(const auto& info : infos)
        {
            auto keyIt = std::find(m_keys.begin(), m_keys.end(), info.id);

            if((keyIt != m_keys.end() && isIncludeMode) || (keyIt == m_keys.end() && !isIncludeMode))
            {
                configs.push_back(info);
            }
        }
    }
    else
    {
        configs = infos;
    }

    return configs;
}

//------------------------------------------------------------------------------

void SLauncher::updating()
{
    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    const bool launchAS = this->launchAS(selection.get_shared());
    if(!launchAS)
    {
        ActivityInfoContainer infos = Activity::getDefault()->getInfos(selection.get_shared());
        infos = this->getEnabledActivities(infos);

        if(!infos.empty())
        {
            ActivityInfo info;
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

void SLauncher::updateState()
{
    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    bool isEnabled = false;

    if(selection->size() == 1 && std::dynamic_pointer_cast<data::Activity>((*selection)[0]))
    {
        data::Activity::sptr as = std::dynamic_pointer_cast<data::Activity>((*selection)[0]);

        if(m_filterMode == "include" || m_filterMode == "exclude")
        {
            const bool isIncludeMode = m_filterMode == "include";

            auto keyIt = std::find(m_keys.begin(), m_keys.end(), as->getActivityConfigId());

            isEnabled = ((keyIt != m_keys.end()) != isIncludeMode);

            isEnabled &= Activity::getDefault()->hasInfo(
                as->getActivityConfigId()
            );
        }
        else
        {
            isEnabled = Activity::getDefault()->hasInfo(
                as->getActivityConfigId()
            );
        }
    }
    else
    {
        ActivityInfo::DataCountType dataCount;
        dataCount = Activity::getDefault()->getDataCount(selection.get_shared());
        if(m_filterMode.empty() && dataCount.size() == 1)
        {
            data::Object::sptr obj = selection->front();
            if(std::dynamic_pointer_cast<data::Activity>(obj))
            {
                isEnabled = true;
            }
        }

        ActivityInfoContainer infos = Activity::getDefault()->getInfos(selection.get_shared());
        infos = this->getEnabledActivities(infos);

        isEnabled |= !infos.empty();
    }

    this->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void SLauncher::buildActivity(
    const ActivityInfo& info,
    const data::Vector::csptr& selection
)
{
    auto builder = sight::activity::builder::factory::make(info.builderImpl);
    SIGHT_ASSERT(info.builderImpl << " instantiation failed", builder);

    auto activity = builder->buildData(info, selection);

    if(!activity)
    {
        const std::string msg = "The activity <" + info.title + "> can't be launched. Builder <" + info.builderImpl
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
    if(!info.validatorsImpl.empty())
    {
        for(const std::string& validatorImpl : info.validatorsImpl)
        {
            /// Process activity validator
            auto validator = sight::activity::validator::factory::make(validatorImpl);

            auto activityValidator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);

            if(activityValidator)
            {
                auto ret = activityValidator->validate(activity);
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

    const message msg(activity, info, m_parameters);

    if(m_mode == "message")
    {
        m_sigActivityLaunched->async_emit(msg);
    }
    else
    {
        sight::ui::LockAction lock(this->get_sptr());

        const std::string& viewConfigID = msg.getAppConfigID();
        auto replacementMap             = msg.getReplacementMap();
        replacementMap["GENERIC_UID"] = service::extension::AppConfig::getUniqueIdentifier();

        auto helper = service::app_config_manager::make();
        helper->setConfig(viewConfigID, replacementMap);
        helper->launch();
        helper->stopAndDestroy();
    }
}

//------------------------------------------------------------------------------

void SLauncher::sendConfig(const ActivityInfo& info)
{
    // Start module containing the activity if it is not started
    core::runtime::start_module(info.bundleId);

    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    sight::activity::validator::return_t validation;
    validation.first = true;

    for(auto const& validatorImpl : info.validatorsImpl)
    {
        auto validator = sight::activity::validator::factory::make(validatorImpl);
        SIGHT_ASSERT(validatorImpl << " instantiation failed", validator);

        auto valid = validator->validate(info, selection.get_shared());
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
            "The activity " + info.title + " can't be launched. Reason : " + validation.second,
            sight::ui::dialog::message::WARNING
        );
    }
    else
    {
        this->buildActivity(info, selection.get_shared());
    }
}

//------------------------------------------------------------------------------

bool SLauncher::launchAS(const data::Vector::csptr& selection)
{
    bool launchAS = false;
    ActivityInfo::DataCountType dataCount;
    dataCount = Activity::getDefault()->getDataCount(selection);
    if(dataCount.size() == 1)
    {
        for(const data::Object::sptr& obj : *selection)
        {
            data::Activity::sptr as = std::dynamic_pointer_cast<data::Activity>(obj);
            if(!as)
            {
                launchAS = false;
                break;
            }

            this->launchActivity(as);
            launchAS = true;
        }
    }

    return launchAS;
}

//------------------------------------------------------------------------------

void SLauncher::launchSeries(data::Series::sptr series)
{
    auto selection = std::make_shared<data::Vector>();
    selection->push_back(series);
    ActivityInfoContainer infos = Activity::getDefault()->getInfos(selection);

    if(m_quickLaunch.find(series->get_classname()) != m_quickLaunch.end())
    {
        std::string activityId = m_quickLaunch[series->get_classname()];
        SIGHT_ASSERT(
            "Activity information not found for" + activityId,
            Activity::getDefault()->hasInfo(activityId)
        );
        this->sendConfig(Activity::getDefault()->getInfo(activityId));
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

void SLauncher::launchActivity(data::Activity::sptr activity)
{
    ActivityInfo info;
    info = Activity::getDefault()->getInfo(activity->getActivityConfigId());

    // Applies activity validator on activity to check the data
    if(!info.validatorsImpl.empty())
    {
        for(const std::string& validatorImpl : info.validatorsImpl)
        {
            /// Process activity validator
            auto validator         = sight::activity::validator::factory::make(validatorImpl);
            auto activityValidator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);

            if(activityValidator)
            {
                auto ret = activityValidator->validate(activity);
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

    m_sigActivityLaunched->async_emit(message(activity, info, m_parameters));
}

//------------------------------------------------------------------------------

service::connections_t SLauncher::getAutoConnections() const
{
    connections_t connections;

    connections.push(s_SERIES, data::Vector::ADDED_OBJECTS_SIG, UPDATE_STATE_SLOT);
    connections.push(s_SERIES, data::Vector::REMOVED_OBJECTS_SIG, UPDATE_STATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity

/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SLauncher.hpp"

#include <activity/IActivityValidator.hpp>
#include <activity/IBuilder.hpp>
#include <activity/IValidator.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/Convert.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>
#include <core/thread/ActiveWorkers.hpp>
#include <core/tools/UUID.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>
#include <data/reflection/getObject.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/IAppConfigManager.hpp>
#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <boost/foreach.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVBoxLayout>

Q_DECLARE_METATYPE(sight::activity::extension::ActivityInfo)

namespace sight::module::ui::qt
{

namespace activity
{

//------------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SLauncher::s_LAUNCH_SERIES_SLOT          = "launchSeries";
const core::com::Slots::SlotKeyType SLauncher::s_LAUNCH_ACTIVITY_SERIES_SLOT = "launchActivitySeries";
const core::com::Slots::SlotKeyType SLauncher::s_UPDATE_STATE_SLOT           = "updateState";
const core::com::Signals::SignalKeyType SLauncher::s_ACTIVITY_LAUNCHED_SIG   = "activityLaunched";

using sight::activity::extension::Activity;
using sight::activity::extension::ActivityInfo;
using sight::activity::ActivityMsg;
using sight::activity::IValidator;
using sight::activity::IValidator;

//------------------------------------------------------------------------------

SLauncher::SLauncher() noexcept :
    m_mode("message")
{
    m_sigActivityLaunched = newSignal<ActivityLaunchedSignalType>(s_ACTIVITY_LAUNCHED_SIG);

    newSlot(s_LAUNCH_SERIES_SLOT, &SLauncher::launchSeries, this);
    newSlot(s_LAUNCH_ACTIVITY_SERIES_SLOT, &SLauncher::launchActivitySeries, this);
    newSlot(s_UPDATE_STATE_SLOT, &SLauncher::updateState, this);
}

//------------------------------------------------------------------------------

SLauncher::~SLauncher() noexcept
{
}

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
    this->sight::ui::base::IAction::initialize();
    typedef service::IService::ConfigType ConfigType;

    m_parameters.clear();
    if(this->getConfigTree().count("config") > 0)
    {
        SIGHT_ASSERT(
            "There must be one (and only one) <config/> element.",
            this->getConfigTree().count("config") == 1
        );

        const service::IService::ConfigType srvconfig = this->getConfigTree();
        const service::IService::ConfigType& config   = srvconfig.get_child("config");

        m_mode = config.get_optional<std::string>("mode").get_value_or("message");
        SIGHT_ASSERT(
            "SLauncher mode must be either 'immediate' or 'message'",
            "message" == m_mode || "immediate" == m_mode
        );

        if(config.count("parameters") == 1)
        {
            const service::IService::ConfigType& configParameters = config.get_child("parameters");
            BOOST_FOREACH(const ConfigType::value_type& v, configParameters.equal_range("parameter"))
            {
                ParametersType::value_type parameter(v.second);
                m_parameters.push_back(parameter);
            }
        }

        SIGHT_ASSERT("A maximum of 1 <parameters> tag is allowed", config.count("parameters") < 2);

        if(config.count("filter") == 1)
        {
            const service::IService::ConfigType& configFilter = config.get_child("filter");
            SIGHT_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

            const std::string mode = configFilter.get<std::string>("mode");
            SIGHT_ASSERT(
                "'" << mode << "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
                mode == "include" || mode == "exclude"
            );
            m_filterMode = mode;

            BOOST_FOREACH(const ConfigType::value_type& v, configFilter.equal_range("id"))
            {
                m_keys.push_back(v.second.get<std::string>(""));
            }
        }

        SIGHT_ASSERT("A maximum of 1 <filter> tag is allowed", config.count("filter") < 2);

        if(config.count("quickLaunch") == 1)
        {
            m_quickLaunch.clear();
            const service::IService::ConfigType& configQuickLaunch = config.get_child("quickLaunch");
            BOOST_FOREACH(const ConfigType::value_type& v, configQuickLaunch.equal_range("association"))
            {
                const service::IService::ConfigType& association = v.second;
                const service::IService::ConfigType xmlattr      = association.get_child("<xmlattr>");

                SIGHT_FATAL_IF("The attribute \"type\" is missing", xmlattr.count("type") != 1);
                SIGHT_FATAL_IF("The attribute \"id\" is missing", xmlattr.count("id") != 1);

                std::string type = xmlattr.get<std::string>("type");
                std::string id   = xmlattr.get<std::string>("id");

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

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString("Choose an activity"));

    QStandardItemModel* model = new QStandardItemModel(dialog);
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

        QStandardItem* item = new QStandardItem(QIcon(info.icon.c_str()), QString::fromStdString(text));
        item->setData(QVariant::fromValue(info));
        item->setEditable(false);
        model->appendRow(item);
    }

    QListView* selectionList = new QListView();
    selectionList->setIconSize(QSize(100, 100));
    selectionList->setUniformItemSizes(true);
    selectionList->setModel(model);

    QModelIndex index = model->index(0, 0);
    if(index.isValid())
    {
        selectionList->selectionModel()->select(index, QItemSelectionModel::Select);
    }

    QPushButton* okButton     = new QPushButton("Ok");
    QPushButton* cancelButton = new QPushButton("Cancel");

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(selectionList);
    vLayout->addLayout(hLayout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selectionList, SIGNAL(doubleClicked(const QModelIndex&)), dialog, SLOT(accept()));

    ActivityInfo info;
    if(dialog->exec())
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

        for(ActivityInfoContainer::const_iterator iter = infos.begin() ; iter != infos.end() ; ++iter)
        {
            KeysType::iterator keyIt = std::find(m_keys.begin(), m_keys.end(), iter->id);

            if(keyIt != m_keys.end() && isIncludeMode)
            {
                configs.push_back(*iter);
            }
            else if(keyIt == m_keys.end() && !isIncludeMode)
            {
                configs.push_back(*iter);
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
            sight::ui::base::dialog::MessageDialog::show(
                "Activity launcher",
                "Not available activity for the current selection.",
                sight::ui::base::dialog::MessageDialog::WARNING
            );
        }
    }
}

//------------------------------------------------------------------------------

void SLauncher::updateState()
{
    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    bool isExecutable = false;

    if(selection->size() == 1 && data::ActivitySeries::dynamicCast((*selection)[0]))
    {
        data::ActivitySeries::sptr as = data::ActivitySeries::dynamicCast((*selection)[0]);

        if(m_filterMode == "include" || m_filterMode == "exclude")
        {
            const bool isIncludeMode = m_filterMode == "include";

            KeysType::iterator keyIt = std::find(m_keys.begin(), m_keys.end(), as->getActivityConfigId());

            if(keyIt != m_keys.end() && isIncludeMode)
            {
                isExecutable = true;
            }
            else if(keyIt == m_keys.end() && !isIncludeMode)
            {
                isExecutable = true;
            }

            isExecutable &= Activity::getDefault()->hasInfo(
                as->getActivityConfigId()
            );
        }
        else
        {
            isExecutable = Activity::getDefault()->hasInfo(
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
            if(data::ActivitySeries::dynamicCast(obj))
            {
                isExecutable = true;
            }
        }

        ActivityInfoContainer infos = Activity::getDefault()->getInfos(selection.get_shared());
        infos = this->getEnabledActivities(infos);

        isExecutable |= !infos.empty();
    }

    this->setIsExecutable(isExecutable);
}

//------------------------------------------------------------------------------

void SLauncher::buildActivity(
    const ActivityInfo& info,
    const data::Vector::csptr& selection
)
{
    data::Composite::sptr replaceMap = data::Composite::New();
    auto builder                     = sight::activity::builder::factory::New(info.builderImpl);
    SIGHT_ASSERT(info.builderImpl << " instantiation failed", builder);

    data::ActivitySeries::sptr actSeries;
    actSeries = builder->buildData(info, selection);

    if(!actSeries)
    {
        const std::string msg = "The activity <" + info.title + "> can't be launched. Builder <" + info.builderImpl
                                + "> failed.";
        sight::ui::base::dialog::MessageDialog::show(
            "Activity can not be launched",
            msg,
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        SIGHT_ERROR(msg);
        return;
    }

    // Applies activity validator on activity series to check the data
    if(!info.validatorsImpl.empty())
    {
        for(std::string validatorImpl : info.validatorsImpl)
        {
            /// Process activity validator
            IValidator::sptr validator = sight::activity::validator::factory::New(validatorImpl);

            auto activityValidator = sight::activity::IActivityValidator::dynamicCast(validator);

            if(activityValidator)
            {
                IValidator::ValidationType validation = activityValidator->validate(actSeries);
                if(!validation.first)
                {
                    const std::string message = "The activity '" + info.title + "' can not be launched:\n"
                                                + validation.second;
                    sight::ui::base::dialog::MessageDialog::show(
                        "Activity launch",
                        message,
                        sight::ui::base::dialog::IMessageDialog::CRITICAL
                    );
                    return;
                }
            }
        }
    }

    ParametersType parameters = this->translateParameters(m_parameters);
    auto msg                  = ActivityMsg(actSeries, info, parameters);

    if(m_mode == "message")
    {
        m_sigActivityLaunched->asyncEmit(msg);
    }
    else
    {
        sight::ui::base::LockAction lock(this->getSptr());

        const std::string viewConfigID         = msg.getAppConfigID();
        ActivityMsg::ReplaceMapType replaceMap = msg.getReplaceMap();
        replaceMap["GENERIC_UID"] = service::extension::AppConfig::getUniqueIdentifier();

        service::IAppConfigManager::sptr helper = service::IAppConfigManager::New();
        helper->setConfig(viewConfigID, replaceMap);
        helper->launch();
        helper->stopAndDestroy();
    }
}

//------------------------------------------------------------------------------

void SLauncher::sendConfig(const ActivityInfo& info)
{
    // Start module containing the activity if it is not started
    std::shared_ptr<core::runtime::Module> module = core::runtime::findModule(info.bundleId);
    SIGHT_WARN_IF(
        "Module '" + info.bundleId + "' used by activity '" + info.id + "' is already started.",
        module->isStarted()
    );
    if(!module->isStarted())
    {
        SIGHT_DEBUG("Start module '" + info.bundleId + "' used by activity '" + info.id + "'");
        module->start();
    }

    const auto selection = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", selection);

    IValidator::ValidationType validation;
    validation.first = true;

    for(auto const& validatorImpl : info.validatorsImpl)
    {
        IValidator::sptr validator = sight::activity::validator::factory::New(validatorImpl);
        SIGHT_ASSERT(validatorImpl << " instantiation failed", validator);

        IValidator::ValidationType valid = validator->validate(info, selection.get_shared());
        validation.first &= valid.first;
        if(!valid.first)
        {
            validation.second += "\n" + valid.second;
        }
    }

    if(!validation.first)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Activity can not be launched",
            "The activity " + info.title + " can't be launched. Reason : " + validation.second,
            sight::ui::base::dialog::IMessageDialog::WARNING
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
        for(data::Object::sptr obj : *selection)
        {
            data::ActivitySeries::sptr as = data::ActivitySeries::dynamicCast(obj);
            if(!as)
            {
                launchAS = false;
                break;
            }
            else
            {
                this->launchActivitySeries(as);
                launchAS = true;
            }
        }
    }

    return launchAS;
}

//------------------------------------------------------------------------------

void SLauncher::launchSeries(data::Series::sptr series)
{
    data::ActivitySeries::sptr as = data::ActivitySeries::dynamicCast(series);
    if(as)
    {
        this->launchActivitySeries(as);
    }
    else
    {
        data::Vector::sptr selection = data::Vector::New();
        selection->getContainer().push_back(series);
        ActivityInfoContainer infos = Activity::getDefault()->getInfos(selection);

        if(m_quickLaunch.find(series->getClassname()) != m_quickLaunch.end())
        {
            std::string activityId = m_quickLaunch[series->getClassname()];
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
            sight::ui::base::dialog::MessageDialog::show(
                "Activity launcher",
                "Not available activity for the current selection.",
                sight::ui::base::dialog::MessageDialog::WARNING
            );
        }
    }
}

//------------------------------------------------------------------------------

void SLauncher::launchActivitySeries(data::ActivitySeries::sptr series)
{
    ActivityInfo info;
    info = Activity::getDefault()->getInfo(series->getActivityConfigId());

    // Applies activity validator on activity series to check the data
    if(!info.validatorsImpl.empty())
    {
        for(std::string validatorImpl : info.validatorsImpl)
        {
            /// Process activity validator
            IValidator::sptr validator = sight::activity::validator::factory::New(validatorImpl);

            auto activityValidator = sight::activity::IActivityValidator::dynamicCast(validator);

            if(activityValidator)
            {
                IValidator::ValidationType validation = activityValidator->validate(series);
                if(!validation.first)
                {
                    const std::string message = "The activity '" + info.title + "' can not be launched:\n"
                                                + validation.second;
                    sight::ui::base::dialog::MessageDialog::show(
                        "Activity launch",
                        message,
                        sight::ui::base::dialog::IMessageDialog::CRITICAL
                    );
                    return;
                }
            }
        }
    }

    ParametersType parameters = this->translateParameters(m_parameters);
    ActivityMsg msg           = ActivityMsg(series, info, parameters);

    m_sigActivityLaunched->asyncEmit(msg);
}

//------------------------------------------------------------------------------

SLauncher::ParametersType SLauncher::translateParameters(const ParametersType& parameters)
{
    ParametersType transParams = parameters;
    const auto workingObj      = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not correctly set.", workingObj);

    for(ParametersType::value_type& param : transParams)
    {
        if(param.isSeshat())
        {
            std::string parameterToReplace = param.by;
            if(parameterToReplace.substr(0, 1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            data::Object::sptr obj = data::reflection::getObject(workingObj.get_shared(), parameterToReplace);
            SIGHT_ASSERT("Invalid seshat path : '" << param.by << "'", obj);

            data::String::sptr stringParameter = data::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0, 1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }

            param.by = parameterValue;
        }
    }

    return transParams;
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SLauncher::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_SERIES, data::Vector::s_ADDED_OBJECTS_SIG, s_UPDATE_STATE_SLOT);
    connections.push(s_SERIES, data::Vector::s_REMOVED_OBJECTS_SIG, s_UPDATE_STATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace activity

} // namespace sight::module

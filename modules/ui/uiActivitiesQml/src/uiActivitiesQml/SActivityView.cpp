/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "uiActivitiesQml/SActivityView.hpp"

#include <activities/registry/Activities.hpp>

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/operations.hpp>

#include <data/Composite.hpp>

#include <gui/dialog/MessageDialog.hpp>

#include <services/macros.hpp>

namespace uiActivitiesQml
{

fwServicesRegisterMacro( ::fwQml::IQmlEditor, ::uiActivitiesQml::SActivityView )

static const core::com::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG = "activityLaunched";

static const core::com::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SLOT        = "launchActivity";
static const core::com::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SERIES_SLOT = "launchActivitySeries";

//------------------------------------------------------------------------------

SActivityView::SActivityView()
{
    m_sigActivityLaunched = newSignal< ActivityLaunchedSignalType >(s_ACTIVITY_LAUNCHED_SIG);
    newSlot(s_LAUNCH_ACTIVITY_SLOT, &SActivityView::launchActivity, this);
    newSlot(s_LAUNCH_ACTIVITY_SERIES_SLOT, &SActivityView::launchActivitySeries, this);
}

//------------------------------------------------------------------------------

SActivityView::~SActivityView()
{
}

//------------------------------------------------------------------------------

void SActivityView::configuring()
{
    const ConfigType config = this->getConfigTree();

    this->parseConfiguration(config, this->getInOuts());
}

//------------------------------------------------------------------------------

void SActivityView::starting()
{
}

//------------------------------------------------------------------------------

void SActivityView::stopping()
{

}

//------------------------------------------------------------------------------

void SActivityView::updating()
{
}

//------------------------------------------------------------------------------

void SActivityView::notifyActivityCreation()
{
    m_sigActivityLaunched->asyncEmit();
}

//------------------------------------------------------------------------------

void SActivityView::launchActivity(data::ActivitySeries::sptr activitySeries)
{
    bool isValid;
    std::string message;

    // check if the activity can be launched
    std::tie(isValid, message) = this->validateActivity(activitySeries);

    if (isValid)
    {
        activities::registry::ActivityInfo info;
        info = activities::registry::Activities::getDefault()->getInfo(activitySeries->getActivityConfigId());

        std::shared_ptr< core::runtime::Module > module = core::runtime::findModule(info.bundleId, info.bundleVersion);
        SLM_INFO_IF("Module '" + module->getIdentifier() + "' (used for '" + info.appConfig.id + "') is already "
                    "started !", module->isStarted())
        if (!module->isStarted())
        {
            module->start();
        }

        // get Activity path, it allows to retrieve the associated Qml file
        const auto path = core::runtime::getModuleResourceFilePath(info.bundleId, info.appConfig.id + ".qml");

        ReplaceMapType replaceMap;
        this->translateParameters(m_parameters, replaceMap);
        this->translateParameters(activitySeries->getData(), info.appConfig.parameters, replaceMap);
        replaceMap["AS_UID"] = activitySeries->getID();

        // convert the replaceMap to Qt Map to send it to Qml(only QVariantMap type is implemented in Qml)
        QVariantMap qReplaceMap;
        for (const auto& elt: replaceMap)
        {
            QString replace = QString::fromStdString(elt.first);
            QString by      = QString::fromStdString(elt.second);
            qReplaceMap.insert(replace, by);

        }
        Q_EMIT launchRequested(QUrl::fromLocalFile(QString::fromStdString(path.string())), qReplaceMap);
    }
    else
    {
        gui::dialog::MessageDialog::show("Activity launch",
                                         message,
                                         gui::dialog::IMessageDialog::CRITICAL);
    }
}

//------------------------------------------------------------------------------

void SActivityView::launchActivitySeries(data::Series::sptr series)
{
    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::dynamicCast(series);
    if (activitySeries)
    {
        this->launchActivity(activitySeries);
    }
}

//------------------------------------------------------------------------------

}// namespace uiActivitiesQml

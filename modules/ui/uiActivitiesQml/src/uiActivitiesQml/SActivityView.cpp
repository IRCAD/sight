/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <fwActivities/registry/Activities.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

namespace uiActivitiesQml
{

fwServicesRegisterMacro( ::fwQml::IQmlEditor, ::uiActivitiesQml::SActivityView );

static const fwCom::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG = "activityLaunched";

static const ::fwCom::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SLOT        = "launchActivity";
static const ::fwCom::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SERIES_SLOT = "launchActivitySeries";

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

void SActivityView::launchActivity(::fwMedData::ActivitySeries::sptr activitySeries)
{
    bool isValid;
    std::string message;

    // check if the activity can be launched
    std::tie(isValid, message) = this->validateActivity(activitySeries);

    if (isValid)
    {
        ::fwActivities::registry::ActivityInfo info;
        info = ::fwActivities::registry::Activities::getDefault()->getInfo(activitySeries->getActivityConfigId());

        std::shared_ptr< ::fwRuntime::Bundle > bundle = ::fwRuntime::findBundle(info.bundleId,
                                                                                info.bundleVersion);
        SLM_INFO_IF("Bundle '" + bundle->getIdentifier() + "' (used for '" + info.appConfig.id + "') is already "
                    "started !", bundle->isStarted())
        if (!bundle->isStarted())
        {
            bundle->start();
        }

        // get Activity path, it allows to retrieve the associated Qml file
        const auto path = ::fwRuntime::getBundleResourceFilePath(info.bundleId, info.appConfig.id + ".qml");

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
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity launch",
                                                          message,
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
    }
}

//------------------------------------------------------------------------------

void SActivityView::launchActivitySeries(fwMedData::Series::sptr series)
{
    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(series);
    if (activitySeries)
    {
        this->launchActivity(activitySeries);
    }
}

//------------------------------------------------------------------------------

}// namespace uiActivitiesQml
